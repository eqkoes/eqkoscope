#include "eqkoscope.h"


ofxMidiMessage createMessage(int pitch){
    ofxMidiMessage eventArgs;
    eventArgs.channel=3;
    eventArgs.status = MIDI_NOTE_ON;
    eventArgs.pitch = pitch;
    return eventArgs;
}

void eqkoscope::updatePromptValue(){
#ifndef OF_10
    ofStringReplace(promptValue,"\x01", "");
    ofStringReplace(promptStr,"\x01", "");
#endif
    
    int pid = getParameterFromName(promptStr);
    if(pid>=0){
        lastPromptStr = promptStr;
    }else{
        pid = getParameterFromName(quickPromptStr);
        if(pid>=0){
            lastPromptStr = quickPromptStr;
        }else{
            return;
        }
    }

    lastPromptValue = ofToFloat(promptValue);;

    if( pid==ledTint && !promptValue.compare("hue"))
        lastPromptValue = deltaMap[tintHue];
        
        parameterMap[pid] = deltaMap[pid] = lastPromptValue;
        return;
}

void eqkoscope::keyReleased(int key){
    keyIsDown[key] = false;
}

void eqkoscope::keyPressed(int key){

    keyIsDown[key] = true;
    
    if(key==210){
        saveMacroTC = false;
        saveCurrentMacro();
        saveMacroTC = true;
        return;
    }
    
    
    if(keyIsDown[OF_KEY_CONTROL] ){
        if(key=='s'){
            saveMacroTC = !keyIsDown[OF_KEY_ALT];
            saveCurrentMacro();
            saveMacroTC = true;
            return;
        }
        else if(key=='p'){
            stringstream str;
            str << "open " << + "/Users/Raph/Dev/of_v0.8.4_osx_release/apps/myApps/Feedback/bin/data/" << uzi->getCurrentMedia().c_str();
            cout << "CMD " << str.str() << endl;
            ofSystem(str.str());
            return;
        }
    }
    
    if(isPrompt) {
        
        if(key==OF_KEY_SHIFT || key==OF_KEY_COMMAND || key==3680) //shift
            return;
#ifndef OF_10
        if(key==2305 || key==2306) //shift
            return;
#endif
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
        
        if(key==OF_KEY_RETURN){
            
            if(!promptStr.compare("")){
                isPrompt = false;
                return;
            }
            ///ONE LINERS
            if(!promptStr.compare("save")){
                saveCurrentMacro();
                isPrompt = false;
                return;
            }
            
            if(!promptStr.compare("guru")){
                if(guruQuotes.size()>0){
                loadScene(print, 0);
                    string str = guruQuotes[ofRandom(guruQuotes.size())];
                    ofStringReplace(str, "//", "\n");
                print->setText(str);
                    deltaMap[fontSize] = 22;
                    deltaMap[randomFont] = 0;
                    print->updateFont();
                }
                isPrompt = false;
                return;
            }
            
            if(!promptStr.compare("oblique")){
                if(obliqueQuotes.size()>0){
                    loadScene(print, 0);
                    string str = obliqueQuotes[ofRandom(obliqueQuotes.size())];
                    ofStringReplace(str, "//", "\n");
                    print->setText(str);
                    deltaMap[fontSize] = 22;
                    deltaMap[randomFont] = 0;
                    print->updateFont();
                }
                isPrompt = false;
                return;
            }
            
            if(!promptStr.compare("open")){
                ofFile file(currentMacroStr, ofFile::ReadOnly);
                if(file.exists()){
                    stringstream str;
                    str << "open " << file.path() ;
                    ofSystem(str.str());
                }
                isPrompt = false;
                return;
            }

            if(!promptStr.compare("map")){
                vector<string> splittedPaths = ofSplitString(MIDIMapPath, ":");
                stringstream str;
                for(string path : splittedPaths){
                    str.clear();
#ifdef OF_10
        string tpath = ofBufferFromFile("../"+path).getText();
#else
        string tpath = ofBufferFromFile(ofFilePath::getCurrentWorkingDirectory() +  "/../../../" + path).getText();

#endif
                    ofFile file(tpath, ofFile::ReadOnly);
                    if(file.exists()){
                        str << "open " << file.path() ;
                        ofSystem(str.str());
                    }
                }
                return;
            }
            
            if(!promptStr.compare("MIDI") && isPromptValue){
                ///MIDI LEARN
                int pid = getParameterFromName(promptValue);
                if(pid >= 0 ){
                    midiLearn = pid;
                }
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
                
                int pid = getParameterFromName(promptStr);
                if(pid >=0 ){
                    lastPromptValue = ofToFloat(promptValue);;
                    lastPromptStr = promptStr;
                    updatePromptValue();
                    return;
                }else{ //special commands
                     if(!promptStr.compare("featuredParameter") || !promptStr.compare("fp"))
                        featuredParameter = getParameterFromName(promptValue);
                    if(!promptStr.compare("txt"))
                        print->setText(promptValue);
                    else if(!promptStr.compare("CMD"))
                        addCommand(promptValue, false);
                    else if(!promptStr.compare("BPM")){
                        if(ofStringTimesInString(promptValue, ",")==0){
                            addCommand("BPM,"+promptValue+",0/1,4", false);
                        }else{
                        addCommand("BPM,"+promptValue+",4", false);
                        }
                    }else if(!promptStr.compare("moviePos"))
                        cinema->setPosition( ofToFloat(promptValue));
                    else if(!promptStr.compare("HD"))
                        setResolution(ofToInt(promptValue));
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
                if(key==OF_KEY_TAB      ){
                    if(quickPromptStr.compare("")){
                        quickPrmptIndex++;
                    }else{
                        quickPrmptIndex = 0;
                    }
                    lookup = true;
                }
                if(lookup){
                    int index = 0;
                    quickPromptStr = "";
#ifdef OF_10
                    for(int mi=0;mi<parameterIDMap.size();mi++){
                        string s1 = parameterIDMap[mi];
                        int pid = getParameterFromName(s1);
                        if(strStartsWith(s1, promptStr) && pid >=0 ){
                            if(index<quickPrmptIndex){
                                index++;
                                continue;
                            }
                            quickPromptStr = s1;
                            break;
                        }
                    }
#else
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
                    for(map<string, int>::iterator it=parameterNameAliasMap.begin();it!=parameterNameAliasMap.end();it++){
                        if(!strncmp(it->first.c_str(), promptStr.c_str(), promptStr.length())){
                            if(index<quickPrmptIndex){
                                index++;
                                continue;
                            }
                            quickPromptStr = it->first;
                            break;
                        }
                    }
#endif
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
        if(key==OF_KEY_SHIFT || key==OF_KEY_COMMAND || key>1000) //shift
            return;

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
            
        case OF_KEY_TAB:{
            guiVisible = !guiVisible;
//            guiCanvas->setVisible(guiVisible);
            if(guiVisible){
                updateGUI();
            }
        }break;
            
        case '@': reset();
            break;
        case '&': displayFrameRate = !displayFrameRate;
            break;

        case 'g':
            if(savingGif){
                //                gifEncoder.save("../capture/test.gif");
                savingGif = false;
                #ifdef VIDEO_EXPORT
                if(!hasTakenVideo){
                    vidRecorder.close();
                    vidRecorderIndex++;
                }
                hasTakenVideo = true;
#endif
            }else{
                startSequenceSave();
            }
            break;
            
        case '$':saveFrame = true;break;
        case 'm':
            resetMidi();
        break;
        case 'f':fullscreen = !fullscreen;ofSetFullscreen(fullscreen);break;
            
#ifndef EXPORT
            
#ifdef GENETIC
        case 'e':
            genetic->eval(ofGetMouseX()/float(WIDTH));
            break;
#endif
        case ',':{
            deltaMap[randomJump] = parameterMap[randomJump] = 1;
        }break;
        case '_':updateSkew(skewVector);break;
        case 't':loadScene(scenes[parameterMap[currentScene]]!=agents?agents:0, parameterMap[currentScene]);break;
        case 'j':loadScene(scenes[parameterMap[currentScene]]!=print?print:0, parameterMap[currentScene]);break;
        case 'i': loadScene(scenes[parameterMap[currentScene]]!=mapped?mapped:0, parameterMap[currentScene]);break;
#endif
    case ',':{
        deltaMap[randomJump] = parameterMap[randomJump] = 1;
    }break;
    case 'o': loadScene(scenes[parameterMap[currentScene]]!=feedbackScene?feedbackScene:0, parameterMap[currentScene]);break;
    case '<':loadNextMacro();break;
    case '>':loadPrevMacro();break;
    case '(' : randomParameters();break;
    case 'u': loadScene(scenes[parameterMap[currentScene]]!=uzi?uzi:0, parameterMap[currentScene]);break;
    case 'l': loadScene(scenes[parameterMap[currentScene]]!=lines?lines:0, parameterMap[currentScene]);break;
    case 'j':loadScene(scenes[parameterMap[currentScene]]!=print?print:0, parameterMap[currentScene]);break;
    case 'y':loadSoloScene(drawscene); break;
    case 'p':loadScene(scenes[parameterMap[currentScene]]!=cinema?cinema:0, parameterMap[currentScene]);break;
case '0': localFrameNum = 0;break;

//            case '+':
//            deltaMap[macroFade] = 5;
//            break;
        case '=': {pause =  !pause;
            for(int i=0;i<scenes.size();i++){
                if(scenes[i]==cinema)
                    cinema->pause(pause);
                for(int c=0;c<cinemas.size();c++){
                    if(scenes[i]==cinemas[c])
                        cinemas[c]->pause(pause);
                }
            }
            if(pause)
                if(strip!=0)
                    strip->closeSerial();
        }break;
        case 'k':
            deltaMap[kalei] = deltaMap[kalei]==0 ? ofRandom(1) : 0;
            deltaMap[kaleiNb] = (int) ofRandom(-1,5);
            break;

        case 'q':xOffsetDelta-=5; break;
        case 'd':xOffsetDelta+=5; break;
        case 'z':yOffsetDelta-=5; break;
        case 's':yOffsetDelta+=5; break;
        case 'w':trapeze-=0.05;break;
        case 'x':trapeze+=0.05;break;
        case 'R' :         niceRandom(0);break;
        case 'T' :         niceRandom(1);break;
        case 'Y' :         niceRandom(2);break;
        case 'r':// if(!liveMode)
        {saveMacros = !saveMacros;
            if(saveMacros)
                saveMacroStr = macroPath;
        }break;
        case 'c':
            MIDI_check_listen = !MIDI_check_listen;
            break;
        case '!' :{
            if(dualDisplay){
                if(dualDisplay && app->addMirorDisplay)
                    ofSetWindowPosition(-FINALWIDTH*2, 0);
else
                ofSetWindowPosition(-FINALWIDTH, 0);
            }
        } break;
        case 'a' :{
            surpriseParameters.clear();
            for(int i=0;i<7;i++){
                int r = -1;
                do{
                    r = ofRandom(338);
                }while(std::find(stressTestFilterList.begin(), stressTestFilterList.end(), r) != stressTestFilterList.end());
                surpriseParameters.push_back(r);
            }
            featuredParameter = surprise;
        }break;
      
        case 167: loadShaders();break;
    }
    
    if(scenes[parameterMap[currentScene]]!=0)
        scenes[parameterMap[currentScene]]->keyPressed(key);
}
