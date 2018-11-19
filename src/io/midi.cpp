#include "eqkoscope.h"
#include <string>

int NB_MIDI_PORTS = 5;

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
        loadMappingFiles(MIDIMapPath, true, true);
        ofFile midiLearntFile("midiLearnt.csv");
        if(midiLearntFile.exists()){
            string str = ofBufferFromFile("midiLearnt.csv").getText();
            loadMapping(str, true, false);
        }else
            midiLearntFile.create();
    
    launchpadOut = ofxMidiOut(MACRO_INTERFACE);
    launchpadOut.openPort(MACRO_INTERFACE);
    
        gerardpadOut = ofxMidiOut(GERARD_INTERFACE);
        gerardpadOut.openPort(GERARD_INTERFACE);
        
    openPorts(true);
    
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
    offlineDate += 1000.0/maxFrameRate;
        while(offlineMsgDates[offlineMsgsIndex] <= offlineDate && offlineMsgsIndex<offlineMsgs.size()){
            newMidiMessage(offlineMsgs[offlineMsgsIndex]);
            offlineMsgsIndex++;
        }
    
    offlineMsgsIndex;
    
//    ofxMidiMessage mid(bytes);
//    FILE *f = fopen("/Users/Raph/Downloads/jdksmidi-master/songs/test_midi/test6.mid", "r");

}

void eqkoscope::resetMidi(){
   // launchpadOut.openPort(MACRO_INTERFACE);
    openPorts(true);
}

void eqkoscope::openPorts(bool init){
        
    if(init){
        for(int i=0;i<midiIns.size();i++){
            midiIns[i].closePort();
        }
            midiIns.clear();
            for(int i=0;i<NB_MIDI_PORTS;i++){
                ofxMidiIn in = ofxMidiIn();
                in.setVerbose(false);
                in.addListener(this);
                in.ignoreTypes(false, false, false);
                midiIns.push_back(in);
        }
    }
    
    ///check if ports are opened. If not open'em !
    
    try{
    for(int i=0;i<NB_MIDI_PORTS;i++){
//        ofxMidiIn in = ofxMidiIn();
        ofxMidiIn *in = &(midiIns[i]);
//        in.setVerbose(false);
        
        in->closePort(); //bourrin...
        if( ! in->isOpen() ){
            cout << "OPENING MIDI PORT " << in->getName() << " ... ";
            in->openPort(i);
            
            if(!string(MACRO_INTERFACE).empty() && !in->getName().compare(MACRO_INTERFACE)){
//                if(!launchpadOut.isOpen()){
                    launchpadOut.openPort(MACRO_INTERFACE);
                    if(launchpadOut.isOpen())
                        paintMacros();
//                }
            }
        }
 
        if( in->isOpen() ){
//                in.addListener(this);
//                in->ignoreTypes(false, false, false);
//                midiIns.push_back(in);
                cout << "MIDI PORT OPENED " << in->getName() << endl;
                MIDI_device_connected[in->getName()] = true;
    }
        }
    }catch(exception e){
        cout << e.what() << endl;
    }
}

//gruesomly bourrin !
void eqkoscope::makeSureMIDIOutDeviceIsOpen(){
    
//    try{
//        
//        for(int i=0;i<midiIns.size();i++){
//            ofxMidiIn *in = &(midiIns[i]);
//            string name = convertMIDIName(in->getName());
//            // if(!name.empty() &&  ! in->isOpen() ){
//            //                cout << "OPENING MIDI PORT " << name << " ... ";
//            in->openPort(i);
//            //MIDI_device_connected[name] = false;
//            //}
//            if( in->isOpen() ){
//                //                cout << "MIDI PORT OPENED " << name << endl;
//                MIDI_device_connected[name] = true;
//            }
//        }
//    }catch(exception e){
//        cout << e.what() << endl;
//    }

    launchpadOut.openPort(MACRO_INTERFACE);

    paintMacros();
    
}

void eqkoscope::newMidiMessage(ofxMidiMessage& eventArgs){
    
    if(!eventArgs.portName.compare("MIDI Guitar") || !eventArgs.portName.compare("IAC Driver Bus 1"))
        eventArgs.channel = 10;
    
    if(MIDI_check_listen && eventArgs.portNum<MIDI_MAX_NB_PORTS && !MIDI_check_ports[eventArgs.portNum]){
        MIDI_check_ports[eventArgs.portNum] = true;
        MIDI_check_portNames[eventArgs.portNum] = eventArgs.portName;
    }
    
    
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
    
   /* if(eventArgs.portName.compare("Launchpad S")){
        
        if(type==MIDI_NOTEON && id==120){ // C8
            loadNextMacro();
            return;
        }
        
        if(type==MIDI_NOTEON && id==119){ // B8
            loadPrevMacro();
            return;
        }
        
        if(type==MIDI_CC && id==105 && value>0){ // C8
            loadNextMacro();
            return;
        }
        
        if(type==MIDI_CC && id==104 && value>0){ // B8
            loadPrevMacro();
            return;
        }
        
        if(type==MIDI_NOTEON && id==118){ // A#7
            loadFirstMacro();
            return;
        }*/
        
    switch(eventArgs.status){
        case MIDI_NOTE_ON:{
            for(int i=0;i<midiNoteonAutos.size();i++){
                if((midiNoteonAutos[i]->channel==0 || midiNoteonAutos[i]->channel == channel) && ((midiNoteonAutos[i]->minId<= eventArgs.pitch && midiNoteonAutos[i]->maxId>= eventArgs.pitch) || midiNoteonAutos[i]->minId==0)){
                    if(midiNoteonAutos[i]->minId != midiNoteonAutos[i]->maxId){
                        midiNoteonAutos[i]->update(ofMap(eventArgs.pitch, midiNoteonAutos[i]->minId, midiNoteonAutos[i]->maxId, 0, 1));
                    }else{
                        midiNoteonAutos[i]->update(eventArgs.velocity/127.0);
                    }
                    override = true;
                }
            }
            if(eventArgs.portNum!=-2 && noteAutoFastMap[channel][id-1].size()>0){ //factory
                for(Auto* a : noteAutoFastMap[channel][id-1]){
                    if(a->minId != a->maxId){
                        a->update(ofMap(eventArgs.pitch, a->minId, a->maxId, 0, 1));
                    }else{
                        a->update(eventArgs.velocity/127.0);
                    }
                }
            }
            if(eventArgs.portNum!=-2 && noteAutoFastMap[0][id-1].size()>0){ //factory
                for(Auto* a : noteAutoFastMap[0][id-1]){
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
                if(midiNoteonAutos[i]->hold && (midiNoteonAutos[i]->channel==0 || midiNoteonAutos[i]->channel == channel) && ((midiNoteonAutos[i]->minId<= eventArgs.pitch && midiNoteonAutos[i]->maxId>= eventArgs.pitch) || midiNoteonAutos[i]->minId==0)){
                    midiNoteonAutos[i]->update(0);
                    override = true;
                }
            }
            if(eventArgs.portNum!=-2 && noteAutoFastMap[channel][id-1].size()>0){ //factory
                for(Auto* a : noteAutoFastMap[channel][id-1]){
                    if(a->hold){
                        a->update(0);
                        override = true;
                    }
                }
            }
            if(eventArgs.portNum!=-2 && noteAutoFastMap[0][id-1].size()>0){ //factory
                for(Auto* a : noteAutoFastMap[0][id-1]){
                    if(a->hold){
                        a->update(0);
                        override = true;
                    }
                }
            }

        } break;
        case MIDI_CONTROL_CHANGE:{
            for(int i=0;i<midiCCAutos.size();i++){
                if((midiCCAutos[i]->channel==0 || midiCCAutos[i]->channel == channel) && midiCCAutos[i]->minId == id){
                    midiCCAutos[i]->update(eventArgs.value/127.0);
                    override = true;
                }
            }
            if(eventArgs.portNum!=-2 && ccAutoFastMap[channel][id-1].size()>0){ //factory
                for(Auto* a : ccAutoFastMap[channel][id-1]){
                    a->update(eventArgs.value/127.0);
                    //                        override = true;
                }
            }
            if(eventArgs.portNum!=-2 && ccAutoFastMap[0][id-1].size()>0){ //factory
                for(Auto* a : ccAutoFastMap[0][id-1]){
                    a->update(eventArgs.value/127.0);
                    //                        override = true;
                }
            }
        }break;
        default:;
    }
//    }
    
    
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
        loadMappingFiles(MIDIMapPath, true, true);
        loadMacroMap();
        
        analyzeMacros();
        
        for(int i=0;i<N_PARAM;i++){
            deltaMap[i] = parameterMap[i];
        }
        return;
    }
}

void eqkoscope::parseMidi(ofxMidiMessage eventArgs){
    
    if(ofGetFrameNum()<3)
        return;
    
    /// HACKS AND SPECIAL STUFF
    
    if(parameterMap[bypassCTRL] && !eventArgs.portName.compare("Launch Control XL") //hack for bypass with launch XL
       && eventArgs.pitch!=92)
        return;
    
    if(eventArgs.status==MIDI_START){
        clockIndex=0;
        return;
    }
    
 /*   if(eventArgs.status==MIDI_NOTE_ON && eventArgs.channel==1 && eventArgs.pitch==91){
        eraseControlMapping(false); //kill all user automations
        return;
    }*/
    
    
    if(midiLearn >= 0){
        ofFile midiLearnFile("midiLearnt.csv", ofFile::ReadWrite);
        
        Auto* a = new Auto(this, midiLearn);
        a->values.push_back(parametersInGUIBounds[a->parameterID].x);
        a->values.push_back(parametersInGUIBounds[a->parameterID].y);
        a->smoothing = 0.7;
        a->channel = eventArgs.channel;
        a->midiLearnt = true;

        switch(eventArgs.status){
            case MIDI_NOTE_ON:{
                a->type = MIDION;
                a->minId = a->maxId = eventArgs.pitch;
                factorymidiNoteonAutos.push_back(a);
                automationsSanityCheck(&factorymidiNoteonAutos);
                for(int id=a->minId;id<=a->maxId;id++)
                    noteAutoFastMap[a->channel][id-1].push_back(factorymidiNoteonAutos[factorymidiNoteonAutos.size()-1]);
            } break;
            case MIDI_CONTROL_CHANGE:{
                a->type = MIDICC;
                a->minId = a->maxId = eventArgs.control;
                factorymidiCCAutos.push_back(a);
                automationsSanityCheck(&factorymidiCCAutos);
                ccAutoFastMap[a->channel][a->minId-1].push_back(factorymidiCCAutos[factorymidiCCAutos.size()-1]);
            } break;
            default:;
        }
        
        
        stringstream str;
        for(Auto* fa : factorymidiNoteonAutos)
            if(fa->midiLearnt)
            str << fa->toString() << endl;
        for(Auto* fa : factorymidiCCAutos)
            if(fa->midiLearnt)
                str << fa->toString() << endl;
        
        midiLearnFile.clear();
        
        midiLearnFile << str.str() << endl;
        
        midiLearnFile.close();
        
        midiLearn = -1;
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
        
        /// MACROS/SAVE
        
        if((type==MIDI_NOTEON && id==120)
           || (type==MIDI_CC && id==120)){ // C8
            loadNextMacro();
            return;
        }
        
        if((type==MIDI_NOTEON && id==119)
           || (type==MIDI_CC && id==119)){ // B7
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
    
    
    /// MACROS LOAD
    
    float value = eventArgs.value;
    switch(eventArgs.status){
        case MIDI_CONTROL_CHANGE:{
//            logfile << ofGetElapsedTimeMillis() << " MIDI CC " << eventArgs.control << " " << value << endl;
            switch(eventArgs.channel){
                case 1 :{
#ifndef MACRO_APC
                    if(!eventArgs.portName.compare(MACRO_INTERFACE)){
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
#endif
                }break;
            }
        }break;
        case MIDI_NOTE_ON:{
            //                if(eventArgs.pitch!=0)
            //                    logfile << ofGetElapsedTimeMillis() <<" MIDI NOTEON " << eventArgs.pitch << " " << value << endl;
            int pitch = eventArgs.pitch;
            switch(eventArgs.channel){
                case 1 :{
                    if(!eventArgs.portName.compare(MACRO_INTERFACE)){
                        int macroCode;
#ifdef MACRO_APC
                        if(pitch == 65){ //yes this is spaghetto
                            setMacroPage(macroPage - 1);
                            return;
                        }else if(pitch == 64){
                            setMacroPage(macroPage + 1);
                            return;
                        }else if(pitch == 67){
                            loadNextMacro();
                            return;
                        }

                        if(pitch>=0 && pitch <= NB_MACRO_PER_PAGE){
                            macroCode = getMacroPitch(eventArgs.pitch);
#else
                            macroCode = getMacroPitch(eventArgs.pitch);
                            if((pitch>=0 && pitch<=8) || (pitch>=16 && pitch<=24)
                               || (pitch>=32 && pitch<=40) || (pitch>=48 && pitch<=56)
                               || (pitch>=64 && pitch<=72) || (pitch>=80 && pitch<=88)
                               || (pitch>=96 && pitch<=104) || (pitch>=112 && pitch<=120)){
#endif
                                string path = getMacroFromMIDI(macroCode);
                                if(path.empty())
                                    break;
                                setMacroCode(pitch);
                                if(eventArgs.velocity>0){
                                    if(saveMacros){
                                        saveMacro(path);
                                        saveMacros = false;
                                    }else{
                                        loadMacro(path);
                                    }
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
//            logfile << ofGetElapsedTimeMillis() << " MIDI NOTEOFF " << eventArgs.pitch << " " << value << endl;
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
        for(map<string, bool>::iterator i=MIDI_device_connected.begin();i!=MIDI_device_connected.end();i++)
            i->second = false;
        for(int i=0;i<NB_MIDI_PORTS;i++){
            try{
                ofxMidiIn* in = &(midiIns[i]);
                string name = in->getName();
                if(in->isOpen() && name.compare("") && MIDI_device_connected.count(in->getName()))
                    MIDI_device_connected[in->getName()] = true;
                }catch(exception e){}
        }
        ///reopen ports if disconnected
        for(map<string, bool>::iterator i=MIDI_device_connected.begin();i!=MIDI_device_connected.end();i++){
            if(!i->second){
                openPorts(false);
                break;
            }
        }
        /// check macro connection
//        if(!string(MACRO_INTERFACE).empty() && !launchpadOut.isOpen()){
//            launchpadOut.openPort(MACRO_INTERFACE);
//            if(launchpadOut.isOpen())
//                paintMacros();
//        }
        makeSureMIDIOutDeviceIsOpen();
    }
    programChangeFlag = 0;
}
