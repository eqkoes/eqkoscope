#include "eqkoscope.h"
#include <string>

int NB_MIDI_PORTS = 8;

int lastMidiAftertouch= 0;

int clockIndex = 0;
vector<int> MIDIClockDates;

void eqkoscope::sendControlChange(int channel, int control, int value){
    if(midiControlValues[channel-1][control-1]!=value){
        midiControlValues[channel-1][control-1] = value;
        midiOut.sendControlChange(channel, control, value);
    }
}

void eqkoscope::initMidi(){
    try{
    loadMapping(ofBufferFromFile(ofFilePath::getCurrentWorkingDirectory()+"/../../../"+MIDIMapPath).getText(), true, true);
    
    launchpadOut = ofxMidiOut("Launchpad S");
    launchpadOut.openPort("Launchpad S");
    
    openPorts();
    
    midiOut.openVirtualPort();
    
    midiOutToLive.openVirtualPort();
    
    for(int i=0;i<16;i++){
        vector<int> chan;
        for(int j=0;j<127;j++)
            chan.push_back(4);
        midiControlValues.push_back(chan);
    }
    }catch(exception e){
        ;
    }
}

void eqkoscope::initOfflineMIDI(){
    SimpleMidiReader smr;
    MidiSequence seq;
        if (smr.read("/Users/Raph/Sons/Selected\ Drum\ Banks\ For\ Superior\ \&\ EZ\ Drummer/Midi/000601@ANDY_JOHNSON/021@CollapsingDam/1_bar_fill_01.mid", seq)) {
            std::cout << " format: " << smr.format << ", nb_tracks=" << smr.nb_tracks << "\n";
            for (size_t j=0; j < seq.evts.size(); ++j) {
                const MidiSequence::Event &ev = seq.evts[j];
                vector<unsigned char> rawBytes;
                for(int b=0;b<ev.midi.size();b++)
                    rawBytes.push_back(ev.midi[b]);
                ofxMidiMessage mid(&rawBytes);
                mid.status = (MidiStatus) rawBytes[0];
                mid.pitch = rawBytes[1];
                mid.control = rawBytes[1];
                mid.value = rawBytes[2];
                mid.velocity = rawBytes[2];
                mid.deltatime = ev.when; //hack
                mid.channel = (rawBytes[0] & 0x0F) + 1;
                offlineMsgs.push_back(mid);
                offlineMsgDates.push_back(ev.when*1000);
                printf("tick %5d track %d %8.4fs ", (int)ev.tick, (int)ev.track, ev.when);
                for (size_t k=0; k < ev.midi.size(); ++k)
                    printf(" %02x", ev.midi[k]);
                printf("\n");
            }
        }
}

void eqkoscope::updateOfflineMIDI(){
    offlineDate += 1000.0/restrictFrameRate;
        while(offlineMsgDates[offlineMsgsIndex] <= offlineDate && offlineMsgsIndex<offlineMsgs.size()){
            newMidiMessage(offlineMsgs[offlineMsgsIndex]);
            offlineMsgsIndex++;
        }
    
    offlineMsgsIndex;
    
//    ofxMidiMessage mid(bytes);
//    FILE *f = fopen("/Users/Raph/Downloads/jdksmidi-master/songs/test_midi/test6.mid", "r");

}

void eqkoscope::resetMidi(){
    launchpadOut.openPort("Launchpad S");
    
    //    ofxMidiIn::listPorts(); //todo exception handler
    openPorts();
}

void eqkoscope::openPorts(){
    midiIns.clear();
    std::vector<int> ports;
    for(int i=0;i<NB_MIDI_PORTS;i++)
        ports.push_back(i);
    
    for(int i=0;i<ports.size();i++){
        ofxMidiIn in = ofxMidiIn();
        in.setVerbose(true);
        in.openPort(ports[i]);
        if(in.isOpen()){
            if(in.getName().compare("Komplete Audio 6") //forbidden port names
               //               && in.getName().compare("nanoKONTROL2 SLIDER/KNOB")
               //&& in.getName().compare("APC Key 25")
               ){
                cout << "midi port opened " << in.getName() << endl;
                in.addListener(this);
                in.ignoreTypes(false, false, false);
                midiIns.push_back(in);
                cout << "MIDI PORT OK " << in.getName() << endl;
                if(!in.getName().compare("Launchpad S"))
                    is_launchpad = true;
                if(strStartsWith(in.getName(), "SL MkII Port"))
                    is_novation = true;
                if(!in.getName().compare(MPD_DEVICE) || !in.getName().compare("Akai MPD24"))
                    is_io2 = true;
                if(!in.getName().compare(EXTERNAL_MIDI_DEVICE))
                        is_external_device = true;
            }else{
                in.closePort();
            }
        }
    }
}

void eqkoscope::newMidiMessage(ofxMidiMessage& eventArgs){
#ifndef SLAVE
    tcpClient.sendRawMsg((char*)(&eventArgs), sizeof(ofxMidiMessage));
//    cout << "send tcp mess " << ofGetFrameNum() << endl;
    return;
#endif
    
    
 if(eventArgs.channel == 16)
        return;
    
    if(parameterMap[bypassCTRL] && !eventArgs.portName.compare("Launch Control XL") //hack for bypass with launch XL
       && eventArgs.pitch!=92)
        return;
    
    /********* MIDI THROUGH **********/
    //    if(eventArgs.channel==15 && eventArgs.status==MIDI_NOTE_ON){
    //        midiOut.sendNoteOn(eventArgs.channel, eventArgs.pitch);
    //        return;
    //    }
    
    /********* MIDI CLOCK **********/
    if(eventArgs.status==MIDI_TIME_CLOCK){
    if(parameterMap[bpmLock]>0){
        if(clockIndex%(4*24*4)==0){ //reset each four bars
            for(int i=0;i<timedAutos.size();i++)
                timedAutos[i]->uelapsed = 0;
            clockIndex++;
        }
        MIDIClockDates.push_back(ofGetElapsedTimeMicros());
        if(MIDIClockDates.size()>50)
            MIDIClockDates.erase(MIDIClockDates.begin());
        if(MIDIClockDates.size()==50){
            long d=0;
            for(int i=1;i<MIDIClockDates.size();i++){
                d += MIDIClockDates[i] - MIDIClockDates[i-1];
            }
            d /= float(MIDIClockDates.size()-1);
            parameterMap[bpm] = deltaMap[bpm] = round(60*1000000/(d*24.0));
        }
    }
        return;
    }
    
    /****** ACTION MAPS *******/
    
    bool override = false;
    int type=-1;
    int id;
    int channel = eventArgs.channel;
    float value;
    switch(eventArgs.status){
        case MIDI_CONTROL_CHANGE: {
            type = MIDI_CC;
            id = eventArgs.control;
            value = eventArgs.value / 127.0;
        }break;
        case MIDI_NOTE_ON:{
            type = MIDI_NOTEON;
            id = eventArgs.pitch;
            value = eventArgs.velocity / 127.0;
        }break;
        case MIDI_NOTE_OFF:{
            type = MIDI_NOTEOFF;
            id = eventArgs.pitch;
            value = 1;
        }break;
        default:;
    }
    
    /** MACROS **/
    
    if(eventArgs.portName.compare("Launchpad S")){
        
        if(type==MIDI_NOTEON && id==120){ // C8
            loadNextMacro();
            return;
        }
        
        if(type==MIDI_NOTEON && id==119){ // B8
            loadPrevMacro();
            return;
        }
        
        if(type==MIDI_NOTEON && id==118){ // A#7
            loadFirstMacro();
            return;
        }
        
    switch(eventArgs.status){
        case MIDI_NOTE_ON:{
            for(int i=0;i<midiNoteonAutos.size();i++){
                if(midiNoteonAutos[i]->channel == channel && ((midiNoteonAutos[i]->minId<= eventArgs.pitch && midiNoteonAutos[i]->maxId>= eventArgs.pitch) || midiNoteonAutos[i]->minId==0)){
                    if(midiNoteonAutos[i]->minId != midiNoteonAutos[i]->maxId){
                        midiNoteonAutos[i]->update(ofMap(eventArgs.pitch, midiNoteonAutos[i]->minId, midiNoteonAutos[i]->maxId, 0, 1));
                    }else{
                        midiNoteonAutos[i]->update(eventArgs.velocity/127.0);
                    }
                    override = true;
                }
            }
            if(eventArgs.portNum!=-2 && noteAutoFastMap[channel-1][id-1].size()>0){ //factory
                for(Auto* a : noteAutoFastMap[channel-1][id-1]){
                    if(a->minId != a->maxId){
                        a->update(ofMap(eventArgs.pitch, a->minId, a->maxId, 0, 1));
                    }else{
                        a->update(eventArgs.velocity/127.0);
                    }
                }
            }
        } break;
        case MIDI_NOTE_OFF:{
            for(int i=0;i<midiNoteonAutos.size();i++){
                if(midiNoteonAutos[i]->hold && midiNoteonAutos[i]->channel == channel && ((midiNoteonAutos[i]->minId<= eventArgs.pitch && midiNoteonAutos[i]->maxId>= eventArgs.pitch) || midiNoteonAutos[i]->minId==0)){
                    midiNoteonAutos[i]->update(0);
                    override = true;
                }
            }
            if(eventArgs.portNum!=-2 && noteAutoFastMap[channel-1][id-1].size()>0){ //factory
                for(Auto* a : noteAutoFastMap[channel-1][id-1]){
                    if(a->hold){
                        a->update(0);
                        override = true;
                    }
                }
            }
        } break;
        case MIDI_CONTROL_CHANGE:{
            for(int i=0;i<midiCCAutos.size();i++){
                if(midiCCAutos[i]->channel == channel && midiCCAutos[i]->minId == id){
                    midiCCAutos[i]->update(eventArgs.value/127.0);
                    override = true;
                }
            }
            if(eventArgs.portNum!=-2 && ccAutoFastMap[channel-1][id-1].size()>0){ //factory
                for(Auto* a : ccAutoFastMap[channel-1][id-1]){
                    a->update(eventArgs.value/127.0);
                    //                        override = true;
                }
            }
        }break;
        default:;
    }
    }
    
    if(!eventArgs.portName.compare("IAC Driver Bus 1") || !eventArgs.portName.compare(EXTERNAL_MIDI_DEVICE)) // do not process musical MIDI automations
        return;
    
    if(!override){
        midiMutex.lock();
        midiMsgs.push_back(eventArgs);
        midiMutex.unlock();
    }
    
    if(eventArgs.channel == 8 && eventArgs.control==104){
        audioAutos.clear();
        timedAutos.clear();
        leapAutos.clear();
        oscAutos.clear();
        midiCCAutos.clear();
        midiNoteonAutos.clear();
        
        initParameters();
        loadMapping(ofBufferFromFile(ofFilePath::getCurrentWorkingDirectory()+"/../../../"+MIDIMapPath).getText(), true, true);
        loadMacroMap();
        
        analyzeMacros();
        
        for(int i=0;i<N_PARAM;i++){
            deltaMap[i] = parameterMap[i];
        }
        return;
    }
}

void eqkoscope::parseMidi(ofxMidiMessage eventArgs){
#ifdef MPD24
    if(eventArgs.status==MIDI_CONTROL_CHANGE && eventArgs.channel==3 &&
       eventArgs.control==7)
        eventArgs.control = 9;
#endif
    
    if(ofGetFrameNum()<3)
        return;
    
    if(parameterMap[bypassCTRL] && !eventArgs.portName.compare("Launch Control XL") //hack for bypass with launch XL
       && eventArgs.pitch!=92)
        return;
    
    if(eventArgs.status==MIDI_START){
        clockIndex=0;
        return;
    }
    
    if(eventArgs.status==MIDI_AFTERTOUCH && eventArgs.channel==2){
        //            cout << "aftertouc " << eventArgs.pitch << endl;
        //            liveMidiEffect(lastMidiAftertouch, eventArgs.value/127.0, true);
        
        //            ofxMidiMessage msg;
        //            msg.pitch = eventArgs.pitch;
        //            msg.status = MIDI_NOTE_ON;
        //            msg.velocity = eventArgs.velocity;
        //            newMidiMessage(msg);
        return;
    }
    
    if(eventArgs.status==MIDI_CONTROL_CHANGE && eventArgs.channel==assignChannel && eventArgs.control==100){
        deltaMap[featuredParameter] = parameterMap[featuredParameter] = eventArgs.value / 127.0;
        return;
    }
    
    if(eventArgs.status==MIDI_NOTE_ON && eventArgs.channel==1 && eventArgs.pitch==91){
        eraseControlMapping(false); //kill all user automations
        return;
    }
    
    if(!eventArgs.portName.compare("IAC Driver Bus 1")){
        int type=-1;
        int id;
        float value;
        switch(eventArgs.status){
            case MIDI_CONTROL_CHANGE: {
                type = MIDI_CC;
                id = eventArgs.control;
                value = eventArgs.value / 127.0;
            }break;
            case MIDI_NOTE_ON: {
                type = MIDI_NOTEON;
                id = eventArgs.pitch;
                value = eventArgs.velocity / 127.0;
            }break;
            case MIDI_NOTE_OFF:{
                type = MIDI_NOTEOFF;
                id = eventArgs.pitch;
                value = 1;
            }break;
            default:;
        }
        
        if(type==MIDI_CONTROL_CHANGE && id==100){
            extAutoDimmer = value/127.0;
            return;
        }
        
        if(type==MIDI_NOTEON && id==120){ // C8
            loadNextMacro();
            return;
        }
        
        if(type==MIDI_NOTEON && id==119){ // B7
            loadFirstMacro();
            return;
        }
        
        if(type==MIDI_NOTEON && id==118){ // A7#
            reloadMacro();
            return;
        }
        
        
        switch(eventArgs.status){
            case MIDI_NOTE_ON:{
                for(int i=0;i<midiNoteonAutos.size();i++)
                    if(midiNoteonAutos[i]->channel == eventArgs.channel && (midiNoteonAutos[i]->minId == eventArgs.pitch || midiNoteonAutos[i]->minId==0))
                        midiNoteonAutos[i]->update(eventArgs.velocity/127.0);
                
            } break;
            case MIDI_NOTE_OFF:{
                for(int i=0;i<midiNoteonAutos.size();i++)
                    if(midiNoteonAutos[i]->channel == eventArgs.channel && (midiNoteonAutos[i]->minId == eventArgs.pitch || midiNoteonAutos[i]->minId==0))
                        midiNoteonAutos[i]->update(0);
                
            } break;
            case MIDI_CONTROL_CHANGE:{
                for(int i=0;i<midiCCAutos.size();i++)
                    if(midiCCAutos[i]->channel == eventArgs.channel && midiCCAutos[i]->minId == eventArgs.control)
                        midiCCAutos[i]->update(eventArgs.value/127.0);
            } break;
            default:;
        }
        return;
    }
    
    
    if(eventArgs.channel == 1 || eventArgs.channel == 3 || eventArgs.channel == 4 || (eventArgs.channel == 2 && eventArgs.control==91)){ //regular interfaces
        if(scenes[parameterMap[currentScene]]==feedbackScene && parameterMap[embedUzi] && eventArgs.pitch>=52){
            eventArgs.pitch -= 16;
            uzi->midiEvent(eventArgs);
        }else{
            bool alluzis = false;
            if(parameterMap[triumMode]!=0){
                alluzis = true;
                for(int i=0;i<2;i++){
                    bool u = false;
                    for(int j=0;j<uzis.size();j++)
                        if(scenes[i]==uzis[j])
                            u = true;
                    if(scenes[i]==uzi)
                        u = true;
                    if(!u){
                        alluzis = false;
                        break;
                    }
                }
                if(alluzis){
                    if(scenes[0]!=0)
                        scenes[0]->midiEvent(eventArgs);
                    if(scenes[1]!=0)
                        scenes[1]->midiEvent(eventArgs);
                    if(scenes[2]!=0)
                        scenes[2]->midiEvent(eventArgs);
                }
            }
            if(!alluzis)
                if(scenes[parameterMap[currentScene]]!=0)
                    scenes[parameterMap[currentScene]]->midiEvent(eventArgs);
        }
    }
    
    float value = eventArgs.value;
    switch(eventArgs.status){
        case MIDI_CONTROL_CHANGE:{
            logfile << ofGetElapsedTimeMillis() << " MIDI CC " << eventArgs.control << " " << value << endl;
            switch(eventArgs.channel){
                case 1 :{
                    if(!eventArgs.portName.compare("Launchpad S")){
                        switch(eventArgs.control){
                            case 104:{
                                if(value==127)             setMacroPage(macroPage - 1);
                                
                            }break;
                            case 105:{
                                if(value==127)             setMacroPage(macroPage + 1);
                                
                            }break;
                            case 107:{
                                if(value==127)
                                    loadNextMacro();
                            }break;
                        }
                    }
                }break;
                case 2:{
                    switch(eventArgs.control){
                        case 1:
                            modWheel = value;
                            break;
                    }
                }break;
                case 3:{
                    switch(eventArgs.control){
                        case 100:
                            currentParamIntensity = value/127.0f;
                            break;
                    }
                }
            }
        }break;
        case MIDI_NOTE_ON:{
            //                if(eventArgs.pitch!=0)
            //                    logfile << ofGetElapsedTimeMillis() <<" MIDI NOTEON " << eventArgs.pitch << " " << value << endl;
            int pitch = eventArgs.pitch;
            switch(eventArgs.channel){
                case 1 :{
                    if(!eventArgs.portName.compare("Launchpad S") &&
                       ((pitch>=0 && pitch<=8) || (pitch>=16 && pitch<=24)
                        || (pitch>=32 && pitch<=40) || (pitch>=48 && pitch<=56)
                        || (pitch>=64 && pitch<=72) || (pitch>=80 && pitch<=88)
                        || (pitch>=96 && pitch<=104) || (pitch>=112 && pitch<=120))){
                           pitch += 128*macroPage;
                           string path = getMacroFromMIDI(pitch);
                           if(path.empty())
                               break;
                           setMacroCode(pitch);
                           if(eventArgs.velocity>0){ // A FAIRE POUR LE RESTE
                               if(saveMacros){
                                   saveMacro(path);
                                   saveMacros = false;
                               }else{
                                   loadMacro(path);
                               }
                           }
                       }
                }break;
                case 2:{ // MPD 32
                    switch(pitch){
                        default:
                            if(pitch>=84 && pitch < 100)
                                liveMidiEffect(pitch, eventArgs.velocity/127.0, true);
                    }break;
                }break;
                    
            }
        }break;
        case MIDI_NOTE_OFF:{
            int pitch = eventArgs.pitch;
            logfile << ofGetElapsedTimeMillis() << " MIDI NOTEOFF " << eventArgs.pitch << " " << value << endl;
            switch(eventArgs.channel){
                case 3 :{
                    switch(pitch){
                        default:
                            if(pitch>=84 && pitch < 100)
                                liveMidiEffect(pitch, eventArgs.velocity/127.0,  false);
                    }
                    
                }break;
            }
        }break;
        default:;
    }
}

void eqkoscope::liveMidiEffect(int code, float intensity, bool on){
    if(on)
        lastMidiAftertouch = code;
    else
        lastMidiAftertouch = 0;
}

void eqkoscope::updateMidi(){
    if(ofGetFrameNum()%100==0){
        is_io2 = is_external_device = is_novation = is_launchpad = false;
        for(int i=0;i<NB_MIDI_PORTS;i++){
            try{
                ofxMidiIn in = ofxMidiIn();
                in.setVerbose(false);
                in.openPort(i);
                string name = in.getName();
                if(enforce_launchControl)
                    if(in.isOpen() && (!name.compare(LAUNCHCONTROL_DEVICE)))
                        is_launchControl = true;
                if(enforce_mpd24)
                    if(in.isOpen() && (!name.compare(MPD24_DEVICE)))
                        is_mpd24 = true;
                if(enforce_io2)
                    if(in.isOpen() && (!name.compare(MPD_DEVICE) || !name.compare("Akai MPD24")))
                        is_io2 = true;
                if(enforce_external_device)
                    if(in.isOpen() && (!name.compare(EXTERNAL_MIDI_DEVICE)))
                        is_external_device = true;
                if(enforce_novation)
                    if(in.isOpen() && strStartsWith(name, "SL MkII Port"))
                        is_novation = true;
                if(enforce_launchpad)
                    if(in.isOpen() && !name .compare("Launchpad S"))
                        is_launchpad = true;
                if(enforce_nano)
                    if(in.isOpen() && !name.compare("nanoKONTROL2 SLIDER/KNOB"))
                        is_nano = true;
            }catch(exception e){}
        }
        if((enforce_mpd24 && !is_mpd24) ||(enforce_launchControl && !is_launchControl) ||
           (enforce_io2 && !is_io2) || (enforce_novation && !is_novation) || (enforce_launchpad && !is_launchpad) || (enforce_nano && !is_nano) || (enforce_external_device && !is_external_device)){
            openPorts();
        }
    }
    programChangeFlag = 0;
}
