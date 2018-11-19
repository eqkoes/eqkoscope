#include "eqkoscope.h"



float skew_lastDate = 0;

int lastChangedParameters[N_PARAM];
int nextTimeStamp = 0;
vector<string> logFileData;
int logFileDataIndex = -1;
void eqkoscope::checkForParametersChange(){
   
    if(recordAndRenderFlag==0){
        std::vector<int> changedParameters;

        for(int p=0;p<N_PARAM;p++){
            if(abs(lastChangedParameters[p] - parameterMap[p]) > BOOL_PARAM_EPSILON){
                changedParameters.push_back(p);
                lastChangedParameters[p] = parameterMap[p];
            }
        }
        
        if(changedParameters.size()>0){
            logfile << "t," << ofGetElapsedTimeMillis() - processingStartDate << endl;
            for(int p=0;p<changedParameters.size();p++){
                logfile << changedParameters[p] << "," <<  parameterMap[changedParameters[p]] << endl;
            }
        }
    }
    
    if(recordAndRenderFlag==1){
        cout << "next time stamp " << nextTimeStamp << endl;
        if(logFileData.size()==0){ //init bourrin
            string data;
            ofBuffer buf = logfile.readToBuffer();
            logFileData = ofSplitString(buf.getText(), "\n");
        }
        u_long t = (ofGetFrameNum()-processingStartFrame) * 1000/maxFrameRate;
        if(nextTimeStamp <= t){ //load
            string line;
            line = logFileData[++logFileDataIndex];
            while(line.substr(0,2).compare("t,")!=0 && logFileDataIndex < logFileData.size()-1){
                int p = ofToInt(ofSplitString(line, ",")[0]);
                float v = ofToFloat(ofSplitString(line, ",")[1]);
                deltaMap[p] = parameterMap[p] = v;
                line = logFileData[++logFileDataIndex];
            }
            
            if(line.substr(0,2).compare("t,")==0){
                nextTimeStamp =  ofToInt(ofSplitString(line, ",")[1]);
            }
        }
        if(t - nextTimeStamp > 2000){
            exit();
        }
    }
    
}

void eqkoscope::loadConfig(){
#ifdef OF_10
    string t = ofBufferFromFile("../config.csv").getText();
#else
    string t = ofBufferFromFile(ofFilePath::getCurrentWorkingDirectory() + ("/../../../config.csv")).getText();
#endif
    if(t.compare("")){
        ofStringReplace(t, "\r", "\n");
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
                if(!param.compare("dual_display"))
                    dualDisplay = ofToInt(value);
                else
                    if(!param.compare("resolution") && value.compare("native")){
                        vector<string> values = ofSplitString(value, "x");
                        if(values.size()>=2){
                        render_WIDTH = ofToInt(values[0]);
                        render_HEIGHT = ofToInt(values[1]);
                        crt_WIDTH =  render_WIDTH;
                        crt_HEIGHT =  render_HEIGHT;
                        FINALWIDTH =  render_WIDTH;
                                FINALHEIGHT =  crt_HEIGHT;
                    }
                    }else if(!param.compare("width") && value.compare("native")){
                        FINALWIDTH = ofToInt(value);
                    }else if(!param.compare("height") && value.compare("native")){
                        FINALHEIGHT = ofToInt(value);
                    }else if(!param.compare("multi_projector")){
                        MULTIPROJECTOR = ofToInt(value);
                    }else if(!param.compare("gif_width") && value.compare("native")){
                        GIF_WIDTH = ofToInt(value);
                    }else if(!param.compare("gif_height") && value.compare("native")){
                        GIF_HEIGHT = ofToInt(value);
                    }else if(!param.compare("macro_path"))
                        macroPath = (value);
                    else if(!param.compare("fp"))
                        featuredParameter = getParameterFromName(value);
                    else if(!param.compare("MIDIMap"))
                        MIDIMapPath = (value);
                    else if(!param.compare("enforce_MIDI")){
                        for(int f=1;f<fields.size();f++)
                        MIDI_device_connected[fields[f]] = 0;
                    }
        }
    }
}

void eqkoscope::loadMappingFiles(std::string paths, bool factory, bool erase)
{
    vector<string> splittedPaths = ofSplitString(paths, ":");
    
    for(int p = 0;p<splittedPaths.size();p++){
        string path = splittedPaths[p];
#ifdef OF_10
        string str = ofBufferFromFile("../"+path).getText();
#else
        string str = ofBufferFromFile(ofFilePath::getCurrentWorkingDirectory() +  "/../../../" + path).getText();

#endif
        loadMapping(str, factory, erase && p==0);
    }
}

void eqkoscope::loadMapping(std::string str, bool factory, bool erase){
    
    if(erase){
        eraseControlMapping(factory);
    }
    
    autoComments = "";
    string t = str;
    if(t.compare("")){
//        ofStringReplace(t, " ", "\n");
        ofStringReplace(t, "; ", ";"); //debug...
        ofStringReplace(t, "\r", ""); //debug...
        vector<string> lines = ofSplitString(t, "\n");
//        vector<string> lines = ofSplitString(t, "\n");
//        if(lines.size()<2) // FOR WINDOWS
//            lines = ofSplitString(t, "\r");
//        if(lines.size()<2){ // FOR WTF
//            string str = "";
//            const char space = 32;
//            str.append(&space);
//            lines = ofSplitString(t, str);
//        }
        if(lines.size()<2){ // FOR WTF
            ofStringReplace(t, "\n", ""); //debug...
            lines = ofSplitString(t, ";");
        }
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
    illuShader.load("../shaders/illu");
    
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
        for(int i=0;i<17;i++)
            for(int j=0;j<127;j++){
                ccAutoFastMap[i][j].clear();
                noteAutoFastMap[i][j].clear();
            }
    }
}

void eqkoscope::automationsSanityCheck(){
    automationsSanityCheck(&leapAutos);
    automationsSanityCheck(&audioAutos);
    automationsSanityCheck(&oscAutos);
    automationsSanityCheck(&midiNoteonAutos);
    automationsSanityCheck(&midiCCAutos);
    automationsSanityCheck(&timedAutos);
}

void eqkoscope::automationsSanityCheck(vector<Auto*>* autos){
    int index = 0;
    for(int index = 0; index < autos->size() ; index++){
        int pid = (*autos)[index]->parameterID;
        for(int i = index + 1; i < autos->size();i++){
            if((*autos)[i]->parameterID == pid){
                autos->erase(autos->begin() + index);
                index --;
            }
        }
    }
}

Auto* eqkoscope::createCommand(string commandString){
    vector<string> args = ofSplitString(commandString, " ");
    vector<string> fields = ofSplitString(args[0], ",");
    
    
    if(fields.size()<2 || args.size()<1)
        return NULL;
    
    string triggerTypeStr = fields[0];
    if(getParameterFromName(fields[1]) == -1)
        return NULL;
    int pid = getParameterFromName(fields[1]);
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
        if(args[1].find("int")!=string::npos)
            a->is_int = true;
        if(args[1].find("transcient")!=string::npos)
            a->transcient = true;
        if(args[1].find("saw")!=string::npos)
            a->saw = true;
    }
    
    if(!triggerTypeStr.compare("AUDIO")){
        if(fields.size()==5){
            a->type = AUDIO;
            a->channel = ofToInt(fields[4]);
            vector<string> v = ofSplitString(fields[2], "/");
            a->smoothing = ofToFloat(fields[3]);
            parameterEasingMap[pid] = a->smoothing;
            if(v.size()>=1){
                for(int vIndex=0;vIndex<v.size();vIndex++)
                    a->addValue(ofToFloat(v[vIndex]));
                return a;
            }
        }
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
                return a;
            }
        }
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
                return a;
            }
        }
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
                return a;
            }
        }
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
                return a;
            }
        }
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
                
                return a;
            }
        }
    }
    if(!triggerTypeStr.compare("BPM")){
        if(fields.size()==4){
            a->type = BPM;
            a->BPMDivision = ofToFloat(fields[3]);
            vector<string> v = ofSplitString(fields[2], "/");
            if(v.size()>=1){
                for(int vIndex=0;vIndex<v.size();vIndex++)
                    a->addValue(ofToFloat(v[vIndex]));
                return a;
            }
        }
    }
    
    return NULL;
}


void eqkoscope::addCommand(string commandString, bool factory){
    Auto* a = createCommand(commandString);
    
    if(a==NULL)
        return;
    
    vector<string> args = ofSplitString(commandString, " ");
    vector<string> fields = ofSplitString(args[0], ",");
    
    if(fields.size()<2)
        return;
    
    string triggerTypeStr = fields[0];
    
    if(!triggerTypeStr.compare("AUDIO")){
                audioAutos.push_back(a);
                automationsSanityCheck(&audioAutos);
        return;
    }
    if(!triggerTypeStr.compare("OSC")){
                       oscAutos.push_back(a);
                automationsSanityCheck(&oscAutos);
        return;
    }
    if(!triggerTypeStr.compare("NOTEON")){
        
                if(factory){
                    factorymidiNoteonAutos.push_back(a);
                    for(int id=a->minId;id<=a->maxId;id++)
                        noteAutoFastMap[a->channel][id-1].push_back(factorymidiNoteonAutos[factorymidiNoteonAutos.size()-1]);
                }else{
                    midiNoteonAutos.push_back(a);
                automationsSanityCheck(&midiNoteonAutos);
                }
        return;
    }
    if(!triggerTypeStr.compare("CC")){
                        if(factory){
                    factorymidiCCAutos.push_back(a);
                    ccAutoFastMap[a->channel][a->minId-1].push_back(factorymidiCCAutos[factorymidiCCAutos.size()-1]);
                }else{
                    midiCCAutos.push_back(a);
                    automationsSanityCheck(&midiCCAutos);
                }
        return;
    }
    if(!triggerTypeStr.compare("LEAP")){
        
                leapAutos.push_back(a);
                automationsSanityCheck(&leapAutos);
        return;
    }
    if(!triggerTypeStr.compare("DUR")){
                       timedAutos.push_back(a);
                automationsSanityCheck(&timedAutos);
        return;
    }
    if(!triggerTypeStr.compare("BPM")){
                      timedAutos.push_back(a);
                automationsSanityCheck(&timedAutos);
        return;
    }
}

void eqkoscope::update(){

#ifdef OFFLINE_RENDER
    updateOfflineMIDI();
#endif
    
    
#ifdef BRASERO
    {
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
    }
#endif
    
    
    ///frame rate control
    if(currentFrameRate < ofGetFrameRate () + 1
       ||
       currentFrameRate != min(maxFrameRate,parameterMap[frameRate])){
        currentFrameRate = min(maxFrameRate,parameterMap[frameRate]);
        ofSetFrameRate(currentFrameRate);
    }
    
    
    //OFFLINE RECORD && RENDER
//    if(ofGetFrameNum()==5){ ///INITIALISATION
//    if(recordAndRenderFlag==0){
//        saveMacro("./record.xml");
//    }else{
//        if(recordAndRenderFlag==1){
//            loadMacro("./record.xml");
//        }
//    }
//    }
//    
    if(processingStartDate>0)
        checkForParametersChange();
    
    
//    if(parameterMap[MIDIMappingAutoLoad]>BOOL_PARAM_EPSILON && ofGetFrameNum()%60==0){
//        loadMapping(MIDIMapPath, true, true);
//    }
    
    if(!kinectRunning && parameterMap[kinect]){ //start the kinect server
        cout << ofSystem("open ../../../FeedbackUtils.app") << endl;
        kinectRunning = true;
    }
    
    if(pause)
        return;

    
    float date = ofGetElapsedTimeMicros();
    
    /** EVENTS **/
    
    if(parameterMap[randomJump]>0){
        float z = parameterMap[mediaZ];
        parameterEasingMap[mediaX] = parameterMap[speed];
        parameterEasingMap[mediaY] = parameterMap[speed];
        deltaMap[mediaX] = ofRandom(-z,z);
        deltaMap[mediaY] = ofRandom(-z,z);
        parameterMap[randomJump] = deltaMap[randomJump] = 0;
    }
    

    
    if(deltaMap[macro]>0){
        niceRandom(deltaMap[macro] - 1);
        deltaMap[macro] = parameterMap[macro] = 0;
    }
    
    updateMidi();
    updateMacros();
    updateSerial();    
//    guiCanvas->update();


    doStressTest();
    
    for(int sp : surpriseParameters)
        deltaMap[sp] = parameterMap[sp] = scaleGUIValue(sp, parameterMap[surprise]);
    
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
    
    if(parameterMap[background]==1 && cinemas.size()>1 && cinemas[1]!=NULL){
        cinemas[1]->update();
    }
    
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
//    for(int i=0;i<scenes.size();i++)
//        if(scenes[i]==three){
//            three->update();
//            break;
//        }
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
            float v = parameterMap[i] + parameterEasingMap[i]*(deltaMap[i]-parameterMap[i]);
            if(abs(v)<0.000001)
                v = 0;
            parameterMap[i] = v;
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

void eqkoscope::updateGUI(){
//    paramPanel->clear();
    
    for(int i = 0;i<parametersInGUI.size();i++){
        int id = parametersInGUI[i];
//        sliders[i]->setup(parameterIDMap[id], deltaMap[id], parametersInGUIBounds[i][0], parametersInGUIBounds[i][1]);
//        paramPanel->add((sliders[i]));
        
    }
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

