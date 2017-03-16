#include "eqkoscope.h"
#include <string>

int NB_MIDI_PORTS = 8;

int lastMidiAftertouch= 0;

int clockIndex = 0;
vector<int> MIDIClockDates;

bool isCmdModifier = false;

ofxMidiMessage createMessage(int pitch){
    ofxMidiMessage eventArgs;
    eventArgs.channel=3;
    eventArgs.status = MIDI_NOTE_ON;
    eventArgs.pitch = pitch;
    return eventArgs;
}

void eqkoscope::sendControlChange(int channel, int control, int value){
    if(midiControlValues[channel-1][control-1]!=value){
        midiControlValues[channel-1][control-1] = value;
        midiOut.sendControlChange(channel, control, value);
    }
}

void eqkoscope::initMidi(){
    loadMapping(ofBufferFromFile(ofFilePath::getCurrentWorkingDirectory()+"/../../../MIDIMap.csv").getText(), true, true);
    
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
               && in.getName().compare("APC Key 25")){
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
 if(eventArgs.channel == 16)
        return;
    
    
    /********* MIDI THROUGH **********/
    //    if(eventArgs.channel==15 && eventArgs.status==MIDI_NOTE_ON){
    //        midiOut.sendNoteOn(eventArgs.channel, eventArgs.pitch);
    //        return;
    //    }
    
    /********* MIDI CLOCK **********/
    if(eventArgs.status==MIDI_TIME_CLOCK){
    if(parameterMap[bpmLock]==1){
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
            parameterMap[bpm] = round(60*1000000/(d*24.0));
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
    
    if(eventArgs.portName.compare("Launchpad S")){
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
            if(noteAutoFastMap[channel-1][id-1].size()>0){
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
            if(noteAutoFastMap[channel-1][id-1].size()>0){
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
                    midiCCAutos[i]->update(value/127.0);
                    override = true;
                }
            }
            if(ccAutoFastMap[channel-1][id-1].size()>0){
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
}

void eqkoscope::parseMidi(ofxMidiMessage eventArgs){
#ifdef MPD24
    if(eventArgs.status==MIDI_CONTROL_CHANGE && eventArgs.channel==3 &&
       eventArgs.control==7)
        eventArgs.control = 9;
#endif
    
    if(ofGetFrameNum()<3)
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
    
    
    if(eventArgs.channel == 3 || eventArgs.channel == 4 || (eventArgs.channel == 2 && eventArgs.control==91)){ //regular interfaces
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
    //        switch(code){
    //            case 84: parameterMap[skewAmp] = intensity*currentParamIntensity;parameterMap[skewAmpMidi] = on*127;break;
    //            case 85: parameterMap[skewDAmp] = 0.15*intensity*currentParamIntensity;parameterMap[skewVAmpMidi] = on*127;break;
    //            case 86: parameterMap[displaceAmp] = intensity*currentParamIntensity;break;
    //            case 87: parameterMap[displaceVAmp] = intensity*currentParamIntensity;break;
    //            case 88: parameterMap[toLine] = intensity*currentParamIntensity;break;
    //            case 89: parameterMap[toCircle] = intensity*currentParamIntensity;break;
    //            case 90: parameterMap[chromaSep] = deltaMap[chromaSep] = intensity*0.3*currentParamIntensity;break;
    //            case 91: deltaMap[chromaSep] = intensity?0.6*currentParamIntensity:0;break;
    //            case 92: ;break;
    //            case 93: ;break;
    //            case 94: parameterMap[xpixellate] = deltaMap[xpixellate] = intensity*100*currentParamIntensity;break;
    //            case 95: parameterMap[ypixellate] = deltaMap[ypixellate] = intensity*100*currentParamIntensity;break;
    //            case 96: deltaMap[sortXThresh] = parameterMap[sortXThresh] = intensity*100*currentParamIntensity;break;
    //            case 97: parameterMap[sharpen] = intensity*currentParamIntensity;break;
    //        }
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
                    if(in.isOpen() && !name.compare("Launchpad S"))
                        is_launchpad = true;
                if(enforce_nano)
                    if(in.isOpen() && !name.compare("nanoKONTROL2 SLIDER/KNOB"))
                        is_nano = true;
            }catch(exception e){}
        }
        if((enforce_io2 && !is_io2) || (enforce_novation && !is_novation) || (enforce_launchpad && !is_launchpad) || (enforce_nano && !is_nano) || (enforce_external_device && !is_external_device)){
            openPorts();
        }
    }
    programChangeFlag = 0;
}


/******************** KEYS **/

void eqkoscope::updatePromptValue(){
    if(parameterNameMap.count(promptStr)>0){
        lastPromptValue = ofToFloat(promptValue);;
        lastPromptStr = promptStr;
        int pid = parameterNameMap[promptStr];
        parameterMap[pid] = deltaMap[pid] = ofToFloat(promptValue);
        return;
    }
}

void eqkoscope::keyReleased(int key){
    switch(key){
        case 4352: isCmdModifier = false;
            break;
        default:;
    }
}

void eqkoscope::keyPressed(int key){
    logfile << ofGetElapsedTimeMillis() << " KeyPressed " << key << endl;
    
    if(isCmdModifier){
        if(key=='s')
            saveCurrentMacro();
        return;
    }
    
    if(isPrompt) {
        if(key==2304 || key==2305 || key==2306) //shift
            return;
        if(key==OF_KEY_BACKSPACE){
            if(promptStr.size()==0){
                isPrompt = false;
                return;
            }
            if(!isPromptValue){
                if(quickPromptStr.compare(""))
                    promptStr = quickPromptStr;
                quickPromptStr = "";
                promptStr = promptStr.substr(0, promptStr.size()-1);
            }else{
                promptValue = promptValue.substr(0, promptValue.size()-1);
                updatePromptValue();
            }
            return;
        }
        
        if(key==OF_KEY_RETURN || key==' '){
            if(!promptStr.compare("")){
                isPrompt = false;
                return;
            }
            if(!isPromptValue){
                isPromptValue = true;
                promptValue = "";
            }else{
                isPrompt = false;
                if(quickPromptStr.compare(""))
                    promptStr = quickPromptStr;
                if(parameterNameMap.count(promptStr)>0){
                    lastPromptValue = ofToFloat(promptValue);;
                    lastPromptStr = promptStr;
                    updatePromptValue();
                    return;
                }else{ //special commands
                    if(!promptStr.compare("currentText"))
                        print->currentText = promptValue;
                    if(!promptStr.compare("featuredParameter") || !promptStr.compare("fp"))
                        featuredParameter = parameterNameMap[promptValue];
                    if(!promptStr.compare("lx"))
                        leapXParameter = parameterNameMap[promptValue];
                    if(!promptStr.compare("ly"))
                        leapYParameter = parameterNameMap[promptValue];
                    if(!promptStr.compare("lz"))
                        leapZParameter = parameterNameMap[promptValue];
                    if(!promptStr.compare("ldx"))
                        leapDXParameter = parameterNameMap[promptValue];
                    if(!promptStr.compare("ldy"))
                        leapDYParameter = parameterNameMap[promptValue];
                    if(!promptStr.compare("ldz"))
                        leapDZParameter = parameterNameMap[promptValue];
                    if(!promptStr.compare("lroll"))
                        leapRollParameter = parameterNameMap[promptValue];
                    if(!promptStr.compare("assignChannel"))
                        assignChannel = ofToInt(promptValue);
                    if(!promptStr.compare("assignCtrl"))
                        assignCtrl = ofToInt(promptValue);
                    if(!promptStr.compare("CMD"))
                        addCommand(promptValue, false);
                    if(!promptStr.compare("moviePos"))
                        cinema->setPosition( ofToFloat(promptValue));
                }
            }
        }else{
            if(!isPromptValue){
                bool lookup = false;
                if(key== OF_KEY_DOWN){
                    if(quickPromptStr.compare(""))
                        quickPrmptIndex++;
                    else
                        quickPrmptIndex = 0;
                    lookup = true;
                }
                if(key== OF_KEY_UP){
                    if(quickPromptStr.compare(""))
                        quickPrmptIndex--;
                    else
                        quickPrmptIndex = 0;
                    lookup = true;
                }
                if(key==9){
                    if(quickPromptStr.compare(""))
                        quickPrmptIndex++;
                    else
                        quickPrmptIndex = 0;
                    lookup = true;
                }
                if(lookup){
                    int index = 0;
                    quickPromptStr = "";
                    for(map<string, int>::iterator it=parameterNameMap.begin();it!=parameterNameMap.end();it++){
                        if(!strncmp(it->first.c_str(), promptStr.c_str(), promptStr.length())){
                            if(index<quickPrmptIndex){
                                index++;
                                continue;
                            }
                            quickPromptStr = it->first;
                            break;
                        }
                    }
                }else
                    promptStr += ((char) key);
            }else{
                promptValue += ((char) key);
                updatePromptValue();
            }
        }
        return;
    }
    
    if(saveMacros){
        //        cout << "new key " << key << " " << (int) key << endl;
        if(key==2304 || key==2305 || key==2305) //ignore shift key
            return;
        if(key=='R'){
            saveMacros = false;
            return;
        }
        //        if(key=='L'){
        //            saveMacroStr.append(".xml");
        //            loadMacro(saveMacroStr);
        //            saveMacroStr = macroPath;
        //            saveMacros = false;
        //            return;
        //        }
        if(key==OF_KEY_RETURN){
            saveMacroStr.append(".xml");
            saveMacro(saveMacroStr);
            saveMacroStr = macroPath;
            saveMacros = false;
            return;
        }
        if(key=='O'){ //overwrite current macro
            saveMacroStr.append(".xml");
            cout << currentMacroStr << endl;
            saveMacro(currentMacroStr);
            saveMacroStr = macroPath;
            saveMacros = false;
            return;
        }
        if(key==OF_KEY_BACKSPACE){
            if(saveMacroStr.size()==0){
                saveMacros = false;
                return;
            }
            saveMacroStr = saveMacroStr.substr(0, saveMacroStr.size()-1);
            return;
        }
        char b[2];
        b[0] = key;
        b[1] = '\0';
        saveMacroStr.append(b);
        return;
    }else{ //prompt
        if(key==OF_KEY_RETURN){
            if(!isPrompt){
                isPrompt = true;
                isPromptValue = false;
                promptStr = "";
                quickPromptStr = "";
                quickPrmptIndex = 0;
                promptValue = "";
                return;
            }
        }
    }
    
    switch(key){
        case '@': reset();
            break;
        case '&': displayFrameRate = !displayFrameRate;
            break;
        case '<':loadNextMacro();break;
        case '>':loadPrevMacro();break;
        case 'g':
            if(savingGif){
//                gifEncoder.save("../capture/test.gif");
                savingGif = false;
            }else{
                savingGif = true;
                gifIndex = int(ofRandom(1,100000));
            }
            break;
        case ':':
            parameterMap[antiAliasing] = !parameterMap[antiAliasing];
            break;
        case '-':loadSoloScene(glitch); break;
        case '$':saveFrame = true;break;
        case '_':updateSkew(skewVector);break;
        case 'm':resetMidi(); break;
        case 'f':fullscreen = !fullscreen;ofSetFullscreen(fullscreen);break;
        case 'o': loadScene(scenes[parameterMap[currentScene]]!=feedbackScene?feedbackScene:0, parameterMap[currentScene]);break;
        case 'i': loadScene(scenes[parameterMap[currentScene]]!=fractal?fractal:0, parameterMap[currentScene]);break;
        case 't':loadScene(scenes[parameterMap[currentScene]]!=agents?agents:0, parameterMap[currentScene]);break;
        case 'p':loadScene(scenes[parameterMap[currentScene]]!=cinema?cinema:0, parameterMap[currentScene]);break;
        case 'u': loadScene(scenes[parameterMap[currentScene]]!=uzi?uzi:0, parameterMap[currentScene]);break;
        case 'U': loadScene(scenes[parameterMap[currentScene]]!=uzis[0]?uzis[0]:0, parameterMap[currentScene]);break;
        case 'l': loadScene(scenes[parameterMap[currentScene]]!=lines?lines:0, parameterMap[currentScene]);break;
        case 'j':loadScene(scenes[parameterMap[currentScene]]!=print?print:0, parameterMap[currentScene]);break;
        case -1: loadScene(scenes[parameterMap[currentScene]]!=mapped?mapped:0, parameterMap[currentScene]);break;
        case '=': {pause =  !pause;
            for(int i=0;i<scenes.size();i++){
                if(scenes[i]==cinema)
                    cinema->pause(pause);
                for(int c=0;c<cinemas.size();c++){
                    if(scenes[i]==cinemas[c])
                        cinemas[c]->pause(pause);
                }
            }
        }break;
            //        case 'i':parameterMap[invert] = !parameterMap[invert]; break;
        case 'a': if(!safeMode) {parameterMap[audio] = !parameterMap[audio];
            nbFramesSinceAudioStart = 0;
        }break;
        case 'k':
            deltaMap[kalei] = deltaMap[kalei]==0 ? (int) ofRandom(7) : 0;
            deltaMap[kaleiNb] = (int) ofRandom(-1,5);
            break;
            //        case 'e': parameterMap[skewAmp] = parameterMap[skewAmp]>0 ? 0 : ofRandom(0.3); break;
        case 'q':xOffsetDelta-=5; break;
        case 'd':xOffsetDelta+=5; break;
        case 'z':yOffsetDelta-=5; break;
        case 's':yOffsetDelta+=5; break;
        case 'w':trapeze-=0.05;break;
        case 'x':trapeze+=0.05;break;
        case 'R' :         randomParameters();break;
        case 'r': if(!safeMode){saveMacros = !saveMacros;
            if(saveMacros)
                saveMacroStr = macroPath;
        }break;
        case 'y':loadSoloScene(drawscene); break;
        case 356:  posX += 20; break;
        case 358: posX -= 20;break;
        case 357: posY += 20;  break;
        case 359:  posY -= 20; break;
            
            //MPD
        case 'W':{
            ofxMidiMessage eventArgs = createMessage(36);
            newMidiMessage(eventArgs);
        }break;
        case 'X':{
            ofxMidiMessage eventArgs = createMessage(37);
            newMidiMessage(eventArgs);
        } break;
        case 'C':{
            ofxMidiMessage eventArgs = createMessage(38);
            newMidiMessage(eventArgs);
        }break;
        case 'V':{
            ofxMidiMessage eventArgs = createMessage(39);
            newMidiMessage(eventArgs);
        } break;
        case 'Q':{
            ofxMidiMessage eventArgs = createMessage(40);
            newMidiMessage(eventArgs);
        } break;
        case 'S':{
            ofxMidiMessage eventArgs = createMessage(41);
            newMidiMessage(eventArgs);
        } break;
        case 'D':{
            ofxMidiMessage eventArgs = createMessage(42);
            newMidiMessage(eventArgs);
        } break;
        case 'F':{
            ofxMidiMessage eventArgs = createMessage(43);
            newMidiMessage(eventArgs);
        }break;
        case 'A':{
            ofxMidiMessage eventArgs = createMessage(44);
            newMidiMessage(eventArgs);
        }break;
        case 'Z':{
            ofxMidiMessage eventArgs = createMessage(45);
            newMidiMessage(eventArgs);
        }break;
        case 'E':{
            ofxMidiMessage eventArgs = createMessage(46);
            newMidiMessage(eventArgs);
        }break;
            //        case 'R':{
            //            ofxMidiMessage eventArgs = createMessage(47);
            //            newMidiMessage(eventArgs);
            //        }break;
        case '1':{
            ofxMidiMessage eventArgs = createMessage(48);
            newMidiMessage(eventArgs);
        }break;
            //            case 233:{
        case 2:{
            ofxMidiMessage eventArgs = createMessage(49);
            newMidiMessage(eventArgs);
        }break;
        case 3:{
            ofxMidiMessage eventArgs = createMessage(50);
            newMidiMessage(eventArgs);
        }break;
        case 4:{
            ofxMidiMessage eventArgs = createMessage(51);
            newMidiMessage(eventArgs);
        }break;
        case 4352: isCmdModifier = true;
            break;
    }
    if(scenes[parameterMap[currentScene]]!=0)
        scenes[parameterMap[currentScene]]->keyPressed(key);
}
