#include "eqkoscope.h"



float skew_lastDate = 0;

void eqkoscope::loadConfig(){
    string t = ofBufferFromFile(ofFilePath::getCurrentWorkingDirectory()+"/../../../config.csv").getText();
    if(t.compare("")){
        vector<string> lines = ofSplitString(t, "\n");
        for(int x=0;x<lines.size();x++){
            vector<string > fields = ofSplitString(lines[x], ",");
            if(fields.size()<2)
                continue;
            string param = fields[0];
            string value = fields[1];
            if(!param.compare("live_mode"))
                liveMode = ofToInt(value);
            else
                if(!param.compare("dualDisplay"))
                    dualDisplay = ofToInt(value);
                else
                    if(!param.compare("width") && value.compare("native")){
                        FINALWIDTH = ofToInt(value);
                    }else if(!param.compare("height") && value.compare("native")){
                        FINALHEIGHT = ofToInt(value);
                    }else if(!param.compare("multiProjector")){
                        MULTIPROJECTOR = ofToInt(value);
                    }else if(!param.compare("gif_width") && value.compare("native")){
                        GIF_WIDTH = ofToInt(value);
                    }else if(!param.compare("gif_height") && value.compare("native")){
                        GIF_HEIGHT = ofToInt(value);
                    }else if(!param.compare("macro_path"))
                        macroPath = (value);
                    else if(!param.compare("fp"))
                        featuredParameter = (parameterNameMap[value]);
                    else if(!param.compare("MIDIMap"))
                        MIDIMapPath = (value);
                    else if(!param.compare("enforce_io2"))
                        enforce_io2 = !value.compare("1");
                    else if(!param.compare("enforce_launchpad"))
                        enforce_launchpad = !value.compare("1");
                    else if(!param.compare("enforce_novation"))
                        enforce_novation = !value.compare("1");
                    else if(!param.compare("enforce_nano"))
                        enforce_nano = !value.compare("1");
        }
    }
}

void eqkoscope::loadMapping(std::string str, bool factory, bool erase){
    
    if(erase){
        eraseControlMapping(factory);
    }
    
    autoComments = "";
    string t = str;
    if(t.compare("")){
        vector<string> lines = ofSplitString(t, "\n");
        for(int x=0;x<lines.size();x++){
            if(lines[x].compare("") && lines[x].compare(0, 2, "//")
               && lines[x].compare(0, 2, "<!")){
                addCommand(lines[x], factory);
            }else{
                if(!factory)
                    autoComments += lines[x] + "\n";
            }
        }
    }
    
}

void eqkoscope::loadShaders(){
    pixelHShader.load("../shaders/pixelsH");
    threedeeShader.load("../shaders/threeDee");
    lineShader.load("../shaders/line");
    mandalaShader.load("../shaders/mandala");
    squareMandalaShader.load("../shaders/squareMandala");
    sharpenShader.load("../shaders/sharpen");
    saturationShader.load("../shaders/saturation");
    testShader.load("../shaders/threeDee");
    blendShader.load("../shaders/blend");
    disposeShader.load("../shaders/dispose");
    hueShiftShader.load("../shaders/hueShift");
    hueCrazeShader.load("../shaders/hueCraze");
    maskShader.load("../shaders/dmask");
    hueFilterShader.load("../shaders/hueFilter");
//    psyShiftShader.load("../shaders/psyShift");
    chromaPointShader.load("../shaders/chromaPoint");
    bwShader.load("../shaders/b&w");
    
    loadFXShaders();
}


void eqkoscope::eraseControlMapping(bool factory){
    if(!factory){
        //        parameterEasingMap.clear();
        for(int i=0;i<audioAutos.size();i++)
            delete audioAutos[i];
        audioAutos.clear();
        for(int i=0;i<oscAutos.size();i++)
            delete oscAutos[i];
        oscAutos.clear();
        for(int i=0;i<midiNoteonAutos.size();i++)
            delete midiNoteonAutos[i];
        midiNoteonAutos.clear();
        for(int i=0;i<midiCCAutos.size();i++)
            delete midiCCAutos[i];
        midiCCAutos.clear();
        for(int i=0;i<leapAutos.size();i++)
            delete leapAutos[i];
        leapAutos.clear();
        for(int i=0;i<timedAutos.size();i++)
            delete timedAutos[i];
        timedAutos.clear();
    }else{
        for(int i=0;i<factorymidiNoteonAutos.size();i++){
            if(factorymidiNoteonAutos[i]==0){
                delete factorymidiNoteonAutos[i];
                factorymidiNoteonAutos[i]=0;
            }
        }
        factorymidiNoteonAutos.clear();
        for(int i=0;i<factorymidiCCAutos.size();i++){
            if(factorymidiCCAutos[i]==0){
                delete factorymidiCCAutos[i];
                factorymidiCCAutos[i] =0;
            }
        }
        factorymidiCCAutos.clear();
        for(int i=0;i<16;i++)
            for(int j=0;j<127;j++){
                ccAutoFastMap[i][j].clear();
                noteAutoFastMap[i][j].clear();
            }
    }
}

void eqkoscope::addCommand(string commandString, bool factory){
    vector<string> args = ofSplitString(commandString, " ");
    vector<string> fields = ofSplitString(args[0], ",");
    
    if(fields.size()<2 || args.size()<1)
        return;
    
    string triggerTypeStr = fields[0];
    int pid = parameterNameMap[fields[1]];
    Auto* a = new Auto(this, pid);
    
    
    if(args.size()>=2){
        a->args = args[1];
        
        if(args[1].find("random")!=string::npos)
            a->random = 1;
        if(args[1].find("loop")!=string::npos)
            a->loopMode = 2;
        if(args[1].find("hold")!=string::npos)
            a->hold = true;
        if(args[1].find("invert")!=string::npos)
            a->invert = true;
        if(args[1].find("bin")!=string::npos)
            a->bin = true;
        if(args[1].find("transcient")!=string::npos)
            a->transcient = true;
    }
    
    if(!triggerTypeStr.compare("AUDIO")){
        if(fields.size()==5){
            a->type = AUDIO;
            a->channel = ofToInt(fields[4]);
            vector<string> v = ofSplitString(fields[2], "/");
            a->smoothing = ofToFloat(fields[3]);
            if(v.size()>=1){
                for(int vIndex=0;vIndex<v.size();vIndex++)
                    a->addValue(ofToFloat(v[vIndex]));
                audioAutos.push_back(a);
            }
        }
        return;
    }
    if(!triggerTypeStr.compare("OSC")){
        if(fields.size()==5){
            a->type = OSC;
            a->oscPrefix = fields[4];
            vector<string> v = ofSplitString(fields[2], "/");
            a->smoothing = ofToFloat(fields[3]);
            parameterEasingMap[pid] = a->smoothing;
            if(v.size()>=1){
                for(int vIndex=0;vIndex<v.size();vIndex++)
                    a->addValue(ofToFloat(v[vIndex]));
                oscAutos.push_back(a);
            }
        }
        return;
    }
    if(!triggerTypeStr.compare("NOTEON")){
        if(fields.size()==6){
            a->type = MIDION;
            a->channel = ofToInt(fields[4]);
            if(fields[5].find("/")!=string::npos){
                vector<string> ids = ofSplitString(fields[5], "/");
                a->minId = ofToInt(ids[0]);
                a->maxId = ofToInt(ids[1]);
            }else
                a->minId = a->maxId = ofToInt(fields[5]);
            vector<string> v = ofSplitString(fields[2], "/");
            a->smoothing = ofToFloat(fields[3]);
            parameterEasingMap[pid] = a->smoothing;
            if(v.size()>=1){
                for(int vIndex=0;vIndex<v.size();vIndex++)
                    a->addValue(ofToFloat(v[vIndex]));
                if(factory){
                    factorymidiNoteonAutos.push_back(a);
                    for(int id=a->minId;id<=a->maxId;id++)
                        noteAutoFastMap[a->channel-1][id-1].push_back(factorymidiNoteonAutos[factorymidiNoteonAutos.size()-1]);
                }else
                    midiNoteonAutos.push_back(a);
            }
        }
        return;
    }
    if(!triggerTypeStr.compare("CC")){
        if(fields.size()==6){
            a->type = MIDICC;
            a->channel = ofToInt(fields[4]);
            a->minId = a->maxId = ofToInt(fields[5]);
            vector<string> v = ofSplitString(fields[2], "/");
            a->smoothing = ofToFloat(fields[3]);
            parameterEasingMap[pid] = a->smoothing;
            if(v.size()>=1){
                for(int vIndex=0;vIndex<v.size();vIndex++)
                    a->addValue(ofToFloat(v[vIndex]));
                if(factory){
                    factorymidiCCAutos.push_back(a);
                    ccAutoFastMap[a->channel-1][a->minId-1].push_back(factorymidiCCAutos[factorymidiCCAutos.size()-1]);
                }else
                    midiCCAutos.push_back(a);
            }
        }
        return;
    }
    if(!triggerTypeStr.compare("LEAP")){
        if(fields.size()>=5){
            a->type = LEAP;
            a->leapDimension = (fields[4]);
            a->smoothing = ofToFloat(fields[3]);
            vector<string> v = ofSplitString(fields[2], "/");
            if(v.size()>1){
                for(int vIndex=0;vIndex<v.size();vIndex++)
                    a->addValue(ofToFloat(v[vIndex]));
                if(fields.size()==6){
                    vector<string> v2 = ofSplitString(fields[5], "/");
                    if(v2.size()==2){
                        a->inputMinValue = ofToFloat(v2[0]);
                        a->inputMaxValue = ofToFloat(v2[1]);
                    }else{
                        a->inputMinValue = ofToFloat(v2[0]);
                        a->inputMaxValue = ofToFloat(v2[0]);
                    }
                }
                leapAutos.push_back(a);
            }
        }
        return;
    }
    if(!triggerTypeStr.compare("DUR")){
        if(fields.size()==4){
            a->type = TIMED;
            a->BPMDivision = 0;
            vector<string> v = ofSplitString(fields[2], "/");
            a->uDuration = ofToFloat(fields[3]);
            if(v.size()>=1){
                for(int vIndex=0;vIndex<v.size();vIndex++)
                    a->addValue(ofToFloat(v[vIndex]));
                timedAutos.push_back(a);
            }
        }
        return;
    }
    if(!triggerTypeStr.compare("BPM")){
        if(fields.size()==4){
            a->type = BPM;
            a->BPMDivision = ofToFloat(fields[3]);
            vector<string> v = ofSplitString(fields[2], "/");
            if(v.size()>=1){
                for(int vIndex=0;vIndex<v.size();vIndex++)
                    a->addValue(ofToFloat(v[vIndex]));
                timedAutos.push_back(a);
            }
        }
        return;
    }
}

void eqkoscope::update(){
    
#ifdef ATELIERS
//    if(ofGetFrameNum()>10)
//        app->deltaMap[tintBrightness] = ofMap(ofGetMouseX(), 0, WIDTH, 0, 1);
#endif
    
#ifndef MASTER //receive orders via TCP
    int size = sizeof(ofxMidiMessage);
    for(int i = 0; i<tcpServer.getLastID(); i++){ // getLastID is UID of all clients
        if( tcpServer.isClientConnected(i) ){
            int r;
            while((r = tcpServer.receiveRawMsg(i, (char*)&tcpMIDIMsg, size))){
                if(r==size){
                    cout << "received midi " << tcpMIDIMsg.control << endl;
                    //newMidiMessage(tcpMIDIMsg);
                }
            }
        }
    }
#endif
    
#ifndef SLAVE
    return;
#endif
    
#ifdef INSTALL_THOUGHTS
    thougts->update();
#endif
    
#ifdef OFFLINE_RENDER
    updateOfflineMIDI();
#endif
    
#ifdef LAZERS
    //    if(ofGetFrameNum()%50==49)
    //    laser.update();
    //    laser.addLaserCircle(ofPoint(800,00), 500, ofColor::green);
#endif
    
#ifdef BRASERO
    //    if(kinectDevice.isFrameNew()){
    //        kinectDevice.draw(0, 0);
    kinectDevice.update();
    
    if(ofGetFrameNum()==5){ //initialisation
        scenes[0] = feedbackScene;
        feedbackScene->nestedScene = cinema;
        deltaMap[noSource] = 1;
        deltaMap[_mask] = 1;
        deltaMap[mediaZ] = -0.2;
        deltaMap[omg3D2] = 0;
        deltaMap[kinect] = 1;
        deltaMap[sobel] = 1;
        deltaMap[tintHue] = 0.67;
        deltaMap[threshold] = 0.5;
        cinema->loadDirectMovie("/Users/Raph/Dev/of_v0.8.4_osx_release/apps/myApps/Feedback/bin/data/cinema/1AaAvronDB/coro2_1.mov");
        for(int i=0;i<kinectDevice.getWidth();i++)
            for(int j=0;j<kinectDevice.getHeight();j++)
                kinectCalibration[i][j] = kinectDevice.getDistanceAt(i, j);
    }
    
    float detectionThreshold = 100;
    
    //calibration
    //sur pied X
    //    float maxHeight = 400;
    //    float braseroDistance = 1120;
    //    float margin = 300;
    //    float braseroRadius = 480/2 * 0.5;
    
    //calibration plafond JA
    float maxHeight = 1500;
    float maxDepth = 3500;
    float braseroDistance = 2500;
    float margin = 300;
    float braseroRadius = 480/2 * 0.4;
    float offX = 0*480*0.1;
    float offY =  480 * 0.1;
    float freezeThreshold = 0.4;
    float freezeEasing = 0.035;
    
    //RMax to RC computation
    float c = 0, Cx = 0, Cy = 0;
    float presence = 0;
    float dMin = 10000;
    float dMax = -1;
    for(int i=0;i<kinectDevice.getWidth();i++)
        for(int j=0;j<kinectDevice.getHeight();j++){
            float d = kinectDevice.getDistanceAt(i, j);
            if(d < kinectCalibration[i][j] - detectionThreshold){
                if(d > maxHeight && d < maxDepth){
//                    if(d < braseroDistance - margin){
                        presence ++;
                        dMin = min(dMin, d);
                        dMax = max(dMax, d);
                    if(d < braseroDistance - margin){
                        float x = i - kinectDevice.getWidth()/2 - offX;
                        float y = j - kinectDevice.getHeight()/2 - offY;
                        if(x*x + y*y < braseroRadius*braseroRadius){
                            Cx += i;
                            Cy += j;
                            c++;
                        }
                    }
//                    }
                }
            }
        }
    
    //centroid computation
    Cx = 0.5  - Cx/(c*kinectDevice.getWidth());
    Cy = Cy/(c*kinectDevice.getHeight()) - 0.5;
    cout << "kinect centroid " << deltaMap[user1]  << " " << c/10000 << " : " << Cx <<" " << Cy << " - " << dMin <<" " << dMax <<  endl;
    
    /** presence **/
    deltaMap[user1] = ofClamp(presence > 500 ? presence / (10000) : 0, 0, 1);
   deltaMap[user1] =1; //full presence
    if(c>0){
        deltaMap[user2] = Cx;
        deltaMap[user3] = Cy;
    }
    deltaMap[tintBrightness] = parameterMap[user1];
    //    deltaMap[mediaX] = parameterMap[user2];
    //    deltaMap[mediaY] = parameterMap[user3];
    
    /** ice **/
    if(c>freezeThreshold){
        deltaMap[user4] = ofClamp(deltaMap[user4] + freezeEasing, 0, 1);
    }else
        deltaMap[user4] = ofClamp(deltaMap[user4] - freezeEasing, 0, 1);
    
    
    
    oscout("/Kinect/n", deltaMap[user1] - deltaMap[user4]);
    oscout("/Kinect/c", parameterMap[user4]);
    
    deltaMap[_reTint] = parameterMap[user4];
    deltaMap[feedbackRemanence] = 1-parameterMap[user4]*0.2;
    deltaMap[_reTint] = 0.8;
    deltaMap[tintHue] = 0.03 + 0.64*parameterMap[user4];
    //    deltaMap[reSaturate] = parameterMap[user4]*0.75;
    //    deltaMap[tintSaturation] = 0.25 + 0.75 * (1-parameterMap[user4]);
    deltaMap[sobel] = 1 * (1 - parameterMap[user4]);
    deltaMap[upRot] = 0.05 + 1 * (1 - parameterMap[user4]);
    deltaMap[movieSpeed] = 1 + 4 * (1 - parameterMap[user4])>0.5 ? 1 : 0;
#endif
    
    if(parameterMap[MIDIMappingAutoLoad]>BOOL_PARAM_EPSILON && ofGetFrameNum()%60==0){
        loadMapping(ofBufferFromFile(ofFilePath::getCurrentWorkingDirectory()+"/../../../"+MIDIMapPath).getText(), true, true);
    }
    
    if(!kinectRunning && parameterMap[kinect]){ //start the kinect server
        cout << ofSystem("open ../../../FeedbackUtils.app") << endl;
        kinectRunning = true;
    }
    
    if(pause)
        return;
    
    float date = ofGetElapsedTimeMicros();
    
    if(debugMapping){
        if(controlFile.compare(""))
            loadMapping(ofBufferFromFile("parameterMappings/"+controlFile).getText(), false, true);
    }
    
    updateMidi();
    updateMacros();
    updateSerial();
    
    
    doStressTest();
    
    /** MACROS UPDATES **/
    macroMutex.lock();
    for(int i=0;i<pendingMacros.size();i++)
        parseMacro(pendingMacros[i]);
    pendingMacros.clear();
    macroMutex.unlock();
    
    /** MIDI UPDATES **/
    midiMutex.lock();
    for(int i=0;i<midiMsgs.size();i++)
        parseMidi(midiMsgs[i]);
    midiMsgs.clear();
    midiMutex.unlock();
    
    /** SCENE UPDATES **/
    if(parameterMap[embedCinema])
        cinema->update();
    
    for(int i=0;i<scenes.size();i++){ //genericit√©
        if(scenes[i]==cinema){
            cinema->update();
            continue;
        }
        for(int j=0;j<cinemas.size();j++)
            if(scenes[i]==cinemas[j])
                cinemas[j]->update();
    }
    for(int i=0;i<scenes.size();i++)
        if(scenes[i]==drawscene){
            drawscene->update();
            break;
        }
    for(int i=0;i<scenes.size();i++)
        if(scenes[i]==print){
            print->update();
            break;
        }
    for(int i=0;i<scenes.size();i++)
        if(scenes[i]==lines){
            lines->update();
            break;
        }
    for(int i=0;i<scenes.size();i++)
        if(scenes[i]==three){
            three->update();
            break;
        }
    for(int i=0;i<scenes.size();i++)
        if(scenes[i]==feedbackScene){
            feedbackScene->update();
            break;
        }
    for(int i=0;i<scenes.size();i++)
        if(scenes[i]==agents){
            agents->update();
            break;
        }
    for(int i=0;i<scenes.size();i++)
        if(mapped!=0 && scenes[i]==mapped){
            mapped->update();
            break;
        }
    for(int i=0;i<scenes.size();i++){
        if(scenes[i]==uzi){
            uzi->update();
            continue;
        }
        for(int j=0;j<uzis.size();j++)
            if(scenes[i]==uzis[j])
                uzis[j]->update();
    }
    
    
    /** EASING **/
    
    deltaMap[pertEvo] += parameterMap[pertEvoAuto];
    
    if(macroMorphing>0 && macroMorphEvo<1){
        macroMorphEvo += 1/(ofGetFrameRate()*macroMorphing);
        for(int i=0;i<N_PARAM;i++)
            parameterMap[i] = macroMorphEvo*deltaMap[i] + (1-macroMorphEvo)*macroMorphParameters[i];
    }else{
        
        for(int i=0;i<N_PARAM;i++){
            //        if(i!=ledEvent){
            float v = parameterMap[i] + parameterEasingMap[i]*(deltaMap[i]-parameterMap[i]);
            if(abs(v)<0.000001)
                v = 0;
            parameterMap[i] = v;
            //        }
        }
    }
    
    xOffset += 0.25*(xOffsetDelta-xOffset);
    yOffset += 0.25*(yOffsetDelta-yOffset);
    
    /** AUDIO IN**/
    if(parameterMap[_audioIn] || audioAutos.size()>0)
        for(int i=0;i<audioAutos.size();i++){
            audioAutos[i]->update(max(0.0f, min(1.0f,currentRms[audioAutos[i]->channel] * extAutoDimmer)));
        }
    
    float autoDate = ofGetElapsedTimeMicros();
    for(int i=0;i<timedAutos.size();i++){
        timedAutos[i]->update(0);
    }
    autoMicros += microsEasing*(ofGetElapsedTimeMicros() - autoDate - autoMicros);
    
    
    if(cinemaBackground || cinemaTexture)
        cinema->update();
    
    if(parameterMap[randomTint]>0){
        feedbackScene->updateRandomColor();
        utils->setTintHue((parameterMap[tintHue] + ofRandom(-parameterMap[tintAmp], parameterMap[tintAmp])/2.0));
    }
    
    /** OSC **/
    manageOSC();
    
    
    /** LEAP **/
    updateLeap();
    
    
    updateMicros += microsEasing*(ofGetElapsedTimeMicros() - date - updateMicros);
}

void eqkoscope::doStressTest(){
    if(parameterMap[stressTest]){
        int pIndex = int(ofRandom(N_PARAM));
        if(std::find(stressTestFilterList.begin(), stressTestFilterList.end(),pIndex)==stressTestFilterList.end()){
            if(ofRandom(1)<parameterMap[stressTestRate]){
                deltaMap[pIndex] = ofRandom(1)<0.5 ? 0 : ofRandom(1);
            }
        }
        
        //        if(ofRandom(1)<parameterMap[stressTestRate])
        //            for(int i=0;i<1;i++){
        //                ofxMidiMessage midi;
        //                midi.channel = 2 + ofRandom(2);
        //
        //                midi.pitch = ofRandom(128);
        //                midi.control = ofRandom(128);
        //                midi.value = ofRandom(128);
        //                //les d√©s sont truqu√©s
        //                if(ofRandom(1)<0.6)
        //                    midi.pitch = 36 + ofRandom(16);
        //                midi.value = ofRandom(64);
        //                if(ofRandom(1)<0.2)
        //                    midi.value = 127;
        //
        //                int stat = (int) ofRandom(3);
        //                switch(stat){
        //                    case 0 :  midi.status = MIDI_NOTE_OFF;break;
        //                    case 1 : { midi.status = MIDI_NOTE_ON;
        //                        stressPending.push_back(midi);
        //                    }break;
        //                    case 2 : { midi.status = MIDI_CONTROL_CHANGE;
        //                        stressPending.push_back(midi);
        //                    }break;
        //                }
        //                newMidiMessage(midi);
        //            }
        //        for(int i=0;i<stressPending.size();i++){
        //            if(ofRandom(1)<0.01){
        //                ofxMidiMessage midi = stressPending[i];
        //                if(midi.status==MIDI_NOTE_ON)
        //                    midi.status = MIDI_NOTE_OFF;
        //                midi.value = 0;
        //                newMidiMessage(midi);
        //                stressPending.erase(stressPending.begin()+i);
        //            }
        //        }
    }
}


void eqkoscope::updateSerial(){
    if(strip!=0)
        strip->update();
}

