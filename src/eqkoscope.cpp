#include "eqkoscope.h"
#include <GLFW/glfw3.h>

struct token_name {
    GLuint count;
    GLenum token;
    const char *name;
};

ofImage paintTexture;

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
    cout << endl << "∞∞∞                             ∞∞∞" << endl;
    cout << " ∞  WELCOME TO eqkoscope v0.1 !  ∞ " << endl;
    cout << "∞∞∞                             ∞∞∞" << endl << endl;

    
#ifndef VERBOSE
    std::streambuf * old = std::cerr.rdbuf(stderrBuffer.rdbuf()); //redirection of stderr
#endif
    
#ifdef OFFLINE_RENDER
    initOfflineMIDI();
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
    
    //print_limits();
    
    fxShader.load("../shaders/fx");
    trapezeShader.load("../shaders/trapeze");
    mappingmaskShader.load("../shaders/mappingmask");
    
    keyShader.load("../shaders/key");
    
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
    
    float date = 0;
    
    promptFont.loadFont(guiFontFamily, 22);
    promptFontSmall.loadFont(guiFontFamily, 24);
    
    if(!promptFont.isLoaded()){
        promptFont.loadFont("Arial", 22);
    }
    if(!promptFontSmall.isLoaded()){
        promptFontSmall.loadFont("Arial", 24);
    }

#ifdef VERBOSE
    cout << "dateA " << ofGetTimestampString() << endl;
#endif
    
    initMidi();
    loadMacroMap();
    
    initOSC();
#ifdef VERBOSE
    cout << "dateB " << ofGetTimestampString() << endl;
#endif
    
    utils = new Utils(this);
    
    freezer = new Freezer(this);
    
#ifdef VERBOSE
    cout << "dateC " << ofGetTimestampString() << endl;
#endif
    
    initGlitches(this);
#ifdef VERBOSE
    cout << "dateD " << ofGetTimestampString() << endl;
#endif
    
#ifdef OF_10
    paintTexture.loadImage("textures/baiser.jpg");
#else
    paintTexture.loadImage(("textures/paintTex.jpg"));
#endif
    paintTexture.resize(WIDTH, HEIGHT);
    
    
    /** SCENES **/
    for(int i=0;i<3;i++){
        scenes.push_back(0);
    }
#ifdef VERBOSE
    cout << "dateE " << ofGetTimestampString() << endl;
#endif
    feedbackScene = new Feedback(this); //0.
    //    pointback = new PointBack(this);
#ifdef VERBOSE
    cout << "dateE1 " << ofGetTimestampString() << endl;
#endif
    cinema = new Cinema(this);
#ifdef VERBOSE
    cout << "dateE11 " << ofGetTimestampString() << endl;
#endif
    uzi = new Uzi(this);
#ifdef VERBOSE
    cout << "dateE2 " << ofGetTimestampString() << endl;
#endif
    fractal = new Fractal(this);
    agents = new Agents(this);
    
    
//    for(int i=0;i<2;i++){
//        cinemas.push_back(new Cinema(this));
//        cinemas[i]->setSleep(i>0);
//        cinemas[i]->setup();
//    }
//    for(int i=0;i<1;i++){
//        uzis.push_back(new Uzi(this));
//    }
    
    drawscene = new Draw(this);
    print = new Print(this);
    lines = new Lines(this);
    glitch = new Glitch(this);
//    three = new Three(this);
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
//    allScenes.push_back(three);
    allScenes.push_back(mapped);
    
    scenes[0] = feedbackScene;


#ifdef MAPPING
    scenes[0] = mapped;
#endif
    scenes[0]->setup();
    
    analyzeMacros();
    
    i.allocate(WIDTH,HEIGHT, OF_IMAGE_COLOR);
    audioImg.allocate(audioCapture, audioCapture, OF_IMAGE_COLOR);
    
    /** END OF SCENES **/
    
    /** AUDIO **/
    initAudio();
    
    loadShaders();
    
    {
        coordShader.load("../shaders/coords");
        noiseCoordShader.load("../shaders/noisecoord");
        selSatShader.load("../shaders/saturationSelection");
        testShader.load("../shaders/test");
        orionShader.load("../shaders/orion");
        lcdShader.load("../shaders/lcd");
        tramesShader.load("../shaders/trames");
        tramesbwShader.load("../shaders/trames_bw");
        megaShader.load("../shaders/megaMask");
        //        noiseCoordFbo.allocate(WIDTH, HEIGHT2);
        //        noiseCoordFbo2.allocate(WIDTH, HEIGHT2);
    }
    
    fbo.allocate(render_WIDTH, render_HEIGHT, GL_RGBA);
    fbo2.allocate(render_WIDTH, render_HEIGHT, GL_RGBA);
    tempFbo.allocate(render_WIDTH, render_HEIGHT, GL_RGBA);
    tempFbo2.allocate(render_WIDTH, render_HEIGHT, GL_RGBA);
    psyFbo.allocate(WIDTH, HEIGHT2, GL_RGBA);
    
    backgroundFbo.allocate(WIDTH, HEIGHT2, GL_RGBA);
    
    copyFbo.allocate(WIDTH,HEIGHT);
    
    grabFbo = new ofFbo;
    grabFbo->allocate(WIDTH, HEIGHT2);
    captureFbo = new ofFbo;

    if(recordAndRenderFlag==0)
        logfile.open("./record.csv",ofFile::WriteOnly);
    else{
            if(recordAndRenderFlag==1)
                logfile.open("./record.csv",ofFile::ReadOnly);
    }
//    logfile.open("./log.txt",ofFile::WriteOnly);
    
    for(int i=0;i<255;i++)
        keyIsDown[i] = false;
    
    ofDirectory strokeDir("brushes");
    strokeDir.listDir();
    for(int i=0;i<strokeDir.size();i++){
        string picpath = strokeDir.getPath(i);
        if(picpath.find(".jpg")!=string::npos || picpath.find(".png")!=string::npos || picpath.find(".gif")!=string::npos||picpath.find(".JPG")!=string::npos || picpath.find(".PNG")!=string::npos || picpath.find(".GIF")!=string::npos
           || picpath.find(".tiff")!=string::npos){
            brushstrokes.push_back(new ofImage(picpath));
        }}
    
    
    int brushDownSize = 2;
    for(int i=0;i<brushstrokes.size();i++){
        brushstrokes[i]->resize(brushstrokes[i]->getWidth()/brushDownSize,brushstrokes[i]->getHeight()/brushDownSize);
    }
    
    for(int i=0;i<NB_PAINT_FRAMES;i++){
        paintFbos.push_back(new ofFbo);
        paintFbos[i]->allocate(WIDTH, HEIGHT);
    }
    paintFbo.allocate(WIDTH, HEIGHT);
    
    currentFrameRate = min(maxFrameRate,parameterMap[frameRate]);
    ofSetFrameRate(currentFrameRate);
    
    ofLogVerbose("verbose test");
    
    //test
    //    drawscene->embedScene = cinema;
    
    for(int i=0;i<MAX_NB_ECHOES;i++){
        ofFbo* fbo = new ofFbo;
        fbo->allocate(WIDTH,HEIGHT2,GL_RGBA);
        fbo->begin();
        ofBackground(0);
        fbo->end();
        echoFbos.push_back(fbo);
    }
    
    attachedSecondDisplay = dualDisplay && attachedSecondDisplay;
    if(dualDisplay){
//            ofSetWindowShape(FINALWIDTH, FINALHEIGHT); ///WHY ? OF v 10 ?
                    }else{
//                        mainDisplay = glfwGetCurrentContext();
//                        glfwWindowHint(GLFW_DECORATED, true);
//                        secondDisplay = glfwCreateWindow(720, 405, "Aux", NULL, mainDisplay);
//                        glfwSetWindowPos(secondDisplay,WIDTH-320,0);

    }
    
//    if(liveMode)
//        deltaMap[tintBrightness] = parameterMap[tintBrightness] = 0;
    
    
#ifdef OFFLINE_RENDER
    savingGif = true;
    gifIndex = int(ofRandom(1,100000));
#endif

    genetic = new Genetic(this);
#ifdef GENETIC
    randomParameters();
#endif
    
    //
    //    loadMacro("/Users/Raph/Dev/of_v0.8.4_osx_release/apps/myApps/Feedback/bin/data/Projects/Stienis/lightning.xml");
    
    // override the default codecs if you like
    // run 'ffmpeg -codecs' to find out what your implementation supports (or -formats on some older versions)
    //    for(int i = 0;i<numVidRec;i++){
    //        ofxVideoRecorder v;
    //        vidRecorder.push_back(v);
#ifdef VIDEO_EXPORT
    sampleRate = 44100;
    channels = 2;
    
    if(!hasTakenVideo){
        vidRecorder.setVideoCodec("h264");
        vidRecorder.setVideoBitrate("15000k");
        vidRecorder.setAudioCodec("mp3");
        vidRecorder.setAudioBitrate("192k");
        
        vidRecorder.setup("capture/eqkoCapture_"+ofToString(int(ofRandom(10000)))+fileExt, WIDTH, HEIGHT, 30, sampleRate, channels);
    }
#endif
    
    /*paramPanel = new ofxPanel;
    paramPanel->loadFont("Avenir", 10);
    paramPanel->setShape(0, 0, 150, 720);
	ofParameter<float> radius;
    paramPanel->setup("eqkoscope");
    paramPanel->setDefaultBackgroundColor(ofColor(0,0,0,0));
    paramPanel->setBackgroundColor(ofColor(0,0,0,0));
    
    newGUI = new EqkoGUI(this);
    for(int i = 0;i<parametersInGUI.size();i++){
        int id = parametersInGUI[i];
        PSlider* slider = new PSlider(this, id);
        slider->setup(parameterIDMap[id], deltaMap[id], parametersInGUIBounds[i][0], parametersInGUIBounds[i][1]);
        slider->setDefaultHeight(12);
        slider->setFillColor(guiColor);
        sliders.push_back(slider);
        paramPanel->add((sliders[i]));
        
        newGUI->addParam(id, parameterIDMap[id]);
    }
    
    //    guiCanvas = new ofxUISuperCanvas("eqkoscope");
    //    guiCanvas->setColorFill(guiColor);
    ////    guiCanvas->addSlider("Test",0.0,255.0,100.0);
    //    ofAddListener(guiCanvas->newGUIEvent, this, &eqkoscope::guiEvent);
    ////    guiCanvas->addRotarySlider("jhg", 0, 1, 0.4, 20);
    //    for(int i = 0;i<parametersInGUI.size();i++){
    //        int id = parametersInGUI[i];
    ////        ofxUISlider* s = new ofxUISlider(parametersInGUIBounds[i][0], parametersInGUIBounds[i][1], deltaMap[id], parameterIDMap[id]);
    //        guiCanvas->addSlider(parameterIDMap[id],parametersInGUIBounds[i][0], parametersInGUIBounds[i][1], deltaMap[id]);
    ////        guiCanvas->addRotarySlider(s);
    ////        s->ofxUIWidget::setVisible(true);
    ////        guiCanvas->addWidgetDown(s);
    
    
    //    }
    //    guiCanvas->autoSizeToFitWidgets();
    
    //    guiCanvas->setVisible(guiVisible);
    
    //    ofxColorSlider* color = new ofxColorSlider();
    //    color->setDefaultHeight(11);
    //    color->setup(ofColor::red);
    //    paramPanel->add(color);
    
    gui.push_back(paramPanel);
    
    //    ofxPanel* resolutionPanel = new ofxPanel;
    //    resolutionPanel->loadFont(guiFontFamily, 9);
    //    resolutionPanel->setShape(0, 0, 150, 720);
    
    //    for(int i=0;i<=3;i++){
    //        int res = 360;
    //        switch(i) {
    //            case 1:res = 720;break;
    //            case 2:res = 1080;break;
    //            case 3:res = 2160;break;
    //            default:;
    //        }
    //        PButton *btn = new PButton(this, res);
    //        btn->setup(ofToString(res));
    //        resBtns.push_back(btn);
    //        paramPanel->add((resBtns[i]));
    //    }
    
    
    //    }
    //    ofAddListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
    
    */
    
//#ifndef EXPORT
    if(!liveMode){
    if(ofFile("./lastMacro.csv").exists() && recordAndRenderFlag==-1)
        loadMacro("./lastMacro.csv");
    }else{
        uzi->loadDirectImage("assets/black.jpg");
        scenes[0] = uzi;
    }
//#endif
    
        feedbackFbo.allocate(app->render_WIDTH, app->render_HEIGHT, GL_RGBA);
        
    for(int i=0;i<nbPastFbos;i++){
        ofFbo* f = new ofFbo();
        f->allocate(WIDTH, HEIGHT);
        pastFbos.push_back(f);
    }
    
    for(int i=0;i<nbGranularFbos;i++){
        ofFbo* f = new ofFbo();
        f->allocate(ofRandom(granularFboSize_m, granularFboSize_M)
                    , ofRandom(granularFboSize_m, granularFboSize_M));
        granularFbos.push_back(f);
    }
    
    
    /// GURU INIT
    string guruQuotesRawStr = ofFile("guru/quotes.txt").readToBuffer().getText();
    string obliqueQuotesRawStr = ofFile("guru/oblique.txt").readToBuffer().getText();
    stringstream ssss;
    ssss << endl;
    guruQuotes = ofSplitString(guruQuotesRawStr, ssss.str());
    if(guruQuotes.size()<2){
        guruQuotes = ofSplitString(guruQuotesRawStr, "/r");
    }
    ssss.clear();
    obliqueQuotes = ofSplitString(obliqueQuotesRawStr, ssss.str());
    if(obliqueQuotes.size()<2){
        obliqueQuotes = ofSplitString(obliqueQuotesRawStr, "/r");
    }

    ofSetEscapeQuitsApp(false);
    
    cout << endl << "∞∞∞             ∞∞∞"<< endl;
    cout << " ∞  NOW RUNNING  ∞ "<< endl;
    cout << "∞∞∞             ∞∞∞"<< endl << endl;
}

void eqkoscope::swapFBOs(ofFbo* a, ofFbo* b){
    srcFbo = (srcFbo==&fbo) ? &fbo2 : &fbo;
    curFbo = (curFbo==&fbo) ? &fbo2 : &fbo;
}

ofPixels pixels;
void eqkoscope::draw(){
    
    if(!shadowFrame && shadowFrames>0){
        shadowFrame = true;
        for(int i = 0; i < shadowFrames ; i++){
            draw();
            update();
        }
        shadowFrame = false;
    }
        
    if(pause)
        return;

    localFrameNum ++;
    
    long date = ofGetElapsedTimeMicros();
    frameRateTurboCounter = 0;

    ofPushMatrix();
    {
        doGlitches = false;
        float tglitch = 1000/parameterMap[glitchFreq];
#ifdef REAL_TIME_FX
        tglitch /= maxFrameRate/ofGetFrameRate();
#endif
        if(parameterMap[glitchFreq]>=30 || (ofGetElapsedTimeMillis() - lastGlitchDate) > tglitch
           || ofRandom(ofGetElapsedTimeMillis() - lastGlitchDate) > tglitch){
            doGlitches = true;
            lastGlitchDate = ofGetElapsedTimeMillis();
        }
        
        ofTranslate((FINALWIDTH - WIDTH)/2, (FINALHEIGHT - HEIGHT)/2);
        
        if(strip!=0) /// LED STRIP
            strip->draw();
        
        srcFboMutex.lock();
        
        ofBackground(0);
        ofSetColor(ofColor::black);
        ofDrawRectangle(0,0,WIDTH,HEIGHT2);
        ofSetColor(ofColor::white);
        
        ofPushMatrix();
        
        grabFbo->begin();
        sceneMutex.lock();
        
        if(scenes.size()>0 && scenes[0]!=0){
            scenes[0]->draw();
        }
        
#ifdef KINECT
        //    if(kinectDevice.isFrameNew()){
        //        kinectDevice.draw(0, 0);
        //    }
#endif
        ofSetColor(255);
        sceneMutex.unlock();
        
        grabFbo->end();
        
#ifdef OF_10
        grabFbo->getTexture().getTextureData().bFlipTexture = false;//(scenes[0]==cinema || scenes[0]==feedbackScene);
#else
        grabFbo->getTextureReference().getTextureData().bFlipTexture = !(scenes[0]==cinema || scenes[0]==feedbackScene);
#endif

        ///GRANULAR
       /* if(app->parameterMap[user4]){
            for(int i=0;i<nbGranularFbos;i++){
                granularFbos[i]->begin();
                ofBackground(0);
                ofPushMatrix();
                ofTranslate(WIDTH/2, HEIGHT/2);
                ofRotate(ofRandom(360));
                grabFbo->draw(ofRandom(-0.5, 0.5)*(WIDTH-granularFbos[i]->getWidth()),
                              ofRandom(-0.5, 0.5)*(HEIGHT-granularFbos[i]->getHeight()));
                granularFbos[i]->end();
                ofPopMatrix();
            }
            
            grabFbo->begin();
            ofBackground(0);
            for(int i=0;i<nbGranularFbos;i++){
                ofPushMatrix();
                ofTranslate(ofRandom(0, WIDTH-granularFbos[i]->getWidth()),
                            ofRandom(0, HEIGHT-granularFbos[i]->getHeight()));
                maskShader.begin();
                maskShader.setUniform1f("thresh", 0.5);
                maskShader.setUniform1f("alpha", 1);
                maskShader.setUniform1i("blackAsVoid", 0);
                maskShader.setUniform1i("circleMode", 0);
                maskShader.setUniform1i("rectMode", 0);
                maskShader.setUniform1f("WIDTH", granularFbos[i]->getWidth());
                maskShader.setUniform1f("HEIGHT", granularFbos[i]->getHeight());
                granularFbos[i]->draw(0,0);
                maskShader.end();
                ofPopMatrix();
            }
            grabFbo->end();

        }*/
        
        if(pastFbos.size()>0){
//            if(ofGetFrameNum() % pastFbosPeriod == 0){
            pastFbos[pastFbosIndex]->begin();
            grabFbo->draw(0,0);
            pastFbos[pastFbosIndex]->end();
            pastFbosIndex = (pastFbosIndex+1) % nbPastFbos;
//            }
        }
        
        ofPopMatrix();
        
        fbo.begin();
        
        ofBackground(0);
        
        ofPushMatrix();
       
        if(parameterMap[tiles]) //TEST
            grabFbo->draw(0, 0, WIDTH, render_HEIGHT);
        else{
            if(abs(parameterMap[borderMask])>0.001 && render_WIDTH >= WIDTH){
                ofPushMatrix();
                if(!(parameterMap[omg3D2] || parameterMap[omg3D2X] || parameterMap[omg3D2Y]) && parameterMap[multiFbos])
                    ofTranslate((render_WIDTH - WIDTH)/2, (render_HEIGHT-HEIGHT)/2);
                maskShader.begin();
                maskShader.setUniform1f("thresh", abs(parameterMap[borderMask]));
                maskShader.setUniform1f("alpha", 1);
                maskShader.setUniform1i("circleMode", parameterMap[borderMask]<0);
                maskShader.setUniform1i("rectMode", 0);
                maskShader.setUniform1f("WIDTH", WIDTH);
                maskShader.setUniform1f("HEIGHT", HEIGHT);
                grabFbo->draw(0,0, WIDTH, HEIGHT);
                maskShader.end();
                ofPopMatrix();
            }else
                grabFbo->draw((render_WIDTH - WIDTH)/2, (render_HEIGHT-HEIGHT)/2, WIDTH, HEIGHT);
        }
        ofPopMatrix();
        

        
        fbo.end();


        if (parameterMap[post_traitement]){
//            ofSetColor(abs(parameterMap[tintBrightness])*255, abs(parameterMap[tintBrightness])*255, abs(parameterMap[tintBrightness])*255);
            ofPushMatrix();
            sceneMutex.lock();
            if(scenes[0]!=0){
                scenes[0]->mask();
                scenes[0]->capture(grabFbo);
            }
            sceneMutex.unlock();
            ofPopMatrix();
        }
        
        /** FX **/
        curFbo = &fbo2;
        srcFbo = &fbo;
        
//        frameRateTurboCounter = ofGetElapsedTimeMicros()-date;
//        if(frameRateTurboCounter < 1000000/maxFrameRate){
        
            ofPushMatrix();
            applyFXChain(srcFbo, curFbo);
            ofPopMatrix();
            
//        }
       
        if(parameterMap[background]){
            backgroundFbo.begin();
            ofPushMatrix();
            if(parameterMap[background]<1){
                ofColor c = ofColor::fromHsb(parameterMap[background]*255, 255, 255);
                ofBackground(c);
            }else{
                ofBackground(255,0,0);
                cinemas[1]->draw();
            }
            ofPopMatrix();
            backgroundFbo.end();
            
            curFbo->begin();
            backgroundFbo.draw(0,0);
            blendShader.begin();
            blendShader.setUniform1i("mode", (int)parameterMap[blendType]);
            blendShader.setUniformTexture("tex1", backgroundFbo.getTextureReference(), 1);
            srcFbo->draw(0,0);
            blendShader.end();
            curFbo->end();
            swapFBOs(srcFbo, curFbo);
        }
        
    
        
        /** GLOBAL PARAMETERS **/
        
//        saturationShader.load("../shaders/saturation");
        saturationShader.begin();
        saturationShader.setUniform1f("saturation", parameterMap[tintSaturation]);
        saturationShader.setUniform1f("hue", 0);

//        if(parameterMap[tintBrightness]<0){
//            saturationShader.setUniform1f("_floor", -(parameterMap[tintBrightness]+1));
//            saturationShader.setUniform1f("value", 1);
//        }else{
            saturationShader.setUniform1f("_floor", 0);
            saturationShader.setUniform1f("value", parameterMap[tintBrightness]);
//        }
        curFbo->begin();
        ofBackground(0);
        srcFbo->draw(0,0);
        curFbo->end();
        saturationShader.end();
        swapFBOs(srcFbo, curFbo);
        
        
        if (!(parameterMap[post_traitement]>BOOL_PARAM_EPSILON)){
            //            ofSetColor(abs(parameterMap[tintBrightness])*255);
            ofPushMatrix();
            sceneMutex.lock();
            scenes[0]->capture(srcFbo); //2
            scenes[0]->mask(); //1
            sceneMutex.unlock();
            ofPopMatrix();
        }
        
        
//        if(parameterMap[tintBrightness]>=0)
//            ofSetColor(parameterMap[tintBrightness]*255);
        
        if(parameterMap[flash])
            ofSetColor(ofRandom(255-parameterMap[flash]));
        else
            ofSetColor(ofColor::white);
        
        curFbo->begin();
        if(parameterMap[antiAliasing]>BOOL_PARAM_EPSILON){
            //        fxShader.load("../shaders/fx");
            fxShader.begin();
            fxShader.setUniform1f("alpha", abs(parameterMap[tintBrightness]));
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
        
        ofBackground(0);
        
        displayDualScenes();
        
        applyPostProcess();
        
        srcFbo->draw(0, 0, WIDTH, HEIGHT);
        
        if(dualDisplay && addMirorDisplay)
//            srcFbo->draw(WIDTH, 0, WIDTH, HEIGHT);
        srcFbo->draw(WIDTH*2, 0, -WIDTH, HEIGHT);
        
        
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



///COMPOSITING

        /*if(app->parameterMap[user1]>0){
       //            curFbo->begin();

                   for(int i=0;i<app->parameterMap[user1];i++){
                       megaShader.begin();
                       megaShader.setUniform1f("n", app->parameterMap[user1]);
                       megaShader.setUniform1f("offset", i/app->parameterMap[user1]);
                       megaShader.setUniform1f("evo", ofGetFrameNum());
                       megaShader.setUniform1f("WIDTH", WIDTH);
                       megaShader.setUniform1f("HEIGHT", HEIGHT);
                       //            megaPlayers[i].draw(-WIDTH/2,-HEIGHT/2);
                       //            zsFbos[i]->draw(0,0);

                       int index = (currentEcho - (i+1)*int(parameterMap[echoPeriod]/1000.0 * ofGetFrameRate()) + MAX_NB_ECHOES*100) % MAX_NB_ECHOES;
                       echoFbos[index]->draw(0,0);

                       megaShader.end();
                   }
       //            curFbo->end();
       //            swapFBOs(srcFbo, curFbo); //seems to be terminal...
               }*/

        
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
        ofDrawRectangle(0, -(HEIGHT2-HEIGHT)/2, WIDTH, 0);
        ofDrawRectangle(0, HEIGHT, WIDTH, HEIGHT2+1);
        ofSetColor(ofColor::white);
        
        if(parameterMap[audio]){
            audioImg.grabScreen(WIDTH/2-audioCapture/2, HEIGHT/2 - audioCapture/2, audioCapture, audioCapture);
        }
        
    }
    ofPopMatrix();
    
//    displayInfo(FINALWIDTH, FINALHEIGHT); //live coding mode
    
    if(analyzeAudioB || savingGif){
        i.grabScreen(0,0,WIDTH,HEIGHT);
//        srcFbo->readToPixels(pixels);
//        pixels.mirror(1, 0);
       
        if(analyzeAudioB)
            analyzeAudio();
        if(savingGif){
//            i.resize(GIF_WIDTH, GIF_HEIGHT);
//            i.update();
            
#ifdef VIDEO_EXPORT
            if(!hasTakenVideo){
                bool success = vidRecorder.addFrame(i.getPixelsRef());
                if (!success)
                    ofLogWarning("This frame was not added!");
                //                }
                if (vidRecorder.hasVideoError())
                    ofLogWarning("The video recorder failed to write some frames!");
                if (vidRecorder.hasAudioError())
                    ofLogWarning("The video recorder failed to write some audio samples!");
            }else
#endif
            {
                ofSaveImage(i, "capture/gif_"+ofToString(gifIndex)+"/"+ofToString(ofGetFrameNum())+".jpg", OF_IMAGE_QUALITY_BEST);
            }
           }
    }
    
    
    if(saveFrame){
        stringstream str;
        str << "./capture/" << int(ofRandom(1,100000));

        int capture_x=0,capture_y=0,capture_w=render_WIDTH,capture_h=render_HEIGHT;
        if(capture_w > capture_limit)
            capture_w = capture_limit;
        if(capture_h > capture_limit)
            capture_h = capture_limit;
        int count = 1;

        do{
            do{
                stringstream strext;
#ifdef OF_10
                strext << str.str() << count++ << ".jpg";
#else
                strext << str.str() << count++ << ".png";
#endif
//                if(parameterMap[echoNb]>0 || parameterMap[paint]>0){
//                    if(!captureImg.isAllocated()){
//                        captureImg.allocate(min(WIDTH,capture_limit),min(HEIGHT,capture_limit), OF_IMAGE_COLOR);
//                    }
//                    captureImg.grabScreen(capture_x, capture_y , capture_w, capture_h);
//                    captureImg.saveImage(strext.str(), ofImageQualityType::OF_IMAGE_QUALITY_BEST);
//                }else{
                    if(!captureFbo->isAllocated()){
                        captureFbo->allocate(min(WIDTH,capture_limit),min(HEIGHT,capture_limit));
                    }
                    if(!pixels.isAllocated() || pixels.getWidth()>capture_limit || pixels.getHeight()>capture_limit){
                        pixels.allocate(min(WIDTH,capture_limit),min(HEIGHT,capture_limit), OF_IMAGE_COLOR);
                    }
                    captureFbo->begin();
                    srcFbo->draw(-capture_x, -capture_y);
                    captureFbo->end();

                    captureFbo->readToPixels(pixels);
                    pixels.mirror(1, 0);
                ofSaveImage(pixels, strext.str(), OF_IMAGE_QUALITY_BEST);
//                }
                capture_x += capture_w;
            } while(capture_x<render_WIDTH);
            capture_y += capture_h;
        } while(capture_y<render_HEIGHT);

        if(currentMacroStr.compare("")){
            string p = currentMacroStr;
            ofStringReplace(p, ".xml", ".jpg");
//            if(parameterMap[echoNb]>0 || parameterMap[paint]>0)
//                captureImg.saveImage(p);
//            else{
                ofSaveImage(pixels, p);
//            }
        }
        saveFrame = false;
    }
    
    drawMicros += microsEasing*(ofGetElapsedTimeMicros() - date - drawMicros);;
    
    if(attachedSecondDisplay){
        ofPushMatrix();
        int x = FINALWIDTH;
        if(dualDisplay && addMirorDisplay)
            x += FINALWIDTH;
            ofTranslate(x, 0);
        ofScale(attachedSecondDisplayWidth/FINALWIDTH, attachedSecondDisplayWidth/FINALWIDTH);
        srcFbo->draw(0,0);
        ofPopMatrix();
    }else
//    if(secondDisplay!=0){
//        glfwMakeContextCurrent(secondDisplay);
//        int width, height;
//        glfwGetFramebufferSize(secondDisplay, &width, &height);
//        glViewport(0, 0, width, height);
//        glMatrixMode(GL_PROJECTION);
//        glLoadIdentity();
//        glOrtho(0, width, height, 0, -1, 1);
//        glMatrixMode(GL_MODELVIEW);
//        glLoadIdentity();
//        ofBackground(0, 0, 0);
//        srcFbo->draw(0,0);
//    }
    
    ofPushMatrix();
    if(attachedSecondDisplay){
        int x = FINALWIDTH;
        if(dualDisplay && addMirorDisplay)
            x += FINALWIDTH;
        ofTranslate(x, 0);
        ofScale(attachedSecondDisplayWidth/FINALWIDTH, attachedSecondDisplayWidth/FINALWIDTH);
    }
    displayInfo(FINALWIDTH, FINALHEIGHT);
    ofPopMatrix();

    ofPopMatrix();
    
    if(!guiVisible){
        ofHideCursor();
    }else{
        ofShowCursor();
        
        //    ofPushMatrix();
        
        //    for(ofxPanel* panel : gui){
        //        panel->draw();
        //    }
        
        //        ofPopMatrix();
        
#ifdef NEW_GUI
        if(newGUI!=NULL)
        newGUI->draw();
#endif
    }
    
//    if(secondDisplay!=0){
//        glfwSwapBuffers(secondDisplay);
//        glfwPollEvents();
//        glfwMakeContextCurrent(mainDisplay);
//    }
    
    sceneMutex.unlock();
}

ofBuffer tobeglitchedBuffer;
ofBuffer glitchedBuffer;
string ext_jpg = ".jpg";
string ext_bmp = ".bpm";
string ext_png = ".png";
string ext_gif = ".gif";
string ext_targa = ".targa";
void eqkoscope::applyFXChain(ofFbo* a, ofFbo* b){
    
    long date = ofGetElapsedTimeMicros();
    
    if(parameterMap[tiles])
    {
        doTiles(srcFbo, curFbo);
        swapFBOs(srcFbo, curFbo);
    }

//    if (parameterMap[omg3D2]>BOOL_PARAM_EPSILON){
//        if(parameterMap[multiFbos] && !parameterMap[post_traitement]){
//            ofPushMatrix();
//            curFbo->begin();
//            ofBackground(parameterMap[_invert]  ? 255 : 0);
//            curFbo->end();
//            
//            displayOmg3D2(srcFbo, curFbo, parameterMap[omg3D2Speed],
//                          parameterMap[multiFbos]>BOOL_PARAM_EPSILON, parameterMap[omg3D2Rotation], parameterMap[omg3D2Dist], parameterMap[omg3D2Symetry], 0, 0);
//            
//            ofPopMatrix();
//            swapFBOs(srcFbo, curFbo);
//            ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//        }
//    }
    
    if(parameterMap[post_traitement]==0)
        doOmg3D2();
    
    if(app->parameterMap[jpgGlitch]){
        string ext = ext_jpg;
//        ofImageFormat format = (ofImageFormat) app->parameterMap[user1];
        ofImageFormat format = OF_IMAGE_FORMAT_JPEG;
        if(app->parameterMap[jpgGlitch]<0)
            format = OF_IMAGE_FORMAT_PNG;

if(format==OF_IMAGE_FORMAT_PNG)
     ext = ext_png;
else if(format==OF_IMAGE_FORMAT_BMP)
    ext = ext_bmp;
else if(format==OF_IMAGE_FORMAT_GIF)
    ext = ext_gif;
else if(format==OF_IMAGE_FORMAT_TARGA)
    ext = ext_targa;
        string source = "./tmp/tobeglitchd"+ext;
        string path = "./tmp/glitchd"+ext;
        
        
        if(ofGetFrameNum()%2==0){ //capture // 150 msec, 27 msec at 0.51 resolution ratio
            
            srcFbo->readToPixels(pixels);
            double scale = 1;
            scale = 0.51;
//            scale = 0.66;
            scale = min(1.0, scale);
            pixels.resize(scale*srcFbo->getWidth(), scale*srcFbo->getHeight());
            ofSaveImage(pixels, tobeglitchedBuffer, format);
            //2 ; jpg
            //18: tiff
            //30 : jp
            //31: jp2
//            DEBUG_TIME_2
        }
        
//        glitchImage(path, source); // 1msec
        
//        DEBUG_TIME_1
        glitchedBuffer = tobeglitchedBuffer;

        if(glitchedBuffer.size()>0){

        glitchBuffer(&glitchedBuffer);

        ofLoadImage(pixels, glitchedBuffer);
        
        pixels.mirror(1, 0);
        pixels.resize(WIDTH, HEIGHT);

        i.setFromPixels(pixels);
//        DEBUG_TIME_2

//        curFbo->
//        i.loadImage(path); //20 msec
        
        curFbo->begin();
        i.draw(0,0);
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
    }
        }
    
    
    if(parameterMap[psyShift]){ //save the original content
        psyFbo.begin();
        srcFbo->draw(0,0);
        psyFbo.end();
    }
    
    if(parameterMap[keying]>0){
//LOAD_ASYNC        keyShader.load("../shaders/key");
        curFbo->begin();
        keyShader.begin();
        keyShader.setUniform1f("hue", parameterMap[keying]);
        srcFbo->draw(0,0);
        keyShader.end();
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
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
        
        if(abs(parameterMap[noise])>BOOL_PARAM_EPSILON ){
            doNoise(srcFbo, curFbo);
            swapFBOs(srcFbo, curFbo);
        }
        
        if(parameterMap[kalei]>BOOL_PARAM_EPSILON){
            doKalei(srcFbo, curFbo, parameterMap[kalei], parameterMap[kaleiNb], parameterMap[kaleiOffX], parameterMap[kaleiOffY]);
            swapFBOs(srcFbo, curFbo);
        }

        if(parameterMap[otherWorlds]>0){
                  doKalei(srcFbo, &tempFbo, 1, parameterMap[kaleiNb], 0, 0);
            if(parameterMap[sobel]==0)
                  sobelContours(&tempFbo, &tempFbo2, 1, 0, 0);

                  curFbo->begin();

                blendShader.begin(); // SCREEN
            blendShader.setUniform1i("mode", 1);
            blendShader.setUniform1f("_mix", app->parameterMap[otherWorlds]);
                  blendShader.setUniformTexture("tex1", srcFbo->getTextureReference(), 1);

            if(parameterMap[sobel]==0)
                  tempFbo2.draw(0, 0, render_WIDTH, render_HEIGHT);
            else
                tempFbo.draw(0, 0, render_WIDTH, render_HEIGHT);

                  blendShader.end();

                  curFbo->end();

                  swapFBOs(srcFbo, curFbo);
              }
        
//        if(app->parameterMap[user1]){
////            LOAD_ASYNC        orionShader.load("../shaders/orion");
//            curFbo->begin();
//            orionShader.begin();
//            orionShader.setUniform1f("intensity", app->parameterMap[user1]);
//            orionShader.setUniform1f("WIDTH", app->render_WIDTH);
//            orionShader.setUniform1f("HEIGHT", app->render_HEIGHT);
//            srcFbo->draw(0,0);
//            orionShader.end();
//            curFbo->end();
//            swapFBOs(srcFbo, curFbo);
//        }
        
        if(app->parameterMap[lcd]){
//            LOAD_ASYNC        lcdShader.load("../shaders/lcd");
            curFbo->begin();
            lcdShader.begin();
            if(app->parameterMap[lcd]>0){
                lcdShader.setUniform1f("intensity", app->parameterMap[lcd]);
                lcdShader.setUniform1f("offset", 0);
            }
//            else{
//                lcdShader.setUniform1f("intensity", 1);
//                lcdShader.setUniform1f("offset", -app->parameterMap[lcd]);
//            }
            srcFbo->draw(0,0);
            lcdShader.end();
            curFbo->end();
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
            
            if(abs(parameterMap[selSat]) > BOOL_PARAM_EPSILON){
                //                selSatShader.load("../shaders/saturationSelection");
                curFbo->begin();
                selSatShader.begin();
                selSatShader.setUniform1f("thresh", -parameterMap[selSat]);
                srcFbo->draw(0,0);
                selSatShader.end();
                curFbo->end();
                swapFBOs(srcFbo, curFbo);
            }
        }
        

        if(parameterMap[skewAmp]>0){
            skew(srcFbo, curFbo, parameterMap[skewAmp]/10.0, ofGetFrameNum()*3, 0);
            swapFBOs(srcFbo, curFbo);
        }
        if(parameterMap[skewVAmp]>0){
            skew(srcFbo, curFbo, parameterMap[skewVAmp]/3.0, ofGetFrameNum()*3, 1);
            swapFBOs(srcFbo, curFbo);
        }
        if(parameterMap[skewAAmp]>0){
            skew(srcFbo, curFbo, parameterMap[skewAAmp]/3.0, ofGetFrameNum()*3, 3);
            swapFBOs(srcFbo, curFbo);
        }
        if(parameterMap[skewDAmp]>0){
            skew(srcFbo, curFbo, parameterMap[skewDAmp]/3.0, ofGetFrameNum()*3, 2);
            swapFBOs(srcFbo, curFbo);
        }
    }
    
    //memory leak
    if(parameterMap[sortXThresh]>0 || parameterMap[sortYThresh]>0){
        if(parameterMap[sortXThresh]>BOOL_PARAM_EPSILON){
            ofPushMatrix();
            
            srcFbo->readToPixels(pixels);
            pixels.mirror(1,0);
            ofImage *iii = sortBrightXCenter(&pixels, parameterMap[sortXThresh]);
            //
            srcFbo->begin();
            ofPushMatrix();
            ofTranslate(0, (HEIGHT2-HEIGHT)/2);
            ofSetColor(ofColor::white);
            iii->draw(0, 0, render_WIDTH, render_HEIGHT);
            ofPopMatrix();
            srcFbo->end();
            ofPopMatrix();
            //
        }
        if(parameterMap[sortYThresh]>BOOL_PARAM_EPSILON){ //to debug
            ofPushMatrix();
            srcFbo->readToPixels(pixels);
            pixels.mirror(1,0);
            ofImage *iii = sortBrightYCenter(&pixels, parameterMap[sortYThresh]);
            srcFbo->begin();
            ofPushMatrix();
            ofTranslate(0, (HEIGHT2-HEIGHT)/2);
            ofSetColor(ofColor::white);
            iii->draw(0, 0, render_WIDTH, render_HEIGHT);
            ofPopMatrix();
            srcFbo->end();
            ofPopMatrix();
        }
    }
    
    
    if(abs(parameterMap[pert])>0.001){
        doPerturbation(srcFbo, curFbo);
        swapFBOs(srcFbo, curFbo);
    }
    
    if(parameterMap[pnoise]>0.001){
        doPNoise(srcFbo, curFbo);
        swapFBOs(srcFbo, curFbo);
    }
    
    if(parameterMap[typhoon]){
        //        coordShader.load("../shaders/coords");
        //        noiseCoordShader.load("../shaders/noisecoord");
        
        //        ofFbo * ncF1, *ncF2;
        //        ncF1 = ofGetFrameNum()%2==0 ? &noiseCoordFbo : &noiseCoordFbo2;
        //        ncF2 = ofGetFrameNum()%2==0 ? &noiseCoordFbo2 : &noiseCoordFbo;
        //        ncF2->begin();
        //        noiseCoordShader.begin();
        //        noiseCoordShader.setUniform1f("init", ofGetFrameNum()%100==0 ? 1 : 0);
        //        noiseCoordShader.setUniform1f("evolution", deltaMap[pertEvo]);
        //        ncF1->draw(0,0);
        //
        //        for(int i=0;i<noiseAgents.size();i++)
        //        {
        //            ofRect(noiseAgents[i].x, noiseAgents[i].y, 1, 1);
        //        }
        //        noiseCoordShader.end();
        //        ncF2->end();
        
        //        curFbo->begin();
        //        ofBackground(0);
        //        ncF2->draw(0,0);
        
        
        //        coordShader.begin();
        //        coordShader.setUniform1f("WIDTH", WIDTH);
        //        coordShader.setUniform1f("HEIGHT", HEIGHT);
        //        coordShader.setUniformTexture("coordinates", *ncF2, 1);
        //        srcFbo->draw(0,0);
        //        coordShader.end();
        //        curFbo->end();
        //        swapFBOs(srcFbo, curFbo);
        
//        if(ofGetFrameNum()%30==0) noiseCoordShader.load("../shaders/noisecoord");
        
        curFbo->begin();
        noiseCoordShader.begin();
        noiseCoordShader.setUniform1f("WIDTH", WIDTH);
        noiseCoordShader.setUniform1f("HEIGHT", HEIGHT);
        noiseCoordShader.setUniform1f("intensity", parameterMap[typhoon]);
        noiseCoordShader.setUniform1f("freq", parameterMap[pertFreq]);
        noiseCoordShader.setUniform1f("evolution", parameterMap[pertEvo]);
        srcFbo->draw(0,0);
        noiseCoordShader.end();
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
    }
    
//    if(parameterMap[test]>0){
//LOAD_ASYNC testShader.load("../shaders/test");
//
//        curFbo->begin();
//    testShader.begin();
//    testShader.setUniform1f("nb", ofGetMouseY()/float(HEIGHT)*25);
//    testShader.setUniform1f("dist", ofGetMouseX()/float(WIDTH)*10);
//        srcFbo->draw(0,0);
//        testShader.end();
//        curFbo->end();
//        swapFBOs(srcFbo, curFbo);
//    }
    
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
    
    if(abs(parameterMap[toCircle]) > BOOL_PARAM_EPSILON){
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
        blur(srcFbo, curFbo, parameterMap[vblur], parameterMap[blurOriginalMix], true, parameterMap[tiltShift]);
        swapFBOs(srcFbo, curFbo);
    }
    
    if(parameterMap[hblur]>BOOL_PARAM_EPSILON){
        blur(srcFbo, curFbo, parameterMap[hblur], parameterMap[blurOriginalMix], false, parameterMap[tiltShift]);
        swapFBOs(srcFbo, curFbo);
    }
    
    if(abs(parameterMap[_gamma]-1)>BOOL_PARAM_EPSILON || abs(parameterMap[contrast])>BOOL_PARAM_EPSILON){
        doGamma(srcFbo, curFbo, parameterMap[_gamma]);
        swapFBOs(srcFbo, curFbo);
    }
    
    
    
    bool doInvertion = false;
    if(abs(parameterMap[invertCircle])>BOOL_PARAM_EPSILON|| (!parameterMap[strobe] && parameterMap[_invert]>BOOL_PARAM_EPSILON)
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
//      if(ofGetFrameNum()%25==0) bwShader.load("../shaders/b&w");
            curFbo->begin();
            bwShader.begin();
            if(abs(parameterMap[bw])<1){
                bwShader.setUniform1f("_mix", abs(parameterMap[bw]));

            }else{
                bwShader.setUniform1f("_mix", 1);
            }
            bwShader.setUniform1f("steps", parameterMap[bw]);
            bwShader.setUniform1f("offset", parameterMap[bwOffset]);
            bwShader.setUniform1f("forceSaturation", parameterMap[reSaturate]);
            srcFbo->draw(0,0);
            bwShader.end();
            curFbo->end();
            swapFBOs(srcFbo, curFbo);
        }
    
    if(app->parameterMap[illu]){
        curFbo->begin();
        illuShader.begin();
        illuShader.setUniform1f("gradient", parameterMap[gradient]);
        illuShader.setUniform1f("levels", parameterMap[illu]);
        srcFbo->draw(0,0);
        illuShader.end();
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
    }
    
    if(parameterMap[tintHue]!=0){
    saturationShader.begin();
    saturationShader.setUniform1f("saturation", 1);
    saturationShader.setUniform1f("hue", parameterMap[tintHue]);
    saturationShader.setUniform1f("_floor", 0);
    saturationShader.setUniform1f("value", 1);
    curFbo->begin();
    srcFbo->draw(0,0);
    curFbo->end();
    saturationShader.end();
    swapFBOs(srcFbo, curFbo);
    }

    
    if(parameterMap[paint]==0){
        if(1==1){ //saturation mgmt
        colorGrading(srcFbo, curFbo, doInvertion, parameterMap[tintAmp], parameterMap[_reTint], parameterMap[sidesSaturation]);
        swapFBOs(srcFbo, curFbo);
    }
    }
    
    if(parameterMap[squareMandala]>BOOL_PARAM_EPSILON){
        curFbo->begin();
        //      if(ofGetFrameNum()%30==0)
        //                    squareMandalaShader.load("../shaders/squareMandala");
        squareMandalaShader.begin();
        squareMandalaShader.setUniform1f("WIDTH", WIDTH);
        squareMandalaShader.setUniform1f("HEIGHT", HEIGHT2);
        squareMandalaShader.setUniform1f("t", ofGetFrameNum() * app->parameterMap[speed]);
        squareMandalaShader.setUniform1f("resolution", parameterMap[squareMandala]);
        squareMandalaShader.setUniform1f("_border", parameterMap[mandalaBorder]);
        srcFbo->draw(0,0);
        squareMandalaShader.end();
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
    }
    
    if(parameterMap[mandala]>BOOL_PARAM_EPSILON){
        curFbo->begin();
//        if(ofGetFrameNum()%25==0) mandalaShader.load("../shaders/mandala");
        mandalaShader.begin();
        mandalaShader.setUniform1f("WIDTH", WIDTH);
        mandalaShader.setUniform1f("HEIGHT", HEIGHT2);
        mandalaShader.setUniform1f("t", ofGetFrameNum() * app->parameterMap[speed]);
        mandalaShader.setUniform1f("resolution", parameterMap[mandala]);
        mandalaShader.setUniform1f("_border", parameterMap[mandalaBorder]);
        mandalaShader.setUniform1f("keepBackground", 0);
        srcFbo->draw(0,0);
        mandalaShader.end();
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
    }
    
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
    
        //LOAD_ASYNC
#ifndef EXPORT
    if(abs(parameterMap[engraving])>BOOL_PARAM_EPSILON){
        ofShader* t_shader = parameterMap[engraving] > 0 ? &tramesShader : &tramesbwShader;
//        if(ofGetFrameNum()%25==0)
//            tramesbwShader.load("../shaders/trames_bw");
        curFbo->begin();
        t_shader->begin();
        t_shader->setUniform1f("levels", 2 + abs(parameterMap[engraving]));
        t_shader->setUniform1i("base", parameterMap[tintSaturation]>0 ? 1 : 0);
        srcFbo->draw(0,0);
        t_shader->end();
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
    }
#endif
    
    
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
    
    if(parameterMap[sobel]>BOOL_PARAM_EPSILON){
        if(!frameRateTurbo || bypassedFX[sobel] == 0 ){
            sobelContours(srcFbo, curFbo,
                          std::abs(app->parameterMap[sobel]),
                              app->parameterMap[sobelMix],
                              app->parameterMap[whiteSobel]);
            swapFBOs(srcFbo, curFbo);
        }
    }
    
    if(frameRateTurbo){
    frameRateTurboCounter += ofGetElapsedTimeMicros() - date;
    if(frameRateTurboCounter > 1000000/maxFrameRate){
        bypassedFX[sobel] = 1;
        swapFBOs(srcFbo, curFbo);
        return;
    }
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
        float alpha = abs(parameterMap[chromaSepAlpha]);
        chromaSeparation(srcFbo, curFbo, doInvertion, parameterMap[chromaSep]/2, parameterMap[chromaSepHue], parameterMap[chromaSepAngle], parameterMap[chromaOffset]/2, parameterMap[sidesSaturation], parameterMap[chromaSepAlpha]);
        swapFBOs(srcFbo, curFbo);
        if(parameterMap[doubleChromaSep]){
            for(int i=0;i<app->parameterMap[doubleChromaSep];i++){
                
                
                float factor = (parameterMap[doubleChromaSep] - i + 1) / parameterMap[doubleChromaSep];
                
                if(parameterMap[chromaSepAlpha]<0){
                    alpha = factor * abs(parameterMap[chromaSepAlpha]);
                }
                
                chromaSeparation(srcFbo, curFbo, doInvertion, parameterMap[chromaSep]/2*factor, parameterMap[chromaSepHue], parameterMap[chromaSepAngle], parameterMap[chromaOffset]/2*factor, parameterMap[sidesSaturation], alpha * factor);
                
                swapFBOs(srcFbo, curFbo);
            }
        }
    }
    
    if(abs(parameterMap[borderMask])>0.001 && render_WIDTH == WIDTH){
        curFbo->begin();
//               maskShader.load("../shaders/dmask");
        maskShader.begin();
        maskShader.setUniform1f("thresh", abs(parameterMap[borderMask]));
        maskShader.setUniform1f("alpha", 1);
        maskShader.setUniform1i("circleMode", parameterMap[borderMask]<0);
//        maskShader.setUniform1i("circleMode", 0);
        maskShader.setUniform1f("WIDTH", render_WIDTH);
        maskShader.setUniform1f("HEIGHT", render_HEIGHT);
        maskShader.setUniform1f("minDistance", parameterMap[borderMaskd0]);
        srcFbo->draw(0,0);
        maskShader.end();
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
    }
    
    //***** SPACE ****//
    

    
//    if(parameterMap[multiFbos]){
//
//        int o_index = 0;
//        int maxZIndex = 0;
//        float maxZ = 0;
//        int nb = app->parameterMap[omg3D2Nb];
//        
//        if(zs[o_index].size() != (int) nb){ //init //TODO reset zs on macro load ?
//            zs[o_index].clear();
//            for(int x=0; x<nb;x++){
//                zs[o_index].push_back(ofVec4f((x==0 || zs[o_index][x-1].x > 0) ? ofRandom(-WIDTH/4, 0) : ofRandom(0, WIDTH/4)
//                                              ,ofRandom(-WIDTH/4, WIDTH/4)
//                                              , x/nb
//                                              ,(x%2)==0));
//                zsFbosOk.push_back(false);
//            }
//        }
//
//        
//        for(int x=0; x<zs[o_index].size();x++){
//            zs[o_index][x].z += speed;
//
//            if(zs[o_index][x].z<0 || zs[o_index][x].z>1){
//                zsFbos[x]->begin();
//                ofBackground(0);
//                ofSetColor(ofColor::white);
//                srcFbo->draw(0, 0);
//                zsFbos[x]->end();
//            }
//            for(int index=0;index<zsFbos.size();index++) //force init
//                if(!zsFbosOk[index]){
//                    zsFbos[index]->begin();
//                    ofBackground(0);
//                    ofSetColor(ofColor::white);
//                    srcFbo->draw(0, 0);
//                    zsFbos[index]->end();
//                    zsFbosOk[index] = true;
//                }
//            
//            bool reset = false;
//            if(zs[o_index][x].z<0){
//                zs[o_index][x].z = 1;
//                reset = true;
//            }
//            if(zs[o_index][x].z>1){
//                zs[o_index][x].z = 0 ;
//                reset = true;
//            }
//            
//            if(reset){
//                zs[o_index][x].x = (x==0 || zs[o_index][x-1].x > 0) ? ofRandom(-WIDTH/4, 0) : ofRandom(0, WIDTH/4);
//                zs[o_index][x].y = ofRandom(-WIDTH/4, WIDTH/4);
//            }
//            
//            if(zs[o_index][x].z >= maxZ){
//                maxZ = zs[o_index][x].z;
//                maxZIndex = x;
//            }
//        }
//    }
    
//    if(parameterMap[multiFbos]>BOOL_PARAM_EPSILON && parameterMap[omg3D2]>BOOL_PARAM_EPSILON){
//        multiFbo_tmp.begin();
//        ofBackground(0,0,0);
//        srcFbo->draw(0,0);
//        multiFbo_tmp.end();
//    }
    
    if(parameterMap[customBorders] > BOOL_PARAM_EPSILON){
        curFbo->begin();
        srcFbo->draw(0,0);
        
        int intensity = parameterMap[customBorders] * render_HEIGHT;
        int intensity2 = render_HEIGHT/4;
        float f = ofGetFrameNum()/25.0;
        ofSetColor(0);
        int x = 0;
        int y = 0;
        while(x < render_WIDTH){
            int w = intensity/2 + ofNoise(f+x)*intensity/2;
            int h = ofNoise(f+w)*intensity2;
            ofDrawRectangle(x, y, w, h);
            x += w;
        }
        x = 0;
        y = render_HEIGHT;
        while(x < render_WIDTH){
            int w = intensity/2 + ofNoise(x-f)*intensity/2;
            int h = ofNoise(f+w)*intensity2;
            ofDrawRectangle(x, y-h, w, h);
            x += w;
        }
        
        x = 0;
        y = 0;
        while(y < render_HEIGHT){
            int w = ofNoise(f+y)*intensity2;
            int h = intensity/2 + ofNoise(f+h)*intensity/2;
            ofDrawRectangle(x, y, w, h);
            y += h;
        }
        
        x = render_WIDTH;
        y = 0;
        while(y < render_HEIGHT){
            int w = ofNoise(y-f)*intensity2;
            int h = intensity/2 + ofNoise(h-f)*intensity/2;
            ofDrawRectangle(x-w, y, w, h);
            y += h;
        }
        
        curFbo->end();
        
        ofSetColor(255);

        swapFBOs(srcFbo, curFbo);

    }
    
if(parameterMap[post_traitement])
    doOmg3D2();
    
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
    if(parameterMap[strobe]!=0 && localFrameNum%2!=0){
        doStrobe(srcFbo, curFbo);
        swapFBOs(srcFbo, curFbo);
    }
#endif
    
    
    //un etage de omg3D peut etre pas mal en cas d'utilisation d'omg3D2Y
    
    if(parameterMap[randHHide]>BOOL_PARAM_EPSILON || parameterMap[randVHide]>BOOL_PARAM_EPSILON){
        randHide(srcFbo, curFbo);
//        swapFBOs(srcFbo, curFbo);
    }
    
    if(abs(parameterMap[scan])>0.001 && abs(parameterMap[scan])<0.999){
        scan_(srcFbo, curFbo);
//        swapFBOs(srcFbo, curFbo);
    }
  
    
    if(parameterMap[kalei_2]>BOOL_PARAM_EPSILON){
        doKalei(srcFbo, curFbo, parameterMap[kalei_2], parameterMap[kaleiNb_2], parameterMap[kaleiOffX], parameterMap[kaleiOffY]);
        swapFBOs(srcFbo, curFbo);
    }
    
#ifdef STIENIS ///MASKING
    if(abs(parameterMap[user1])>0){
        
        float t = parameterMap[user1];
        if(t>0)
            t = 1-t;
        curFbo->begin();
        //        mappingmaskShader.load("../shaders/mappingmask");
        mappingmaskShader.begin();
        mappingmaskShader.setUniform1f("thresh", t);
        mappingmaskShader.setUniform1f("mode", 2);
        mappingmaskShader.setUniform1f("WIDTH", WIDTH);
        mappingmaskShader.setUniform1f("HEIGHT", HEIGHT);
        srcFbo->draw(0,0);
        mappingmaskShader.end();
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
    }
    
    if(abs(parameterMap[user2])>0){
        float t = parameterMap[user2];
        if(t>0)
            t = 1-t;
        curFbo->begin();
        //        mappingmaskShader.load("../shaders/mappingmask");
        mappingmaskShader.begin();
        mappingmaskShader.setUniform1f("thresh", t);
        mappingmaskShader.setUniform1f("mode", 1);
        mappingmaskShader.setUniform1f("WIDTH", WIDTH);
        mappingmaskShader.setUniform1f("HEIGHT", HEIGHT);
        srcFbo->draw(0,0);
        mappingmaskShader.end();
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
    }
#endif
    
    if(parameterMap[sobel]<-BOOL_PARAM_EPSILON){
        sobelContours(srcFbo, curFbo,
                      std::abs(app->parameterMap[sobel]),
                          app->parameterMap[sobelMix],
                          app->parameterMap[whiteSobel]);
        swapFBOs(srcFbo, curFbo);
    }

    doPaint(srcFbo, curFbo); //beware resolution

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
        ofDrawRectangle(0,0,w,25);
        ofSetColor(ofColor::white);
    }
    
    if(savingGif && ofGetFrameNum()%2 == 0){
        ofSetColor(100);
        ofDrawRectangle(0,0,w,25);
        ofSetColor(ofColor::white);
        stringstream str;
            str << "Gif export timecode @ 30FPS : " << (ofGetFrameNum() - gifCaptureStartFrame) / 30.0 << "s. " << endl;
            ofDrawBitmapString(str.str(), w/2, 20);
    }
    
//    if(isPrompt && secondDisplay!=0 ){
//        ofSetColor(ofColor::darkGray);
//        ofDrawRectangle(0,0,w,25);
//        ofSetColor(ofColor::white);
//    }
    
    
    if(isPrompt){
        stringstream str;
        string pstr = promptStr;
        if(quickPromptStr.compare(""))
            pstr = quickPromptStr;
        if(ofGetFrameNum()%14>7){
            str << "| -> | " << pstr << " : " << promptValue;
            int pid = getParameterFromName(pstr);
            if(pid>=0)
                str << " /// " << parameterMap[pid];
        }else
            str << "  " << pstr << " : " << promptValue;
        str << '\n' << '\n';
        
        for(int mi=0;mi<parameterIDMap.size();mi++){
            string s1 = parameterIDMap[mi];
            int pid = getParameterFromName(s1);
            if(strStartsWith(s1, pstr) && pid>=0 ){
                str << s1 << " : " <<  parameterMap[pid] <<  '\n';
            }
        }
//        if(secondDisplay==0){
            if(funkyPrompt){
            float decal = abs(ofNoise(ofGetFrameNum()/40.0));
            decal *= decal;
            decal *= 150;
            float decal2 = (ofNoise(2,ofGetFrameNum()/100.0));
            decal2 *= decal2;
            decal2 *= 5000;
            for(int d = 0;d<decal;d++){
                ofColor lerpedColor = ofColor::fromHsb(
                                                       guiColor.getHue() - 25 + 25 * d/float(decal/2) ,
                                                       guiColor.getSaturation()
                                                       //*(decal-d)/float(decal/2),
                                                       ,guiColor.getBrightness());
                if(d==int(decal)-1){
                    lerpedColor.setSaturation(50);
                    lerpedColor.setBrightness(255);
                }else{
                    lerpedColor.a = 200*d/float(decal);
                }
                ofSetColor( lerpedColor );
                promptFont.drawString(str.str(), 0.19*WIDTH-1 + d, h/5-1 + d/(float)(decal2/decal));
            }
            }else{
            ofSetColor( guiColor );
            promptFont.drawString(str.str(), 0.19*WIDTH-1, h/5-1);
            }
//        }
//        else{
//            promptFontSmall.drawString(str.str(), w/3, h/4);
//        }
        ofSetColor(ofColor::white);
    }
    
    if(saveMacros){
        ofSetColor(ofColor::green);
        string str;
        if(ofGetFrameNum()%10>5)
            str = "| "+saveMacroStr;
        else
            str = "  "+saveMacroStr;
        
//        if(secondDisplay==0)
            promptFont.drawString(str, w/2, h/2);
//        else{
//            promptFontSmall.drawString(str, w/2, h/2);
//        }
        ofSetColor(ofColor::white);
    }
    
    if(displayFrameRate
       //&& !safeMode
       ){
        ofSetColor(guiColor);
        stringstream str;
        str << "FPS " << ((int)(10*ofGetFrameRate()))/10.0 << endl;
        str << "Render 𝛕 " << drawMicros <<  " (" << ((int)(drawMicros/(1000000/maxFrameRate)*100.0)) << "%)" << endl <<
        "Update 𝛕 " << updateMicros <<  " (" << ((int)(updateMicros/(1000000/maxFrameRate)*100.0)) << "%)" << endl <<
        "Auto 𝛕 " << autoMicros <<  " (" << ((int)(autoMicros/(1000000/maxFrameRate)*100.0)) << "%)" << endl <<
        "Total 𝛕 " << drawMicros+drawMicros <<  " (" << ((int)((drawMicros+updateMicros)/(1000000/maxFrameRate)*100.0)) << "%)";
        str << endl << endl;
        for(int i=0;i<3;i++){
            if(scenes[i]!=0)
                str << scenes[i]->getInfo() << endl;
        }
        string macStr = currentMacroStr;
        macStr = macStr.replace(0, currentMacroStr.size()-25, "");
        str << "Crt. macro : []"<< macStr << endl;
        str << endl;
        
        /// OSC
        #ifdef OF_10
        str << "OSC "<< " port " << OSC_INPUT << " | statuts: "<< receiver.isListening()
        << " rcv " << oscHasReceivedMessages  << endl;
        str << endl;
#else
        str << "OSC "<< " port " << OSC_INPUT << " | statuts: "<< receiver.listen_socket->IsBound()
        << " rcv " << oscHasReceivedMessages  << endl;
#endif
        str << endl;
        
        /// MIDI
        str << "MIDI listening : " << MIDI_check_listen << " " << endl;
        for(int i=0;i<MIDI_MAX_NB_PORTS;i++){
            if(MIDI_check_ports[i]){
                str << "\t" << "port #" << i << " " << MIDI_check_portNames[i] << endl;
            }
        }
        
        cout << "Surprise parameters" << endl;
        
        for(int sp : surpriseParameters)
            cout << parameterIDMap[sp] << " ";
        cout << endl;
        

        
//        if(secondDisplay==0)
            ofDrawBitmapString(str.str(), w/2, h/4);
//        else
//            promptFontSmall.drawString(str.str(), w/2, h/4);
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

void eqkoscope::doOmg3D2(){
    if (parameterMap[omg3D2]>BOOL_PARAM_EPSILON
        || parameterMap[omg3D2X]>BOOL_PARAM_EPSILON
        || parameterMap[omg3D2Y]>BOOL_PARAM_EPSILON){
        //        if(!parameterMap[multiFbos] || (parameterMap[post_traitement]))
        {
            ofPushMatrix();
            
            /*if (parameterMap[feedBack]>BOOL_PARAM_EPSILON){
                tempFbo.begin();
                feedbackFbo.draw(0,0); /// DRAW points noBack
                tempFbo.end();
            }*/
            
            curFbo->begin();
            //ofBackground(parameterMap[_invert]  ? 255 : 0);
          
            ofBackground(0,0,0);
            

            curFbo->end();
            
            ///curfbo -> should only get points noBack
            displayOmg3D2(srcFbo, curFbo, parameterMap[omg3D2Speed],
                          parameterMap[multiFbos]>BOOL_PARAM_EPSILON, parameterMap[omg3D2Rotation], parameterMap[omg3D2Dist], parameterMap[omg3D2Symetry], 0, 0);
            
            //here
            //        tunnel(srcFbo, curFbo, scenes[0]!=feedbackScene, parameterMap[omg3D2Speed],
            //                      parameterMap[multiFbos]>BOOL_PARAM_EPSILON, parameterMap[omg3D2Rotation], parameterMap[omg3D2Dist], parameterMap[omg3D2Symetry],
            //                      parameterMap[omg3D2FreeRotation]);
            
         /*   if (parameterMap[feedBack]>BOOL_PARAM_EPSILON){
                feedbackFbo.begin(); ///STORE points noBack
                ofBackground(0,0,0);
                curFbo->draw(0,0); //draw the new frames
                feedbackFbo.end();
                
                srcFbo->begin(); //draw last frame as background
                ofBackground(parameterMap[_invert]  ? 255 : 0);
                
                //                tempFbo.draw(0,0); //draw the background
                //                blendShader.begin();//add alpha shading
                //                blendShader.setUniform1i("mode", 0);
                //                blendShader.setUniform1f("_mix", 1);
                //                blendShader.setUniform1f("thresh", 0.5);
                //                blendShader.setUniform1f("WIDTH", render_WIDTH);
                //                blendShader.setUniform1f("HEIGHT", render_HEIGHT);
                //                if(app->parameterMap[user3]==0)
                feedbackFbo.draw(0,0); /// DRAW points noBack
                //                blendShader.end();
                
                srcFbo->end();
            }*/
            
            ofPopMatrix();
//            if (parameterMap[feedBack]==0 || parameterMap[post_traitement]==0) //post_traitement co,ditio, ? why ?
                swapFBOs(srcFbo, curFbo);
            
            ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        }
    }
}

void eqkoscope::doPaint(ofFbo* srcFbo, ofFbo* curFbo ){
//    if(ofGetFrameNum()%25==0)                 chromaShader.load("../shaders/chromaSep");
    
    if(parameterMap[paint]){ //random calls are pretty fast

        srcFbo->readToPixels(pix); //does this gets optimized ?
#ifdef OF_10
        if(pix.getData()!=nullptr){
#else
            if(pix.getPixels()!=NULL){
#endif
            float r;
            float xx, yy;
            int fboIndex = ofGetFrameNum() % paintFbos.size();
            
            paintFbos[fboIndex]->begin();
            ofBackground(0, 0, 0, 0);
//            srcFbo->draw(0,0);
            float s = parameterMap[paint];
            paintPass(20/parameterMap[paint], s*(1-parameterMap[paintSizeDelta]), s, &pix);
            paintFbos[fboIndex]->end();
            
            if(paintFbos.size()>1){
                paintFbo.begin();
                ofBackground(0);
                //ofBackground(0, 0, 0, 1-app->parameterMap[user3]); //paintOriginalMix
                //            ofEnableBlendMode(OF_BLENDMODE_SCREEN);
                for(int i=0;i<paintFbos.size();i++){
                    int index = (i + fboIndex - 1) % paintFbos.size();
                    alphaBlend( paintFbos[index]); //too harsh ?
                    //paintFbos[index]->draw(0,0);
                }
                ofEnableBlendMode(OF_BLENDMODE_ALPHA);
                paintFbo.end();

                curFbo->begin();
                paintFbo.draw(xOffset,yOffset);
                curFbo->end();
            }else{
                curFbo->begin();
                paintFbos[0]->draw(xOffset,yOffset);
                curFbo->end();
            }
        }
            swapFBOs(srcFbo, curFbo);
    }
}

void eqkoscope::paintPass(int resolution, float minSize, float maxSize, ofPixels* pix){
    if(brushstrokes.size()==0)
        return;

    float xx, yy, r;
    resolution = HEIGHT * app->parameterMap[paintResolution];
    resolution = max(5, resolution);
    for(int x=resolution/2;x<WIDTH;x+=resolution)
        for(int y=resolution/2;y<HEIGHT;y+=resolution){ //lattice
            xx = x;
            yy = y;
            xx += ofRandom(-resolution/2, resolution/2);
            yy += ofRandom(-resolution/2, resolution/2);
            ofColor c = pix->getColor(xx, HEIGHT2-(yy+(HEIGHT2-HEIGHT)/2));
            float b = c.getBrightness();
            bool pb = app->parameterMap[_invert] == 0;
            yy = render_HEIGHT - yy;
            if((b/255.0 > 0.3 && pb) || (b/255.0 < 0.7 && !pb)){ //selective
                
                float ci = parameterMap[_reTint];

                chromaShader.begin();
                chromaShader.setUniform1f("WIDTH", WIDTH);
                chromaShader.setUniform1f("HEIGHT", HEIGHT);
                chromaShader.setUniform1f("intensity", 0);
                chromaShader.setUniform1f("hue", c.getHue()/255.0);
                chromaShader.setUniform1f("saturation", c.getSaturation()/255.0);
                chromaShader.setUniform1f("forceBrightness", c.getBrightness()/255.0);
                chromaShader.setUniform1f("gradient", app->parameterMap[gradient]);
                chromaShader.setUniform1f("reSaturate", app->parameterMap[reSaturate]);
                chromaShader.setUniform1f("reTint", 1-ci);
                //chromaShader.setUniform1f("reTint", ofRandom(1-ci) < 0.5 ? 0 : 1); //nopy nope !

                r = 360*ofNoise(xx*parameterMap[pertEvo],yy*parameterMap[pertEvo]);
                ofPushMatrix();
                ofTranslate(xx,yy);
                ofRotate(r);
                int strokeIndex = (int)ofRandom(brushstrokes.size()-0.5);
                
                if(1==1){
                    chromaShader.setUniform1f("useTexture", 1);
                    chromaShader.setUniform1f("texOffX", ofRandom(paintTexture.getWidth() - brushstrokes[strokeIndex]->getWidth()));
                    chromaShader.setUniform1f("texOffY", ofRandom(paintTexture.getHeight() - brushstrokes[strokeIndex]->getHeight()));
                    //                chromaShader.setUniform1f("texOffX", x);
                    //                chromaShader.setUniform1f("texOffY", y);
#ifdef OF_10
                    chromaShader.setUniformTexture("tex1", paintTexture.getTexture(), 1);
#else
                    chromaShader.setUniformTexture("tex1", paintTexture.getTextureReference(), 1);
#endif
                }
                
                float sizeRatio = ofRandom(minSize, maxSize);
                brushstrokes[strokeIndex]->draw(-brushstrokes[strokeIndex]->getWidth()/2*sizeRatio, -brushstrokes[strokeIndex]->getHeight()/2*sizeRatio,
                                                brushstrokes[strokeIndex]->getWidth()*sizeRatio,
                                                brushstrokes[strokeIndex]->getHeight()*sizeRatio);
                ofPopMatrix();
                chromaShader.end();
            }
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
                brushstrokes[strokeIndex]->draw(-brushstrokes[strokeIndex]->getWidth()/2*sizeRatio, -brushstrokes[strokeIndex]->getHeight()/2*sizeRatio,
                                                brushstrokes[strokeIndex]->getWidth()*sizeRatio,
                                                brushstrokes[strokeIndex]->getHeight()*sizeRatio);
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
        if(keyIsDown[OF_KEY_SHIFT] || keyIsDown[OF_KEY_COMMAND])
            deltaMap[featuredParameter] = parameterMap[featuredParameter] = scaleGUIValue(featuredParameter, ofMap(ofGetMouseX(), 0, float(WIDTH), 0, 1));
    
    if(parameterMap[useJoyStick]){
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
   
#ifdef NEW_GUI
    if(newGUI!=NULL)
    newGUI->mouseDragged(x, y);
#endif
//    if(featuredParameter>=0){
    
//        if(isShiftModifier){
//            deltaMap[featuredParameter] = parameterMap[featuredParameter] = scaleGUIValue(featuredParameter, ofMap(ofGetMouseX(), 0, float(WIDTH), 0, 1));
//        }else{
//        newGUI->mouseDragged(x, y);
//        }
}

void eqkoscope::mousePressed(int x, int y, int button){
    if(scenes[parameterMap[currentScene]]!=0)
        scenes[parameterMap[currentScene]]->mousePressed(x,y,button);
    
    #ifdef NEW_GUI
    if(newGUI!=NULL)
    newGUI->mousePressed(x, y);
#endif
    
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
                loadMappingFiles("parameterMappings/"+controlFile, false, true);
        }else{
            if(!ext.compare("xml")){
                loadMacro(f.getAbsolutePath());
            }else{
                if(!ext.compare("mp4") || !ext.compare("webm") || !ext.compare("mov")|| !ext.compare("gif")
                   || !ext.compare("avi")|| !ext.compare("mkv")){
                    if(scenes[app->parameterMap[currentScene]]==feedbackScene){
                        cinema->loadDirectMovie(f.getFileName());
                        feedbackScene->nestedScene = cinema;
//                        app->deltaMap[noSource] = 1;
                        return;
                    }
                    if(cinemas.size()>0 && scenes[app->parameterMap[currentScene]]==cinemas[0])
                        cinemas[0]->loadDirectMovie(f.getFileName());
                    else{
                        if(scenes[app->parameterMap[currentScene]]!=cinema)
                            loadSoloScene(cinema);
                        cinema->loadDirectMovie(f.path());
                    }
                }else{
                    if(!ext.compare("jpg") ||!ext.compare("jp2") || !ext.compare("jpeg") || !ext.compare("gif") ||!ext.compare("bmp") 
                       || !ext.compare("png")|| !ext.compare("svg")){
                        if(parameterMap[paint]>0){
                            paintTexture.loadImage(f.getAbsolutePath());
                            return;
                        }
                        
                        if(scenes[app->parameterMap[currentScene]]==feedbackScene){
                            uzi->loadDirectImage(f.getAbsolutePath());
                            feedbackScene->nestedScene = uzi;
//                            app->deltaMap[noSource] = 1;
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
    if(recordAndRenderFlag==0)
        logfile.close();
    
    if(MIDI_device_connected.count(MACRO_INTERFACE) && MIDI_device_connected[MACRO_INTERFACE])
        for(int i=0;i<NB_MACRO_PER_PAGE;i++)
            launchpadOut.sendNoteOn(1, i, 0);
    
    if(recordAndRenderFlag==-1)
        saveMacro("./lastMacro.csv");
}
    
    void eqkoscope::setResolution(int w, int h){
        FINALWIDTH = render_WIDTH = crt_WIDTH = w;
        FINALHEIGHT = render_HEIGHT = crt_HEIGHT = h;
        
        i.allocate(WIDTH,HEIGHT, OF_IMAGE_COLOR); //put on stack ?
        fbo.allocate(WIDTH, HEIGHT2, GL_RGBA);
        fbo2.allocate(WIDTH, HEIGHT2, GL_RGBA);
        tempFbo.allocate(WIDTH, HEIGHT2, GL_RGBA);
        tempFbo2.allocate(WIDTH, HEIGHT2, GL_RGBA);
        copyFbo.allocate(WIDTH,HEIGHT);
        multiFbo_tmp.allocate(WIDTH, HEIGHT2, GL_RGBA);
        
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
        if(app->attachedSecondDisplay && app->dualDisplay){
            w += app->attachedSecondDisplayWidth;
        }
        ofSetWindowShape(w, h);
        
        if(app->dualDisplay)
            ofSetWindowPosition(-app->FINALWIDTH, 0);
        
        cinema->setResolution();
        uzi->setResolution();
        feedbackScene->setResolution();
        lines->setResolution();
    }
    

void eqkoscope::setResolution(int r){
//    switch(r){
//        case 720:{
//            crt_WIDTH = 1280;
//            crt_HEIGHT = 720;
//        }break;
//        case 1080:{
//            crt_WIDTH = 1900;
//            crt_HEIGHT = 1080;
//        }break;
//        default:{
//            crt_HEIGHT = r;
//            crt_WIDTH = 16*r/9;;
//        }
//            break;
//    }
    setResolution(r,r);
}

void eqkoscope::niceRandom(int x){
    
    deltaMap[skewAmp] = 0;
    deltaMap[_reTint] = 0;
    deltaMap[tintSaturation] = 1;
    deltaMap[displaceAmp] = 0;
    //    deltaMap[sobel]  = 0;
    deltaMap[hueShift] = 0;
    deltaMap[omg3D2] = 0;
    deltaMap[kalei] = 0;
    deltaMap[pert] = 0;
    deltaMap[nFreeze] = 0;
    
    switch(x){
        case 0:{ // tunnel
            parameterEasingMap[mediaRotZ] = 0.3f;
            //            deltaMap[mediaRotZ] += 90;
            deltaMap[mediaRotZ] += ofRandom(-120, 120);
            
            //            parameterEasingMap[mediaX] = 0.3f;
            //            deltaMap[mediaX] = ofRandom(-0.3, 0.3);
            //
            //            parameterEasingMap[mediaY] = 0.3f;
            //            deltaMap[mediaY] = ofRandom(-0.3, 0.3);
            
            parameterEasingMap[mediaZ] = 0.3f;
            deltaMap[mediaZ] = ofRandom(0, 0.3);
            
            deltaMap[omg3D2] = 1;
            deltaMap[omg3D2Dist] = ofRandom(0.5,1);
            deltaMap[omg3D2Rotation] = 45*(int) ofRandom(0,12);
            deltaMap[omg3D2HardRotation] = 1;
            
            parameterEasingMap[kalei] = 0.3f;
            deltaMap[kalei] =  ofRandom(0,1);
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

void eqkoscope::startSequenceSave(){
    savingGif = true;
    gifIndex = int(ofRandom(1,100000));
    gifCaptureStartFrame = ofGetFrameNum();
    
#ifdef VIDEO_EXPORT
    if(!hasTakenVideo){
        if(!vidRecorder.isInitialized()){
            vidRecorder.setup("capture/vid_"+ofGetTimestampString(), WIDTH, HEIGHT, 30, sampleRate, channels);
        }
        vidRecorder.start();
    }
#endif
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

    if(scenes[0]!=NULL)
    scenes[0]->setup();
    
    initParameters();
    
    analyzeMacros();
    
    for(int i=0;i<N_PARAM;i++){
        deltaMap[i] = parameterMap[i];
    }
 
#ifdef NEW_GUI
    if(newGUI!=0)
    newGUI->updateAutos();
#endif
}



