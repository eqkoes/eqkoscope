#include "eqkoscope.h"
bool isCmdModifier = false;

ofxMidiMessage createMessage(int pitch){
    ofxMidiMessage eventArgs;
    eventArgs.channel=3;
    eventArgs.status = MIDI_NOTE_ON;
    eventArgs.pitch = pitch;
    return eventArgs;
}

void eqkoscope::updatePromptValue(){
    if(parameterNameMap.count(promptStr)>0){
        lastPromptValue = ofToFloat(promptValue);;
        lastPromptStr = promptStr;
        int pid = parameterNameMap[promptStr];
        parameterMap[pid] = deltaMap[pid] = ofToFloat(promptValue);
        return;
    }
    
    if(parameterNameMap.count(quickPromptStr)>0){
        lastPromptValue = ofToFloat(promptValue);;
        lastPromptStr = quickPromptStr;
        int pid = parameterNameMap[quickPromptStr];
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
