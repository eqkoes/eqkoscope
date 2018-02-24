#include "eqkoscope.h"
#include <GLFW/glfw3.h>

struct token_name {
    GLuint count;
    GLenum token;
    const char *name;
};

static void
print_limits(void)
{
    
    static const struct token_name openglLimits[] = {
        { 1, GL_MAX_ATTRIB_STACK_DEPTH, "GL_MAX_ATTRIB_STACK_DEPTH" },
        { 1, GL_MAX_CLIENT_ATTRIB_STACK_DEPTH, "GL_MAX_CLIENT_ATTRIB_STACK_DEPTH" },
        { 1, GL_MAX_CLIP_PLANES, "GL_MAX_CLIP_PLANES" },
        { 1, GL_MAX_COLOR_MATRIX_STACK_DEPTH, "GL_MAX_COLOR_MATRIX_STACK_DEPTH" },
        { 1, GL_MAX_ELEMENTS_VERTICES, "GL_MAX_ELEMENTS_VERTICES" },
        { 1, GL_MAX_ELEMENTS_INDICES, "GL_MAX_ELEMENTS_INDICES" },
        { 1, GL_MAX_EVAL_ORDER, "GL_MAX_EVAL_ORDER" },
        { 1, GL_MAX_LIGHTS, "GL_MAX_LIGHTS" },
        { 1, GL_MAX_LIST_NESTING, "GL_MAX_LIST_NESTING" },
        { 1, GL_MAX_MODELVIEW_STACK_DEPTH, "GL_MAX_MODELVIEW_STACK_DEPTH" },
        { 1, GL_MAX_NAME_STACK_DEPTH, "GL_MAX_NAME_STACK_DEPTH" },
        { 1, GL_MAX_PIXEL_MAP_TABLE, "GL_MAX_PIXEL_MAP_TABLE" },
        { 1, GL_MAX_PROJECTION_STACK_DEPTH, "GL_MAX_PROJECTION_STACK_DEPTH" },
        { 1, GL_MAX_TEXTURE_STACK_DEPTH, "GL_MAX_TEXTURE_STACK_DEPTH" },
        { 1, GL_MAX_TEXTURE_SIZE, "GL_MAX_TEXTURE_SIZE" },
        { 1, GL_MAX_3D_TEXTURE_SIZE, "GL_MAX_3D_TEXTURE_SIZE" },
        { 1, GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB, "GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB" },
        { 1, GL_MAX_RECTANGLE_TEXTURE_SIZE_NV, "GL_MAX_RECTANGLE_TEXTURE_SIZE_NV" },
        { 1, GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB, "GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB" },
        { 1, GL_MAX_TEXTURE_UNITS_ARB, "GL_MAX_TEXTURE_UNITS_ARB" },
        { 1, GL_MAX_TEXTURE_LOD_BIAS_EXT, "GL_MAX_TEXTURE_LOD_BIAS_EXT" },
        { 1, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, "GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT" },
        { 2, GL_MAX_VIEWPORT_DIMS, "GL_MAX_VIEWPORT_DIMS" },
        { 2, GL_ALIASED_LINE_WIDTH_RANGE, "GL_ALIASED_LINE_WIDTH_RANGE" },
        { 2, GL_SMOOTH_LINE_WIDTH_RANGE, "GL_SMOOTH_LINE_WIDTH_RANGE" },
        { 2, GL_ALIASED_POINT_SIZE_RANGE, "GL_ALIASED_POINT_SIZE_RANGE" },
        { 2, GL_SMOOTH_POINT_SIZE_RANGE, "GL_SMOOTH_POINT_SIZE_RANGE" },
        { 0, (GLenum) 0, NULL }
    };
    GLint i, max[2];
    cout << "OpenGL limits:\n";
    for (i = 0; openglLimits[i].count; i++) {
        glGetIntegerv(openglLimits[i].token, max);
        if (glGetError() == GL_NONE) {
            if (openglLimits[i].count == 1)
                cout << "    " << openglLimits[i].name << " = " << max[0] << "\n";
            else /* XXX fix if we ever query something with more than 2 values */
                cout << "    " << openglLimits[i].name << " = " << max[0] << ", " << max[1] << "\n";
        }
    }
}


using namespace std;

void eqkoscope::setup(){
//    std::streambuf * old = std::cerr.rdbuf(stderrBuffer.rdbuf()); //redirection of stderr
    
    
#ifdef OFFLINE_RENDER
    initOfflineMIDI();
#endif
    
#ifdef LAZERS
    etherdream.setup();
    etherdream.setPPS(30000);
#endif
    
    roundMaskImg.loadImage("assets/softroundmask_720p.png");
    interMask.loadImage("assets/tubeMask.png");
    mappingMask.loadImage("assets/mappingMask.png");
    
#ifdef _LEDS_STRIP
    strip = new LedStrip(this);
    strip->setup();
#endif
    

    
    initParameterMap();
    initParameters();
    for(int i=0;i<=N_PARAM;i++){ //sanity
        deltaMap[i] = parameterMap[i];
        parameterEasingMap[i] = 1;
    }
    //events : do not update
    parameterEasingMap[autoSwitchImg] = 0;
    
    parameterEasingMap[scale] = 0.7;
    parameterEasingMap[offx] = 0.1;
    parameterEasingMap[offy] = 0.1;
    
    //immediate params
//    parameterEasingMap[draw_recording] = 1;
    
    
    //print_limits();
    
    fxShader.load("../shaders/fx");
    trapezeShader.load("../shaders/trapeze");
    
#ifdef LEAPMOTION
    leap.open();
    leap.setReceiveBackgroundFrames(true);
    leapX.resize(2);
    leapY.resize(2);
    leapZ.resize(2);
    leapDX.resize(2);
    leapDY.resize(2);
    leapDZ.resize(2);
    leapRoll.resize(2);
#endif
    
#ifdef KINECT
    kinectDevice.init(true, false, false);
    kinectDevice.open();
#endif
    
#ifdef INSTALL_THOUGHTS
     thougts = new Thoughts(this);
#endif
    
    float date = 0;
    
    font.loadFont("Avenir", 16);
    font2.loadFont("Avenir", 24);
    
    cout << "dateA " << ofGetTimestampString() << endl;

    initMidi();
    loadMacroMap();
    
    initOSC();
    cout << "dateB " << ofGetTimestampString() << endl;
    
    utils = new Utils(this);
    
    freezer = new Freezer(this);
    
    if(liveMode)
        safeMode = true;
    
    cout << "dateC " << ofGetTimestampString() << endl;

    initGlitches(this);
    cout << "dateD " << ofGetTimestampString() << endl;

    
    /** SCENES **/
    for(int i=0;i<3;i++){
        scenes.push_back(0);
    }
    
        cout << "dateE " << ofGetTimestampString() << endl;
    feedbackScene = new Feedback(this); //0.
    //    pointback = new PointBack(this);
        cout << "dateE1 " << ofGetTimestampString() << endl;
    cinema = new Cinema(this);
        cout << "dateE11 " << ofGetTimestampString() << endl;
    uzi = new Uzi(this);
        cout << "dateE2 " << ofGetTimestampString() << endl;
    fractal = new Fractal(this);
    agents = new Agents(this);

    
    for(int i=0;i<1;i++){
        cinemas.push_back(new Cinema(this));
        cinemas[i]->setSleep(1);
    }
    for(int i=0;i<1;i++){
        uzis.push_back(new Uzi(this));
        uzis[i]->setSleep(1);
    }
    
    drawscene = new Draw(this);
    print = new Print(this);
    lines = new Lines(this);
    glitch = new Glitch(this);
    three = new Three(this);
#ifdef MAPPING
     mapped = new Mapped(this);
        mapped->nestedScene = cinema;
#endif
    
    allScenes.push_back(feedbackScene);
    allScenes.push_back(cinema);
    allScenes.push_back(uzi);
    allScenes.push_back(fractal);
    allScenes.push_back(agents);
    allScenes.push_back(drawscene);
    allScenes.push_back(print);
    allScenes.push_back(lines);
    allScenes.push_back(glitch);
    allScenes.push_back(three);
    
    scenes[0] = feedbackScene;
//    scenes[0] = three;
#ifdef MAPPING
    scenes[0] = mapped;
#endif
    blackNWhiteMedia = true;
    scenes[0]->setup();
    
    analyzeMacros();
    
    i.allocate(WIDTH,HEIGHT, OF_IMAGE_COLOR);
    audioImg.allocate(audioCapture, audioCapture, OF_IMAGE_COLOR);
    
    /** END OF SCENES **/
    
    /** AUDIO **/
    initAudio();
    
    loadShaders();
    
    fbo.allocate(WIDTH, HEIGHT2, GL_RGBA);
    fbo2.allocate(WIDTH, HEIGHT2, GL_RGBA);
    tempFbo.allocate(WIDTH, HEIGHT2, GL_RGBA);
    tempFbo2.allocate(WIDTH, HEIGHT2, GL_RGBA);
    psyFbo.allocate(WIDTH, HEIGHT2, GL_RGBA);
    
    copyFbo.allocate(WIDTH,HEIGHT);
    
    grabFbo = new ofFbo;
    grabFbo->allocate(WIDTH, HEIGHT2);
    
    logfile.open("./log.txt",ofFile::WriteOnly);
        
    ofDirectory strokeDir("brushes/");
    strokeDir.listDir();
    for(int i=0;i<strokeDir.numFiles();i++){
        string picpath = strokeDir.getPath(i);
        if(picpath.find(".jpg")!=string::npos || picpath.find(".png")!=string::npos || picpath.find(".gif")!=string::npos||picpath.find(".JPG")!=string::npos || picpath.find(".PNG")!=string::npos || picpath.find(".GIF")!=string::npos
           || picpath.find(".tiff")!=string::npos){
                brushstrokes.push_back(new ofImage(picpath));
        }}

    
    int brushDownSize = 2;
    for(int i=0;i<brushstrokes.size();i++){
        brushstrokes[i]->resize(brushstrokes[i]->width/brushDownSize,brushstrokes[i]->height/brushDownSize);
    }
    
    for(int i=0;i<NB_PAINT_FRAMES;i++){
        paintFbos.push_back(new ofFbo);
        paintFbos[i]->allocate(WIDTH, HEIGHT);
    }
    paintFbo.allocate(WIDTH, HEIGHT);
    
    ofSetFrameRate(restrictFrameRate);
    
    
    //test
//    drawscene->embedScene = cinema;
    
    for(int i=0;i<MAX_NB_ECHOES;i++){
        ofFbo* fbo = new ofFbo;
        fbo->allocate(WIDTH,HEIGHT2,GL_RGBA);
        echoFbos.push_back(fbo);
    }
    
    if(dualDisplay){
    mainDisplay = glfwGetCurrentContext();
    glfwWindowHint(GLFW_DECORATED, true);
    secondDisplay = glfwCreateWindow(720, 405, "Aux", NULL, mainDisplay);
    glfwSetWindowPos(secondDisplay,WIDTH-320,0);
    }
    
    if(liveMode)
        deltaMap[tintBrightness] = parameterMap[tintBrightness] = 0;

//    int tcpPort = ofRandom(10000,20000);
    int tcpPort = 12345;
#ifndef MASTER
    tcpServer.setup(tcpPort);
#endif
#ifndef SLAVE
    tcpClient.setup("192.168.0.25", tcpPort);
#endif
    
#ifdef OFFLINE_RENDER
    savingGif = true;
    gifIndex = int(ofRandom(1,100000));
#endif

    stressTestFilterList.push_back(audio);
    stressTestFilterList.push_back(_audioIn);
    stressTestFilterList.push_back(paint);
    stressTestFilterList.push_back(paint2);
    stressTestFilterList.push_back(mediaZ);
    stressTestFilterList.push_back(_mask);
    stressTestFilterList.push_back(movieSpeed);
    stressTestFilterList.push_back(useJoyStick);
    stressTestFilterList.push_back(tintBrightness);
    stressTestFilterList.push_back(leapAutoReco);
    stressTestFilterList.push_back(test);
    stressTestFilterList.push_back(debug);
    stressTestFilterList.push_back(kinect);
    
    genetic = new Genetic(this);
#ifdef GENETIC
    randomParameters();
#endif
    
#ifdef INSTALL_COCHON
    addCommand("NOTEON,user3,100/100,1,0,36/48", true);
#endif
    
    //
//    loadMacro("/Users/Raph/Dev/of_v0.8.4_osx_release/apps/myApps/Feedback/bin/data/Projects/Stienis/lightning.xml");
    
    // override the default codecs if you like
    // run 'ffmpeg -codecs' to find out what your implementation supports (or -formats on some older versions)
//    for(int i = 0;i<numVidRec;i++){
//        ofxVideoRecorder v;
//        vidRecorder.push_back(v);
    sampleRate = 44100;
    channels = 2;
    
    vidRecorder.setVideoCodec("h264");
    vidRecorder.setVideoBitrate("15000k");
    vidRecorder.setAudioCodec("mp3");
    vidRecorder.setAudioBitrate("192k");

    vidRecorder.setup("capture/eqkoCapture_"+ofToString(int(ofRandom(10000)))+fileExt, WIDTH, HEIGHT, 30, sampleRate, channels);

    gui.loadFont("Avenir", 9);
	ofParameter<float> radius;
    gui.setup("eqkoscope v0.∞");
    gui.setDefaultBackgroundColor(ofColor(0,0,0,0));
    for(int i = 0;i<parametersInGUI.size();i++){
        int id = parametersInGUI[i];
        PSlider* slider = new PSlider(this, id);
        slider->setup(parameterIDMap[id], deltaMap[id], parametersInGUIBounds[i][0], parametersInGUIBounds[i][1]);
        slider->setDefaultHeight(12);
        sliders.push_back(slider);
        gui.add((sliders[i]));
    }

    
//    }
//    ofAddListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
}

void eqkoscope::swapFBOs(ofFbo* a, ofFbo* b){
    srcFbo = (srcFbo==&fbo) ? &fbo2 : &fbo;
    curFbo = (curFbo==&fbo) ? &fbo2 : &fbo;
//    ofFbo* temp = a;
//    a = b;
//    b = temp;
}

void eqkoscope::draw(){

    if(!shadowFrame && shadowFrames>0){
        shadowFrame = true;
        for(int i = 0; i < shadowFrames ; i++){
            draw();
            update();
        }
        shadowFrame = false;
    }
    
    trueFrameNum++;
    
#ifndef SLAVE
    return;
#endif

    
    if(pause)
        return;
    
    long date = ofGetElapsedTimeMicros();
    
    ofPushMatrix();
    {
    
    
    doGlitches = false;
    float tglitch = 1000/parameterMap[glitchFreq];
#ifdef REAL_TIME_FX
    tglitch /= restrictFrameRate/ofGetFrameRate();
#endif
    if(parameterMap[glitchFreq]>=30 || (ofGetElapsedTimeMillis() - lastGlitchDate) > tglitch
       || ofRandom(ofGetElapsedTimeMillis() - lastGlitchDate) > tglitch){
        doGlitches = true;
        lastGlitchDate = ofGetElapsedTimeMillis();
    }
    
    ofTranslate((ofGetWidth() - WIDTH)/2, (ofGetHeight() - HEIGHT)/2);
   
    if(!guiVisible)
        ofHideCursor();
    else
        ofShowCursor();
    
    if(strip!=0)
        strip->draw();
    
    srcFboMutex.lock();

    
    ofBackground(0);
    ofSetColor(ofColor::black);
    ofRect(0,0,WIDTH,HEIGHT2);
    ofSetColor(ofColor::white);
    
    //    fbo.begin(); //CPU
    
    ofPushMatrix();
    
    grabFbo->begin();
    sceneMutex.lock();
    if(scenes.size()>0 && scenes[0]!=0)
        scenes[0]->draw();
#ifdef KINECT
//    if(kinectDevice.isFrameNew()){
//        kinectDevice.draw(0, 0);
//    }
#endif
    ofSetColor(255);
    sceneMutex.unlock();
    
    grabFbo->end();
    
    ofPopMatrix();
    
   fbo.begin(); //CPU
    
    grabFbo->draw(0, 0);
    
    if (parameterMap[stripesAmp]>0)
        for (int i=0; i<parameterMap[stripesAmp]*5; i++)
            stripe(parameterMap[stripesSize]);
    
    
    if (parameterMap[post_traitement]){
        ofSetColor(parameterMap[tintBrightness]*255, parameterMap[tintBrightness]*255, parameterMap[tintBrightness]*255);
        ofPushMatrix();
        sceneMutex.lock();
        if(scenes[0]!=0){
            scenes[0]->mask();
            scenes[0]->capture(grabFbo);
        }
        sceneMutex.unlock();
        ofPopMatrix();
    }
    
        fbo.end();
    
    /** FX **/
    curFbo = &fbo2;
        srcFbo = &fbo;
    
    applyFXChain(srcFbo, curFbo);
    
    //need mutex
    if (!(parameterMap[post_traitement]>BOOL_PARAM_EPSILON)){
        ofSetColor(parameterMap[tintBrightness]*255);
        ofPushMatrix();
        sceneMutex.lock();
        scenes[0]->capture(srcFbo); //2
        scenes[0]->mask(); //1
        sceneMutex.unlock();
        ofPopMatrix();
    }
    
    /** GLOBAL PARAMETERS **/
    
    saturationShader.begin();
    saturationShader.setUniform1f("saturation", parameterMap[tintSaturation]);
    curFbo->begin();
    srcFbo->draw(0,0);
    curFbo->end();
    saturationShader.end();
    swapFBOs(srcFbo, curFbo);
    
    
    ofSetColor(parameterMap[tintBrightness]*255);
    if(parameterMap[flash])
        ofSetColor(ofRandom(255-parameterMap[flash]*parameterMap[tintBrightness]*255));
    else
        ofSetColor(parameterMap[tintBrightness]*255);
    
    curFbo->begin();
    if(parameterMap[antiAliasing]>BOOL_PARAM_EPSILON){
//        fxShader.load("../shaders/fx");
        fxShader.begin();
        fxShader.setUniform1f("alpha", parameterMap[tintBrightness]);
        fxShader.setUniform1f("alphaMode", parameterMap[fadeOutMode]);
        fxShader.setUniformTexture("tDiffuse", srcFbo->getTextureReference(), 0);
        srcFbo->draw(xOffset, yOffset);
        fxShader.end();
    }else{
        srcFbo->draw(xOffset,-(HEIGHT2-HEIGHT)/2 + yOffset);
    }
    curFbo->end();
    swapFBOs(srcFbo, curFbo);
    
    /** END OF GLOBAL PARAMETERS **/

    
    ofBackground(0);
    
    ofPushMatrix();
    
    ofTranslate(-(FINALWIDTH-WIDTH)/2, -(FINALHEIGHT-HEIGHT)/2);
    ofScale(FINALWIDTH/(float)WIDTH, FINALHEIGHT/(float)HEIGHT);
//    ofScale(1, 1); //hack for installation (no rescaling)
    
    ofBackground(0);
    
    displayDualScenes();
    
    applyPostProcess();
    
    doPaint();
    
    sceneMutex.lock();
    if(scenes[0]!= 0 && scenes[0]==mapped)
        scenes[0]->mask();
    sceneMutex.unlock();
    
    /** ECHO RECORD **/
    if(parameterMap[echoNb]>0){ //record each frame
        currentEcho = (++currentEcho) % MAX_NB_ECHOES;
        echoFbos[currentEcho]->begin();
        ofSetColor(255);
        srcFbo->draw(0,0);
        applyMask();
        echoFbos[currentEcho]->end();
    }

#ifdef USE_FREEZE
    if (app->parameterMap[nFreeze]==0 && app->parameterMap[nBlocks] ==0)
        freezer->clear();
#endif
    
    
    /** ECHOES **/
    if(parameterMap[echoNb]>0){
        for(int e=0;e<parameterMap[echoNb];e++){
            int index;
            if(parameterMap[echoRandom]==0)
                index = (currentEcho - (e+1)*int(parameterMap[echoPeriod]/1000.0 * ofGetFrameRate()) + MAX_NB_ECHOES) % MAX_NB_ECHOES;
            else
                index = ofRandom(parameterMap[echoPeriod]/1000.0 * ofGetFrameRate()); //test random echos
            if(index >= 0 && index<MAX_NB_ECHOES){
                blendShader.begin();
                blendShader.setUniform1i("mode", 0);
                if(parameterMap[echoAdjust]>=1)
                    blendShader.setUniform1f("_mix", 1-(e)/float(parameterMap[echoNb]));
                else
                    blendShader.setUniform1f("_mix", 1);
                blendShader.setUniform1f("thresh", 0.5);
                echoFbos[index]->draw(0,0);
                blendShader.end();
            }
        }
    }
    
    /** KINECT **/
    
    if(parameterMap[kinect] && kinectMesh.getNumVertices()>0){
        kinectMesh.setMode((ofPrimitiveMode)parameterMap[kinectRender]);
        ofPushMatrix();
        ofTranslate(0, HEIGHT/2);
        float s = parameterMap[kinectScale];
        ofScale(s, s, s);
        kinectMesh.draw();
        ofPopMatrix();
    }
    
    srcFboMutex.unlock();
    
    
    ofSetColor(ofColor::black); //borders
    ofRect(0, -(HEIGHT2-HEIGHT)/2, WIDTH, 0);
    ofRect(0, HEIGHT, WIDTH, HEIGHT2+1);
    ofSetColor(ofColor::white);
    
    if(parameterMap[audio]){
        audioImg.grabScreen(WIDTH/2-audioCapture/2, HEIGHT/2 - audioCapture/2, audioCapture, audioCapture);
    }
    
    }
    ofPopMatrix();
    
    if(analyzeAudioB || savingGif || analyzeImg){
        i.grabScreen(0,0,WIDTH,HEIGHT);
        if(analyzeAudioB)
            analyzeAudio();
        if(savingGif){
            i.resize(GIF_WIDTH, GIF_HEIGHT);
            i.update();

            
            if(!hasTakenVideo){
                    bool success = vidRecorder.addFrame(i.getPixelsRef());
                    if (!success)
                        ofLogWarning("This frame was not added!");
//                }
                if (vidRecorder.hasVideoError())
                    ofLogWarning("The video recorder failed to write some frames!");
                if (vidRecorder.hasAudioError())
                    ofLogWarning("The video recorder failed to write some audio samples!");
            }else{
                            i.saveImage("capture/gif_"+ofToString(gifIndex)+"/"+ofToString(ofGetFrameNum())+".jpg", OF_IMAGE_QUALITY_BEST);
            }
            
            isGifRecording = true;
        }
    }
    
    if(saveFrame){
        i.grabScreen(0, 0 , WIDTH, HEIGHT);
        stringstream str;
        str << "./capture/" << int(ofRandom(1,100000)) << ".png";
        i.saveImage(str.str());
        if(currentMacroStr.compare("")){
            string p = currentMacroStr;
            ofStringReplace(p, ".xml", ".jpg");
            i.saveImage(p);
        }
        saveFrame = false;
    }
    
    drawMicros += microsEasing*(ofGetElapsedTimeMicros() - date - drawMicros);;
    
    if(secondDisplay!=0){
        glfwMakeContextCurrent(secondDisplay);
    int width, height;
    glfwGetFramebufferSize(secondDisplay, &width, &height);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    ofBackground(0, 0, 0);
    srcFbo->draw(0,0);
    }

    displayInfo(FINALWIDTH, FINALHEIGHT);
    
    if(guiVisible){
//        gui.setDefaultFillColor(gui.getFillColor());
        gui.draw();
    }
    
    ofPopMatrix();
    
    if(secondDisplay!=0){
        glfwSwapBuffers(secondDisplay);
        glfwPollEvents();
        glfwMakeContextCurrent(mainDisplay);
    }
    
#ifdef LAZERS
    ildaFrame.update();
    ildaFrame.draw(0, 0, ofGetWidth(), ofGetHeight());
    etherdream.setPoints(ildaFrame);
#endif
    
    sceneMutex.unlock();
}


void eqkoscope::applyFXChain(ofFbo* a, ofFbo* b){
    
    if(parameterMap[psyShift]){ //save the original content
        psyFbo.begin();
        srcFbo->draw(0,0);
        psyFbo.end();
    }
    
    if(doGlitches){
#ifdef USE_FREEZE
#ifdef USE_FREEZE
        if(doGlitches){
            for (int i=0; i<app->parameterMap[nBlocks]; i++)
                freezer->ablock();
            
            for (int j=0; j<app->parameterMap[nFreeze]; j++)
                freezer->shift(srcFbo);
        }
#endif
        
        if(app->parameterMap[nFreeze]>1 || app->parameterMap[nBlocks]>=1){
            srcFbo->begin();
            freezer->display();
            srcFbo->end();
        }
#endif
        
        if(parameterMap[noise]>BOOL_PARAM_EPSILON ){
            doNoise(srcFbo, curFbo);
            swapFBOs(srcFbo, curFbo);
        }
        
        if(parameterMap[kalei]>BOOL_PARAM_EPSILON){
            doKalei(srcFbo, curFbo, parameterMap[kaleiNb], parameterMap[kaleiOffX], parameterMap[kaleiOffY]);
            swapFBOs(srcFbo, curFbo);
        }
        
        if(doGlitches){
            
            
            if(parameterMap[displaceAmp]>BOOL_PARAM_EPSILON){
                displace(srcFbo, curFbo, parameterMap[displaceAmp]*WIDTH/8, parameterMap[displaceProba], false);
                swapFBOs(srcFbo, curFbo);
            }
            if(parameterMap[displaceVAmp]>BOOL_PARAM_EPSILON){
                displace(srcFbo, curFbo, parameterMap[displaceVAmp]*WIDTH/8, parameterMap[displaceProba], true);
                swapFBOs(srcFbo, curFbo);
            }
            
            if(parameterMap[carder]>BOOL_PARAM_EPSILON){
                displaceGlitch(srcFbo, curFbo, parameterMap[carder], parameterMap[displaceProba], false);
                swapFBOs(srcFbo, curFbo);
            }
        }
        
    if(parameterMap[skewAmp]>0){
        skew(srcFbo, curFbo, parameterMap[skewAmp]/10.0, ofGetFrameNum()*3, 0,skewVector);
        swapFBOs(srcFbo, curFbo);
    }
    if(parameterMap[skewVAmp]>0){
        skew(srcFbo, curFbo, parameterMap[skewVAmp]/3.0, ofGetFrameNum()*3, 1,skewVector);
        swapFBOs(srcFbo, curFbo);
    }
    if(parameterMap[skewAAmp]>0){
        skew(srcFbo, curFbo, parameterMap[skewAAmp]/3.0, ofGetFrameNum()*3, 3,skewVector);
        swapFBOs(srcFbo, curFbo);
    }
    if(parameterMap[skewDAmp]>0){
        skew(srcFbo, curFbo, parameterMap[skewDAmp]/3.0, ofGetFrameNum()*3, 2,skewVector);
        swapFBOs(srcFbo, curFbo);
    }
    }
    
    
    if(parameterMap[pert]>0){
        doPerturbation(srcFbo, curFbo);
        swapFBOs(srcFbo, curFbo);
    }
    
    if(abs(parameterMap[warp])>0){
        doLSD(srcFbo, curFbo);
        swapFBOs(srcFbo, curFbo);
    }
    
    if(parameterMap[toLine]>BOOL_PARAM_EPSILON){
        curFbo->begin();
        lineShader.begin();
        lineShader.setUniform1f("WIDTH", WIDTH);
        lineShader.setUniform1f("HEIGHT", HEIGHT2);
        lineShader.setUniform1f("da", parameterMap[lineDa]);
        lineShader.setUniform1f("dx", parameterMap[lineDx]);
        lineShader.setUniform1f("intensity", parameterMap[toLine]);
        srcFbo->draw(0,0);
        lineShader.end();
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
    }
    
    if(parameterMap[toCircle]>BOOL_PARAM_EPSILON){
        doToCircle(srcFbo, curFbo, parameterMap[toCircle]);
        swapFBOs(srcFbo, curFbo);
    }
    
    if(parameterMap[psyShift]){
        curFbo->begin();
        srcFbo->draw(0,0);
        ofSetColor(255,255,255,(1-parameterMap[psyShift]) * 255);
        psyFbo.draw(0,0);
        ofSetColor(255,255,255,255);
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
    }
    
    //B4 displace etc (glitch block)

    
    if(parameterMap[xpixellate]>BOOL_PARAM_EPSILON || parameterMap[ypixellate]>BOOL_PARAM_EPSILON){
        pixellate(srcFbo, curFbo, parameterMap[xpixellate], parameterMap[ypixellate]);
        swapFBOs(srcFbo, curFbo);
    }
    
    if(parameterMap[vblur]>BOOL_PARAM_EPSILON){
        blur(srcFbo, curFbo, parameterMap[vblur], parameterMap[blurOriginalMix], 5, true);
        swapFBOs(srcFbo, curFbo);
    }
    if(parameterMap[hblur]>BOOL_PARAM_EPSILON){
        blur(srcFbo, curFbo, parameterMap[hblur], parameterMap[blurOriginalMix], 5, false);
        swapFBOs(srcFbo, curFbo);
    }

    if(abs(parameterMap[_gamma]-1)>BOOL_PARAM_EPSILON || abs(parameterMap[contrast])>BOOL_PARAM_EPSILON){
        doGamma(srcFbo, curFbo, parameterMap[_gamma]);
        swapFBOs(srcFbo, curFbo);
    }
    

    
    bool doInvertion = false;
    if(abs(parameterMap[invertCircle])>BOOL_PARAM_EPSILON|| (!parameterMap[strobe] && parameterMap[_invert]>BOOL_PARAM_EPSILON)
       //|| (parameterMap[strobe] && (trueFrameNum%2)!=parameterMap[_invert])
       ){
        doInvertion = true;
    }
    
    if(parameterMap[whitePoint]!=1 || parameterMap[blackPoint]!=0){
        curFbo->begin();
        chromaPointShader.begin();
        chromaPointShader.setUniform1f("whitePoint", parameterMap[whitePoint]);
        chromaPointShader.setUniform1f("blackPoint", parameterMap[blackPoint]);
        srcFbo->draw(0,0);
        chromaPointShader.end();
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
    }
    
    
    if(parameterMap[omg3D]>BOOL_PARAM_EPSILON){ //test
        displayOmg3D(srcFbo, curFbo);
        swapFBOs(srcFbo, curFbo);
    }
    
    
    
    if(abs(parameterMap[bw])>0.5){
//       bwShader.load("../shaders/b&w");
        curFbo->begin();
        bwShader.begin();
        bwShader.setUniform1f("steps", parameterMap[bw]);
        srcFbo->draw(0,0);
        bwShader.end();
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
    }
    
    if(parameterMap[tintSaturation]>0 || parameterMap[_reTint] || 1==1){ //saturation mgmt
        colorGrading(srcFbo, curFbo, doInvertion, parameterMap[tintAmp], parameterMap[_reTint], parameterMap[sidesSaturation] + (blackNWhiteMedia?1:0));
        swapFBOs(srcFbo, curFbo);
    }
//hack
    
    if(parameterMap[sharpen]>BOOL_PARAM_EPSILON){
        curFbo->begin();
        sharpenShader.begin();
        sharpenShader.setUniform1f("sharpen", parameterMap[sharpen]/0.7);
        sharpenShader.setUniform1f("nPixels", parameterMap[sharpen]*3);
        sharpenShader.setUniform1i("mode", (int) parameterMap[sharpenMode]);
        srcFbo->draw(0,0);
        sharpenShader.end();
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
    }
    
    
// 171017 : toLine and toCircle
    
    if(parameterMap[dispose]){
        curFbo->begin();
        disposeShader.begin();
        disposeShader.setUniform1f("WIDTH", WIDTH);
        disposeShader.setUniform1f("HEIGHT", HEIGHT2);
        disposeShader.setUniform1f("freq", parameterMap[disposeFreq]);
        disposeShader.setUniform1f("yShift", parameterMap[dispose]);
        disposeShader.setUniform1f("xShift", parameterMap[disposeX]);
        srcFbo->draw(0,0);
        disposeShader.end();
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
    }
    
    if(parameterMap[squareMandala]>BOOL_PARAM_EPSILON){
        curFbo->begin();
//      if(ofGetFrameNum()%30==0)
//                    squareMandalaShader.load("../shaders/squareMandala");
        squareMandalaShader.begin();
        squareMandalaShader.setUniform1f("WIDTH", WIDTH);
        squareMandalaShader.setUniform1f("HEIGHT", HEIGHT2);
        squareMandalaShader.setUniform1f("t", ofGetFrameNum());
        squareMandalaShader.setUniform1f("resolution", parameterMap[squareMandala]);
        srcFbo->draw(0,0);
        squareMandalaShader.end();
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
    }
    
    if(parameterMap[mandala]>BOOL_PARAM_EPSILON){
        curFbo->begin();
        mandalaShader.begin();
        mandalaShader.setUniform1f("WIDTH", WIDTH);
        mandalaShader.setUniform1f("HEIGHT", HEIGHT2);
        mandalaShader.setUniform1f("t", ofGetFrameNum());
        mandalaShader.setUniform1f("resolution", parameterMap[mandala]);
        srcFbo->draw(0,0);
        mandalaShader.end();
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
    }
    
    if(parameterMap[sobel]>BOOL_PARAM_EPSILON){
        sobelContours(srcFbo, curFbo);
        swapFBOs(srcFbo, curFbo);
    }
    
    
    if(parameterMap[hueFilter]>0){
        curFbo->begin();
        hueFilterShader.begin();
        hueFilterShader.setUniform1f("hue", parameterMap[hueFilter]);
        srcFbo->draw(0,0);
        hueFilterShader.end();
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
    }
    

    
    if(parameterMap[hueShift]){
        curFbo->begin();
        hueShiftShader.begin();
        hueShiftShader.setUniform1f("center", parameterMap[hueShiftCenter]);
        hueShiftShader.setUniform1f("intensity", parameterMap[hueShift]);
        hueShiftShader.setUniform1f("range", parameterMap[hueShiftRange]);
        hueShiftShader.setUniform1f("crazyMode", parameterMap[hueShiftCrazy]);
        srcFbo->draw(0,0);
        hueShiftShader.end();
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
    }
    
    if(parameterMap[hueKalei]){
        curFbo->begin();
//        hueCrazeShader.load("../shaders/hueCraze");
        hueCrazeShader.begin();
        hueCrazeShader.setUniform1f("n", parameterMap[hueKalei]+1);
        srcFbo->draw(0,0);
        hueCrazeShader.end();
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
    }
    
    if(parameterMap[chromaSep]>0){
        chromaSeparation(srcFbo, curFbo, doInvertion, parameterMap[chromaSep]/2, parameterMap[chromaSepHue], parameterMap[chromaSepAngle], parameterMap[chromaOffset]/2, parameterMap[sidesSaturation] + (blackNWhiteMedia?1:0));
        swapFBOs(srcFbo, curFbo);
        if(parameterMap[doubleChromaSep]){
            chromaSeparation(srcFbo, curFbo, doInvertion, parameterMap[chromaSep]/2, parameterMap[chromaSepHue], parameterMap[chromaSepAngle], parameterMap[chromaOffset]/2, parameterMap[sidesSaturation] + (blackNWhiteMedia?1:0));
            swapFBOs(srcFbo, curFbo);
        }
    }
    
    //memory leak
//    if(parameterMap[sortXThresh]>0 || parameterMap[sortYThresh]>0){
//        if(parameterMap[sortXThresh]>BOOL_PARAM_EPSILON){
//            ofPushMatrix();
//            srcFbo->draw(0,-(HEIGHT2-HEIGHT)/2);
//            i.grabScreen((ofGetWidth()-WIDTH)/2,  (ofGetHeight()-HEIGHT)/2, WIDTH, HEIGHT);
//            //                i.grabScreen(0, 0, WIDTH, HEIGHT);
//            ofImage *iii = sortBrightXCenter(&i, parameterMap[sortXThresh]);
//            
//            srcFbo->begin();
//            ofPushMatrix();
//            ofTranslate(0, (HEIGHT2-HEIGHT)/2);
//            ofSetColor(ofColor::white);
//            // i.draw(0, 0, WIDTH, HEIGHT);
//            iii->draw(0, 0, WIDTH, HEIGHT);
//            ofPopMatrix();
//            srcFbo->end();
//            ofPopMatrix();
//            
//        }
//        if(parameterMap[sortYThresh]>BOOL_PARAM_EPSILON){ //to debug
//            ofPushMatrix();
//            srcFbo->draw(0,-(HEIGHT2-HEIGHT)/2);
//            i.grabScreen((ofGetWidth()-WIDTH)/2,  (ofGetHeight()-HEIGHT)/2, WIDTH, HEIGHT);
//            ofImage *iii = sortBrightYCenter(&i, parameterMap[sortYThresh]);
//            
//            srcFbo->begin();
//            ofPushMatrix();
//            ofTranslate(0, (HEIGHT2-HEIGHT)/2);
//            ofSetColor(ofColor::white);
//            iii->draw(0, 0, WIDTH, HEIGHT);
//            ofPopMatrix();
//            srcFbo->end();
//            ofPopMatrix();
//        }
//    }

    


    
    if(parameterMap[borderMask]>BOOL_PARAM_EPSILON){
        curFbo->begin();
//                maskShader.load("../shaders/dmask");
        maskShader.begin();
        maskShader.setUniform1f("thresh", 1-parameterMap[borderMask]);
        maskShader.setUniform1f("WIDTH", WIDTH);
        maskShader.setUniform1f("HEIGHT", HEIGHT);
        srcFbo->draw(0,0);
        maskShader.end();
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
    }
    
    //***** SPACE ****//

    
    if(parameterMap[omg3D2]>BOOL_PARAM_EPSILON){
        ofPushMatrix();
        curFbo->begin();
        ofBackground(0);
        curFbo->end();

        displayOmg3D2(srcFbo, curFbo, scenes[0]!=feedbackScene, parameterMap[omg3D2Speed],
                      parameterMap[multiFbos]>BOOL_PARAM_EPSILON, parameterMap[omg3D2Rotation], parameterMap[omg3D2Dist], parameterMap[omg3D2Symetry],
                      parameterMap[omg3D2FreeRotation], 0, 0);
        
        //here
//        tunnel(srcFbo, curFbo, scenes[0]!=feedbackScene, parameterMap[omg3D2Speed],
//                      parameterMap[multiFbos]>BOOL_PARAM_EPSILON, parameterMap[omg3D2Rotation], parameterMap[omg3D2Dist], parameterMap[omg3D2Symetry],
//                      parameterMap[omg3D2FreeRotation]);
        
        ofPopMatrix();
        swapFBOs(srcFbo, curFbo);
    }
    
    if( parameterMap[user5] > 0 ){
        tunnelMeshDistort(srcFbo, curFbo);
        swapFBOs(srcFbo, curFbo);
    }
    
//    if(parameterMap[omg3D2]>BOOL_PARAM_EPSILON){
//        ofPushMatrix();
//        displayOmg3D2(srcFbo, curFbo, scenes[0]!=feedbackScene, parameterMap[omg3D2Speed],
//                      parameterMap[multiFbos]>BOOL_PARAM_EPSILON, parameterMap[omg3D2Rotation], parameterMap[omg3D2Dist], parameterMap[omg3D2Symetry],
//                      parameterMap[omg3D2FreeRotation], 0, 1);
//        ofPopMatrix();
//        swapFBOs(srcFbo, curFbo);
//    }
    
    if(parameterMap[glow]>BOOL_PARAM_EPSILON){
        tempFbo2.begin(); //source
        srcFbo->draw(0,0);
        tempFbo2.end();
        
        doGlow(srcFbo, curFbo, &tempFbo, &tempFbo2, parameterMap[glow], parameterMap[glow], parameterMap[glowResolution]);
        ofEnableBlendMode(OF_BLENDMODE_SCREEN);
        curFbo->begin();
        ofBackground(0);
        tempFbo2.draw(0,0);
        srcFbo->draw(0,0);
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    }

    
    if(abs(parameterMap[invertCircle])>BOOL_PARAM_EPSILON|| parameterMap[_invert]>BOOL_PARAM_EPSILON){
        doInvert(srcFbo, curFbo);
        swapFBOs(srcFbo, curFbo);
    }
    
#ifndef NO_STROBE
    if(parameterMap[strobe]!=0 && trueFrameNum%2!=0){
        doStrobe(srcFbo, curFbo);
        swapFBOs(srcFbo, curFbo);
    }
#endif
    
    
    //un etage de omg3D peut etre pas mal en cas d'utilisation d'omg3D2Y
    
    if(parameterMap[randHHide]>BOOL_PARAM_EPSILON || parameterMap[randVHide]>BOOL_PARAM_EPSILON){
        randHide(srcFbo, curFbo);
        swapFBOs(srcFbo, curFbo);
    }
    if(parameterMap[kalei_2]>BOOL_PARAM_EPSILON){
        doKalei(srcFbo, curFbo, parameterMap[kaleiNb_2], parameterMap[kaleiOffX], parameterMap[kaleiOffY]);
        swapFBOs(srcFbo, curFbo);
    }

}

void eqkoscope::displayInfo(int w, int h){
    //    int freq = 1; //Audio debug
    //    float smooth = ofGetMouseX()/10;
    //    float b = 512*smooth;
    //    float coffset ;
    //    for(int i=0;i<512;i++){"
    //        coffset = int(coffset + freq) % (HEIGHT/2-1);
    //        float s = 0;
    //        for(int j=0;j<smooth;j++){
    //            int x = (WIDTH/2) + coffset*std::cos(2*PI*(j+i*512)/b);
    //            int y = (HEIGHT/2) + coffset*std::sin(2*PI*(j+i*512)/b);
    //            ofEllipse(x, y, 3, 3);
    //        }
    //    }
    
    ofPushMatrix();
    
    if(saveMacros && ofGetFrameNum()%2 == 0){
        ofSetColor(ofColor::red);
        ofRect(0,0,w,25);
        ofSetColor(ofColor::white);
    }
    
    if(savingGif && ofGetFrameNum()%2 == 0){
        ofSetColor(ofColor::blue);
        ofRect(0,0,w,25);
        ofSetColor(ofColor::white);
    }
    
    if(isPrompt && secondDisplay!=0 ){
        ofSetColor(ofColor::darkGray);
        ofRect(0,0,w,25);
        ofSetColor(ofColor::white);
    }

    
    if(isPrompt){
        ofSetColor(ofColor::magenta);
        stringstream str;
        string pstr = promptStr;
        if(quickPromptStr.compare(""))
            pstr = quickPromptStr;
        if(ofGetFrameNum()%10>5){
            str << "| " << pstr << " : " << promptValue;
            if(parameterNameMap.count(pstr)>0)
                str << " /// " << parameterMap[parameterNameMap[pstr]];
        }else
            str << "  " << pstr << " : " << promptValue;
        
        str << '\n' << '\n';
        
        for(map<string, int>::iterator it=parameterNameMap.begin();it!=parameterNameMap.end();it++){
            if(!strncmp(it->first.c_str(), pstr.c_str(), pstr.length())){
                str << it->first << " : " <<  parameterMap[it->second] <<  '\n';
            }
        }
        if(secondDisplay==0)
            ofDrawBitmapString(str.str(), w/3, h/4);
        else
            font2.drawString(str.str(), w/3, h/4);
        ofSetColor(ofColor::white);
    }
    
    if(saveMacros){
        ofSetColor(ofColor::green);
        string str;
        if(ofGetFrameNum()%10>5)
            str = "| "+saveMacroStr;
        else
            str = "  "+saveMacroStr;
        
        if(secondDisplay==0)
            ofDrawBitmapString(str, w/2, h/2);
        else
            font2.drawString(str, w/2, h/2);
        ofSetColor(ofColor::white);
    }
    
    if(displayFrameRate && !safeMode){
        ofSetColor(ofColor::magenta);
        stringstream str;
        str << "FPS " << ((int)(10*ofGetFrameRate()))/10.0 << endl;
        str << "Render 𝛕 " << drawMicros <<  " (" << ((int)(drawMicros/(1000000/restrictFrameRate)*100.0)) << "%)" << endl <<
        "Update 𝛕 " << updateMicros <<  " (" << ((int)(updateMicros/(1000000/restrictFrameRate)*100.0)) << "%)" << endl <<
        "Auto 𝛕 " << autoMicros <<  " (" << ((int)(autoMicros/(1000000/restrictFrameRate)*100.0)) << "%)" << endl <<
        "Total 𝛕 " << drawMicros+drawMicros <<  " (" << ((int)((drawMicros+updateMicros)/(1000000/restrictFrameRate)*100.0)) << "%)";
        str << endl << endl;
        for(int i=0;i<3;i++){
            if(scenes[i]!=0)
                str << scenes[i]->getInfo() << endl;
        }
        string macStr = currentMacroStr;
        macStr = macStr.replace(0, currentMacroStr.size()-25, "");
        str << "Crt. macro : []"<< macStr << endl;
        str << endl;
        str << "OSC "<< " port " << OSC_INPUT << " | statuts: "<< receiver.listen_socket->IsBound() << endl;

        if(secondDisplay==0)
        ofDrawBitmapString(str.str(), w/2, h/4);
        else
            font2.drawString(str.str(), w/2, h/4);
        ofSetColor(ofColor::white);
    }

    ofPopMatrix();
}

void eqkoscope::displayDualScenes(){
//    blendShader.load("../shaders/blend");
    
    ofSetColor(255);
    
    curFbo->begin();
    //ofTranslate(0, -(HEIGHT2-HEIGHT)/2);
    srcFbo->draw(0,0);
    blendShader.begin();
    blendShader.setUniform1i("mode", int(parameterMap[blendType]));
    if(scenes[1]!=0)
    scenes[1]->draw();
    blendShader.end();

        curFbo->end();
        swapFBOs(srcFbo,curFbo);
}

void eqkoscope::applyPostProcess(){
    ofSetColor(255);

    curFbo->begin();
    if(trapeze!=0){
        trapezeShader.begin();
        trapezeShader.setUniform1f("trapeze", trapeze);
        trapezeShader.setUniform1f("WIDTH", WIDTH);
        trapezeShader.setUniform1f("HEIGHT", HEIGHT);
    }
    srcFbo->draw(0, -(HEIGHT2-HEIGHT)/2);
    if(trapeze!=0)
        trapezeShader.end();
    applyMask();
    curFbo->end();
    swapFBOs(srcFbo, curFbo);
    srcFbo->draw(0, 0);
//        curFbo->draw(0, 0);
}

void eqkoscope::applyMask(){
    if(app->parameterMap[_mask]){
        ofPushMatrix();
        ofTranslate(WIDTH/2, HEIGHT2/2);
        if(app->parameterMap[_mask]==-1)
            interMask.draw(-WIDTH/2, -HEIGHT/2, WIDTH, HEIGHT);
        else
            if(app->parameterMap[_mask]==-2)
                mappingMask.draw(-WIDTH/2, -HEIGHT/2, WIDTH, HEIGHT);
            else{
                ofScale(1/app->parameterMap[_mask], 1/app->parameterMap[_mask]);
                roundMaskImg.draw(-WIDTH/2, -HEIGHT/2, WIDTH, HEIGHT);
        }
        ofPopMatrix();
    }
}

void eqkoscope::doPaint(){
    if(parameterMap[paint]){ //random calls are pretty fast
        srcFbo->readToPixels(pix); //does this gets optimized ?
        if(pix.getPixels()!=NULL){
            float r;
            float xx, yy;
            int fboIndex = ofGetFrameNum() % paintFbos.size();
            paintFbos[fboIndex]->begin();
            ofBackground(0, 0, 0); ///REMOVE ! BETA TEST !!
            srcFbo->draw(0,0);
//            paintPass3(60, 0.75, 4, &pix); //different passes for different sizes
            paintPass3(20/parameterMap[paint], 0.25, 0.5, &pix);
            
//            paintPass(150, 0.25, 0.75, &pix); //different passes for different sizes
//            paintPass(40, 0.1, 0.3, &pix);
            
            paintFbos[fboIndex]->end();
            
            ofSetColor(parameterMap[tintBrightness]*255);
            paintFbo.begin();
            paintFbos[fboIndex]->draw(0,0);
            paintFbo.end();
            paintFbo.draw(xOffset,yOffset);
        }
    }
    
    if(parameterMap[paint2]){
        srcFbo->readToPixels(pix);
        if(pix.getPixels()!=NULL){
            float r;
            float xx, yy;
            int resolution = 20;
            int fboIndex = ofGetFrameNum() % paintFbos.size();
            paintFbo.begin();
            
            paintPass2(100, 35, 75, 30, &pix);
            paintFbo.end();
            
            ofSetColor(parameterMap[tintBrightness]*255);
            paintFbo.draw(xOffset,yOffset);
        }
    }
}

void eqkoscope::paintPass(int resolution, float minSize, float maxSize, ofPixels* pix){
    float xx, yy, r;
    for(int x=resolution/2;x<WIDTH;x+=resolution)
        for(int y=resolution/2;y<HEIGHT;y+=resolution){
            xx = x;
            yy = y;
            xx += ofRandom(-resolution/2, resolution/2);
            yy += ofRandom(-resolution/2, resolution/2);
            ofColor c = pix->getColor(x, HEIGHT2-(y+(HEIGHT2-HEIGHT)/2));
            ofSetColor(c);
            r = ofRandom(360);
            ofPushMatrix();
            ofTranslate(xx,yy);
            ofRotate(r);
            int strokeIndex = (int)ofRandom(brushstrokes.size()-0.5);
            float sizeRatio = ofRandom(minSize, maxSize);
            brushstrokes[strokeIndex]->draw(-brushstrokes[strokeIndex]->width/2*sizeRatio, -brushstrokes[strokeIndex]->height/2*sizeRatio,
                                            brushstrokes[strokeIndex]->width*sizeRatio,
                                            brushstrokes[strokeIndex]->height*sizeRatio);
            ofPopMatrix();
        }
}

void eqkoscope::paintPass2(int resolution, float minSize, float maxSize, float randSize , ofPixels* pix){
    float xx, yy, r;
    for(int x=resolution/2;x<WIDTH;x+=resolution)
        for(int y=resolution/2;y<HEIGHT;y+=resolution){
            xx = x;
            yy = y;
            xx += ofRandom(-resolution/2, resolution/2);
            yy += ofRandom(-resolution/2, resolution/2);
            ofColor c = pix->getColor(x, HEIGHT2-(y+(HEIGHT2-HEIGHT)/2));
            c = ofColor(c.r ,c.g,c.b,200);
            ofSetColor(c);
            r = ofRandom(360);
            ofPushMatrix();
            ofTranslate(xx,yy);
            ofRotate(r);
            
            float N = (int) ofRandom(3,6);
            float S =  ofRandom(minSize, maxSize);
            
            ofBeginShape();
            for(int i=0;i<N;i++){
                float a = 2*M_PI * ((i/(N) + ofRandom(1/N)));
                float d = ofRandom(S-randSize, S+randSize);
                ofVertex(d*cos(a), d*sin(a));
            }
            ofEndShape();
            
            ofPopMatrix();
        }
}

/// TODO : imagine que les cloportes puissent se deplacer !
void eqkoscope::paintPass3(int resolution, float minSize, float maxSize, ofPixels* pix){
    float xx, yy, r;
    for(int x=resolution/2;x<WIDTH;x+=resolution)
        for(int y=resolution/2;y<HEIGHT;y+=resolution){
            xx = x;
            yy = y;
            xx += ofRandom(-resolution/2, resolution/2);
            yy += ofRandom(-resolution/2, resolution/2);
            ofColor c = pix->getColor(x, HEIGHT2-(y+(HEIGHT2-HEIGHT)/2));
            float b = c.getBrightness();
            bool pb = app->parameterMap[_invert] == 0;
            if((b>200 && pb) || (b<55 && !pb)){
                ofSetColor(c);
                r = ofRandom(360);
                ofPushMatrix();
                ofTranslate(xx,yy);
                r = ofNoise((ofGetFrameNum()*4 + xx)/500, yy/500) * 360;
                ofRotate(r);
                int strokeIndex = (int)ofRandom(brushstrokes.size()-0.5);
                float sizeRatio = ofRandom(minSize, maxSize);
                brushstrokes[strokeIndex]->draw(-brushstrokes[strokeIndex]->width/2*sizeRatio, -brushstrokes[strokeIndex]->height/2*sizeRatio,
                                                brushstrokes[strokeIndex]->width*sizeRatio,
                                                brushstrokes[strokeIndex]->height*sizeRatio);
                ofPopMatrix();
                y+=resolution;//eviter les embouteillages
            }
        }
}


void eqkoscope::mouseMoved(int x, int y){
    if(scenes[parameterMap[currentScene]]==drawscene)
        drawscene->mouseMoved(x,y);
    if(scenes[parameterMap[currentScene]]!=0)
        scenes[parameterMap[currentScene]]->mouseMoved(x,y);
    if(featuredParameter>=0)
        deltaMap[featuredParameter] = parameterMap[featuredParameter] = ofMap(ofGetMouseX(), 0, float(WIDTH), 0, 1);
    
    if(parameterMap[useJoyStick]){
//        deltaMap[omg3D2Speed] = (ofGetMouseY()/float(HEIGHT) - 0.5) * 0.02;
        float xx = (ofGetMouseX()/float(WIDTH) - 0.5)*3;
        float yy = -(ofGetMouseY()/float(HEIGHT) - 0.5)*3;
        float ease = 0.1;
        deltaMap[omg3D2RollerX] += (xx-deltaMap[omg3D2RollerX]) * ease;
        deltaMap[omg3D2RollerY] += (yy-deltaMap[omg3D2RollerY]) * ease;
    }
}
void eqkoscope::mouseDragged(int x, int y, int button){
    if(scenes[parameterMap[currentScene]]!=0)
        scenes[parameterMap[currentScene]]->mouseDragged(x,y,button);
    if(featuredParameter>=0)
//        deltaMap[featuredParameter] = parameterMap[featuredParameter] = ofMap(ofGetMouseX(), 0, float(WIDTH), 0, 1);
    deltaMap[featuredParameter] = ofMap(ofGetMouseX(), 0, float(WIDTH), 0, 1);
    //    parameterMap[warpX] = deltaMap[warpX] =ofGetMouseX()/(float)WIDTH  -0.5;
    //    parameterMap[warpY] = deltaMap[warpY] =ofGetMouseY()/(float)HEIGHT - 0.5;
    
    //LSD
    //    parameterMap[warp] = ofMap(ofGetMouseX(), 0, float(WIDTH), 0, 1);
    //    parameterMap[warpRemap] = ofMap(ofGetMouseX(), 0, float(WIDTH), 0, 1) > 0;
    //    parameterMap[tintSaturation] = ofMap(ofGetMouseX(), 0, float(WIDTH), 0, 1) * 3 + 1;
    //
    //    parameterMap[sharpen] = ofMap(ofGetMouseX(), 0, float(WIDTH), 0, 1) * 0.3;
    //    parameterMap[chromaSep] = ofMap(ofGetMouseX(), 0, float(WIDTH), 0, 1) * 0.015;
    //
    
//    ildaFrame.getLastPoly().lineTo(x / (float)ofGetWidth(), y / (float)ofGetHeight());
}
void eqkoscope::mousePressed(int x, int y, int button){
    if(scenes[parameterMap[currentScene]]!=0)
        scenes[parameterMap[currentScene]]->mousePressed(x,y,button);
}

void eqkoscope::mouseScrolled(int x, int y){
    
}

void eqkoscope::mouseReleased(int x, int y, int button){}
void eqkoscope::dragEvent(ofDragInfo dragInfo){
    vector<string> files = dragInfo.files;
    if(files.size()>0){
        ofFile f(files[0]);
        if(!f.getFileName().compare("macroMap.csv")){
            macroPath = f.getEnclosingDirectory();
            loadMacroMap();
            return;
        }
        string ext = ofToLower(f.getExtension());
        
        if(!ext.compare("csv")){
            controlFile = f.getFileName();
            if(controlFile.compare(""))
                loadMapping(ofBufferFromFile("parameterMappings/"+controlFile).getText(), false, true);
        }else{
            if(!ext.compare("xml")){
                loadMacro(f.getAbsolutePath());
            }else{
                if(!ext.compare("mp4") || !ext.compare("webm") || !ext.compare("mov")
                   || !ext.compare("avi")){
                    if(scenes[app->parameterMap[currentScene]]==feedbackScene){
                        feedbackScene->nestedScene = cinema;
                        cinema->loadDirectMovie(f.getFileName());
                        return;
                    }
                    if(cinemas.size()>0 && scenes[app->parameterMap[currentScene]]==cinemas[0])
                        cinemas[0]->loadDirectMovie(f.getFileName());
                    else{
                        if(scenes[app->parameterMap[currentScene]]!=cinema)
                            loadSoloScene(cinema);
                        cinema->loadDirectMovie(f.getFileName());
                    }
                }else{
                    if(!ext.compare("jpg") || !ext.compare("jpeg") || !ext.compare("gif")
                       || !ext.compare("png")){
                        if(scenes[app->parameterMap[currentScene]]==feedbackScene){
                            uzi->loadDirectImage(f.getAbsolutePath());
                            feedbackScene->nestedScene = uzi;
                            return;
                        }

                        if(uzis.size()>0 && scenes[app->parameterMap[currentScene]]==uzis[0]){
                            uzis[0]->loadDirectImage(f.getAbsolutePath());
                        }else{
                    if(scenes[app->parameterMap[currentScene]]!=uzi)
                        loadSoloScene(uzi);
                        uzi->loadDirectImage(f.getAbsolutePath());
                        }
                    }
                }
            }
        }
    }
}

void eqkoscope::touchMoved(ofTouchEventArgs &touch){
    //touch.pressure;
}

void eqkoscope::loadScene(Scene* s, int layer){
    sceneMutex.lock();
    if(layer<0 || layer>=scenes.size())
        return;
    if(scenes[layer]!=s && scenes[layer]!=0)
        scenes[layer]->exit();
    scenes[layer] = s;
    if(scenes[layer]!=0)
        scenes[layer]->setup();
    sceneMutex.unlock();
}

void eqkoscope::loadSoloScene(Scene* s){
    sceneMutex.lock(); //T1
    if(scenes[0]!=s && scenes[0]!=0)
        scenes[0]->exit();
    scenes[0] = s;
    scenes[0]->setup();
    for(int i=1;i<scenes.size();i++){
        if(scenes[i]!=s && scenes[i]!=0)
            scenes[i]->exit();
        scenes[i] = 0;
    }
    sceneMutex.unlock();
}

void eqkoscope::tapTempo(){
    if(lastTaps.size()>0 && lastTaps[lastTaps.size()-1]+5000 < ofGetSystemTime())
        lastTaps.clear();
    
    lastTaps.push_back(ofGetSystemTime());
    if(lastTaps.size()>=5){
        unsigned long long m = 0;
        for(int i=1;i<lastTaps.size();i++)
            m += (lastTaps[i]-lastTaps[i-1]);
        m /= (float)(lastTaps.size()-1);
        parameterMap[bpm] = deltaMap[bpm] =  60.0*1000.0 / (m);
    }
}

void eqkoscope::exit(){
#ifdef LEAPMOTION
    if(leap.isConnected())
        leap.close();
#endif
    
#ifdef KINECT
    if(kinectDevice.isConnected()){
        kinectDevice.close();
    }
#endif
    logfile.close();
    
    if(tcpServer.isConnected())
        tcpServer.close();
    
    if(is_launchpad)
        for(int i=0;i<128;i++)
            launchpadOut.sendNoteOn(1, i, 0);
}

void eqkoscope::setResolution(int r){
    switch(r){
        case 720:{
            crt_WIDTH = 1280;
            crt_HEIGHT = 720;
        }break;
        case 1080:{
            crt_WIDTH = 1900;
            crt_HEIGHT = 1080;
        }break;
        default:{
            crt_HEIGHT = r;
            crt_WIDTH = 16*r/9;;
        }
            break;
    }
    FINALWIDTH = crt_WIDTH;
    FINALHEIGHT = crt_HEIGHT;
    
    
    i.allocate(WIDTH,HEIGHT, OF_IMAGE_COLOR);
    fbo.allocate(WIDTH, HEIGHT2, GL_RGBA);
    fbo2.allocate(WIDTH, HEIGHT2, GL_RGBA);
    tempFbo.allocate(WIDTH, HEIGHT2, GL_RGBA);
    tempFbo2.allocate(WIDTH, HEIGHT2, GL_RGBA);
    copyFbo.allocate(WIDTH,HEIGHT);
    
    delete grabFbo;
    grabFbo = new ofFbo;
    grabFbo->allocate(WIDTH, HEIGHT2);
    
    for(int i=0;i<NB_PAINT_FRAMES;i++)
        delete paintFbos[i];
    paintFbos.clear();
    for(int i=0;i<NB_PAINT_FRAMES;i++){
        paintFbos.push_back(new ofFbo);
        paintFbos[i]->allocate(WIDTH, HEIGHT);
    }
    paintFbo.allocate(WIDTH, HEIGHT);
    
    for(int i=0;i<MAX_NB_ECHOES;i++)
        delete echoFbos[i];
    echoFbos.clear();
    for(int i=0;i<MAX_NB_ECHOES;i++){
        ofFbo* fbo = new ofFbo;
        fbo->allocate(WIDTH,HEIGHT2,GL_RGBA);
        echoFbos.push_back(fbo);
    }
    
    updateResolution();
    
//    ofSetupOpenGL(app->FINALWIDTH, app->FINALHEIGHT, OF_WINDOW);
    ofSetWindowShape(app->FINALWIDTH, app->FINALHEIGHT);
    
    if(app->dualDisplay)
        ofSetWindowPosition(-app->FINALWIDTH, 0);
    
    cinema->setResolution(r);
    uzi->setResolution(r);
    feedbackScene->setResolution(r);
    lines->setResolution(r);
}

void eqkoscope::niceRandom(int x){
    if(scenes[0]==cinema){
        cinema->randomJump();
    }
    
    deltaMap[skewAmp] = 0;
    deltaMap[_reTint] = 0;
    deltaMap[tintSaturation] = 1;
    deltaMap[displaceAmp] = 0;
    deltaMap[sobel]  = 0;
    deltaMap[hueShift] = 0;
    deltaMap[omg3D2] = 0;
    deltaMap[kalei] = 0;
    deltaMap[pert] = 0;
    deltaMap[nFreeze] = 0;
    
    switch(x){
        case 0:{ // tunnel
            deltaMap[omg3D2] = 1;
            deltaMap[omg3D2Dist] = ofRandom(0.5,1);
            deltaMap[omg3D2Rotation] = 45*(int) ofRandom(0,12);
            deltaMap[omg3D2HardRotation] = 1;
            deltaMap[kalei] = (int) ofRandom(0,2);
            deltaMap[kaleiNb] = (int) ofRandom(0,5);
            
        }break;
        case 1:{ //liquid
            //            deltaMap[_invert] = ofRandom(0, 1) < 0.5 ? 0 : 1;
//            deltaMap[tintSaturation] = (int) ofRandom(0, 2);
            deltaMap[sobel] = ofRandom(0, 1) * (int) ofRandom(0, 2);
            deltaMap[pert] = ofRandom(0, 1) * (int) ofRandom(0, 2);
            deltaMap[pertEvoAuto] = ofRandom(0, 0.2);
            deltaMap[pertFreq] = ofRandom(1, 10);
            deltaMap[skewAmp] = ofRandom(0, 1) * (int) ofRandom(0, 2);
            
            
           
        }break;
        case 2:{ //glitch
            deltaMap[tintHue] = ofRandom(0, 1);
            deltaMap[_reTint] = ofRandom(0, 1);
            //            deltaMap[_invert] = ofRandom(0, 1) < 0.5 ? 0 : 1;
            deltaMap[hueShift] = ofRandom(0, 1);
            deltaMap[hueShiftCrazy] = (int) ofRandom(0, 2);
            deltaMap[sobel] = ofRandom(0, 1) * (int) ofRandom(0, 2);
            deltaMap[pert] = ofRandom(0, 1) * (int) ofRandom(0, 2);
            deltaMap[displaceAmp] = ofRandom(0, 1) * (int) ofRandom(0, 2);
            deltaMap[displaceProba] = pow(ofRandom(0, 1), 3);
            deltaMap[nFreeze] = ofRandom(0, 6) * (int) ofRandom(0, 2);
            
        }break;
        default:;
    }
    //     for(int p=0;p<=N_PARAM;p++)
    //         deltaMap[p] = parameterMap[p];
}


void eqkoscope::reset(){
    audioAutos.clear();
    timedAutos.clear();
    leapAutos.clear();
    oscAutos.clear();
    midiCCAutos.clear();
    midiNoteonAutos.clear();
    
    initMidi();
    loadMacroMap();
    initOSC();
    
    initParameters();

    analyzeMacros();
    
    for(int i=0;i<N_PARAM;i++){
        deltaMap[i] = parameterMap[i];
    }
}



