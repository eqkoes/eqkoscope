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
#ifdef LAZERS
    etherdream.setup();
    etherdream.setPPS(30000);
#endif
    
    roundMaskImg.loadImage("assets/softroundmask_720p.png");
    
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
    
//    ofSetFullscreen(liveMode);
    if(liveMode)
        safeMode = true;
    
    cout << "dateC " << ofGetTimestampString() << endl;

    initGlitches(this, skewVector);
    cout << "dateD " << ofGetTimestampString() << endl;

    
    /** SCENES **/
    for(int i=0;i<3;i++){
        scenes.push_back(0);
        ofFbo fbo;
        fbo.allocate(WIDTH, HEIGHT2);
        sceneFbos.push_back(fbo);
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

    
    for(int i=0;i<2;i++){
        cinemas.push_back(new Cinema(this));
        cinemas[i]->setSleep(1);
    }
    for(int i=0;i<2;i++){
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
    
    pixelHShader.load("../shaders/pixelsH");
    threedeeShader.load("../shaders/threeDee");
    lineShader.load("../shaders/line");
    sharpenShader.load("../shaders/sharpen");
    saturationShader.load("../shaders/saturation");
    testShader.load("../shaders/threeDee");
    blendShader.load("../shaders/blend");
    disposeShader.load("../shaders/dispose");
    hueShiftShader.load("../shaders/hueShift");
    maskShader.load("../shaders/dmask");
    
    fbo.allocate(WIDTH, HEIGHT2, GL_RGBA);
    fbo2.allocate(WIDTH, HEIGHT2, GL_RGBA);
    tempFbo.allocate(WIDTH, HEIGHT2, GL_RGBA);
    tempFbo2.allocate(WIDTH, HEIGHT2, GL_RGBA);
    
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

    
    int brushDownSize = 3;
    for(int i=0;i<brushstrokes.size();i++){
        brushstrokes[i]->resize(brushstrokes[i]->width/brushDownSize,brushstrokes[i]->height/brushDownSize);
    }
    
    int NB_PAINT_FRAMES = 50;
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

    int tcpPort = ofRandom(10000,20000);
#ifndef MASTER
    tcpServer.setup(tcpPort);
#endif
#ifndef SLAVE
    tcpClient.setup("192.168.0.43", tcpPort);
#endif

}

void eqkoscope::swapFBOs(ofFbo* a, ofFbo* b){
    srcFbo = (srcFbo==&fbo) ? &fbo2 : &fbo;
    curFbo = (curFbo==&fbo) ? &fbo2 : &fbo;
}

void eqkoscope::draw(){

#ifndef SLAVE
    return;
#endif

    
    if(pause)
        return;
    
    sceneMutex.lock();

    
    long date = ofGetElapsedTimeMicros();
    
    doGlitches = false;
    if((ofGetElapsedTimeMillis() - lastGlitchDate) > 1000/parameterMap[glitchFreq]
       || ofRandom(ofGetElapsedTimeMillis() - lastGlitchDate) > 1000/parameterMap[glitchFreq]){
        doGlitches = true;
        lastGlitchDate = ofGetElapsedTimeMillis();
    }
    
    ofTranslate((ofGetWidth() - WIDTH)/2, (ofGetHeight() - HEIGHT)/2);
    
    ofHideCursor();
    
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
//    sceneMutex.lock();
    if(scenes.size()>0 && scenes[0]!=0)
        scenes[0]->draw();
    ofSetColor(255);
//    sceneMutex.unlock();
    
    grabFbo->end();
    
    ofPopMatrix();
    
   fbo.begin(); //CPU
//    sceneFbos[0].begin();
    
    grabFbo->draw(0, 0);
    
    if (parameterMap[stripesAmp]>0)
        for (int i=0; i<parameterMap[stripesAmp]*5; i++)
            stripe(parameterMap[stripesSize]);
    
    
    if (parameterMap[post_traitement]){
        ofSetColor(parameterMap[tintBrightness]*255, parameterMap[tintBrightness]*255, parameterMap[tintBrightness]*255);
        ofPushMatrix();
//        sceneMutex.lock();
        if(scenes[0]!=0){
            scenes[0]->mask();
            scenes[0]->capture(grabFbo);
        }
//        sceneMutex.unlock();
        ofPopMatrix();
    }
    
        fbo.end();
//    sceneFbos[0].end();
    
    /** FX **/
    curFbo = &fbo2;
        srcFbo = &fbo;
//    srcFbo = &(sceneFbos[0]);
    
    applyFXChain(srcFbo, curFbo);
    
    //need mutex
    if (!(parameterMap[post_traitement]>BOOL_PARAM_EPSILON)){
        ofSetColor(parameterMap[tintBrightness]*255);
        ofPushMatrix();
//        sceneMutex.lock();
        scenes[0]->capture(srcFbo); //2
        scenes[0]->mask(); //1
//        sceneMutex.unlock();
        ofPopMatrix();
    }
    
    /** GLOBAL PARAMETERS **/
    
    if(parameterMap[ball]>BOOL_PARAM_EPSILON){
        sphere(srcFbo, curFbo);
        swapFBOs(srcFbo, curFbo);
    }
    
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
        fxShader.begin();
        fxShader.setUniform1f("alpha", parameterMap[tintBrightness]);
        fxShader.setUniformTexture("tDiffuse", srcFbo->getTextureReference(), 0);
#ifndef GAME
        srcFbo->draw(xOffset, yOffset);
#else
        srcFbo->draw(0,0);
#endif
        fxShader.end();
    }else{
#ifndef GAME
        srcFbo->draw(xOffset,-(HEIGHT2-HEIGHT)/2 + yOffset);
#else
        srcFbo->draw(0,-(HEIGHT2-HEIGHT)/2 + 0);
#endif
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
    
    doPaint();
    
    applyPostProcess();
    
    /** ECHO RECORD **/
    if(parameterMap[echoNb]>0){ //record each frame
        //        if(ofGetElapsedTimeMillis()-lastEcho > parameterMap[echoPeriod]){
        //            lastEcho = ofGetElapsedTimeMillis();
        currentEcho = (++currentEcho) % MAX_NB_ECHOES;
        echoFbos[currentEcho]->begin();
        srcFbo->draw(0,0);
        if(app->parameterMap[roundMask]){
            ofPushMatrix();
            ofTranslate(WIDTH/2, HEIGHT2/2);
            ofScale(1/app->parameterMap[roundMask], 1/app->parameterMap[roundMask]);
            roundMaskImg.draw(-WIDTH/2, -HEIGHT/2, WIDTH, HEIGHT);
            ofPopMatrix();
        }
        echoFbos[currentEcho]->end();
        //        }
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
                    blendShader.setUniform1f("_mix", 1/float(parameterMap[echoNb]));
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
    
    /** GAME **/
    
#ifdef GAME//game mode
    float ded = true;
    i.grabScreen(0, 0, WIDTH, HEIGHT2);
    for(int x = -10;x<10;x++){
        for(int y = -10;y<10;y++){
            if(    i.getColor(WIDTH/2+x, HEIGHT2/2+y).getBrightness()<180){
                ded = false;
                break;
            }
        }
    }
    if(ded){
        ofSetColor(255,0,0, 100);
        ofRect(0,0,WIDTH,HEIGHT2);
    }
#endif
    
    ofSetColor(ofColor::black); //borders
    ofRect(0, -(HEIGHT2-HEIGHT)/2, WIDTH, 0);
    ofRect(0, HEIGHT, WIDTH, HEIGHT2+1);
    ofSetColor(ofColor::white);
    
    if(parameterMap[audio]){
        audioImg.grabScreen(WIDTH/2-audioCapture/2, HEIGHT/2 - audioCapture/2, audioCapture, audioCapture);
    }
    
    if((parameterMap[nFreeze]>0 && 0==0) || analyzeAudioB || savingGif || analyzeImg){
        i.grabScreen(0,0,WIDTH,HEIGHT);
        if(analyzeAudioB)
            analyzeAudio();
        if(savingGif){
            i.resize(GIF_WIDTH, GIF_HEIGHT);
            i.update();
            i.saveImage("capture/gif_"+ofToString(gifIndex)+"/"+ofToString(ofGetFrameNum())+".jpg", OF_IMAGE_QUALITY_HIGH);
            isGifRecording = true;
        }
    }
    
    if(saveFrame){
        i.grabScreen(0,0 , FINALWIDTH, FINALHEIGHT);
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
//        displayInfo(width, height);
    }
//    else{
        displayInfo(FINALWIDTH, FINALHEIGHT);
//    }
    
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
    

    
    if(parameterMap[triumMode]>0){
        doTrium(srcFbo, curFbo);
        swapFBOs(srcFbo, curFbo);
    }
    
    if(doGlitches){
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
    
//    if(abs(parameterMap[invertCircle])>BOOL_PARAM_EPSILON|| (!parameterMap[strobe] && parameterMap[_invert]>BOOL_PARAM_EPSILON) || (parameterMap[strobe] && (ofGetFrameNum()%2)!=parameterMap[_invert])){
//        doInvert(srcFbo, curFbo);
//        swapFBOs(srcFbo, curFbo);
//    }
//    
    if(abs(parameterMap[_gamma]-1)>BOOL_PARAM_EPSILON || abs(parameterMap[contrast])>BOOL_PARAM_EPSILON){
        doGamma(srcFbo, curFbo, parameterMap[_gamma]);
        swapFBOs(srcFbo, curFbo);
    }
    

    
    if(parameterMap[sobel]>BOOL_PARAM_EPSILON){
        sobelContours(srcFbo, curFbo);
        swapFBOs(srcFbo, curFbo);
    }
    
    if(parameterMap[chromaSep]>0 || parameterMap[tintSaturation]>0 || parameterMap[_reTint] || 1==1){ //saturation mgmt
        chromaSeparation(srcFbo, curFbo, parameterMap[chromaSep], parameterMap[chromasepHue], parameterMap[chromasepAngle], parameterMap[chromaOffset], parameterMap[tintAmp], parameterMap[_reTint], parameterMap[sidesSaturation] + (blackNWhiteMedia?1:0));
        swapFBOs(srcFbo, curFbo);
        if(parameterMap[doubleChromaSep]){
            chromaSeparation(srcFbo, curFbo, parameterMap[chromaSep]/2, parameterMap[chromasepHue], parameterMap[chromasepAngle], parameterMap[chromaOffset]/2, parameterMap[tintAmp], parameterMap[_reTint], parameterMap[sidesSaturation] + (blackNWhiteMedia?1:0));
            swapFBOs(srcFbo, curFbo);
        }
    }
    
    if(parameterMap[kalei]>BOOL_PARAM_EPSILON){
        doKalei(srcFbo, curFbo, parameterMap[kaleiNb], parameterMap[kaleiOffX], parameterMap[kaleiOffY]);
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
    
    //mempry leak
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
    
    // ofFbo copyFbo;
    // copyFbo.allocate(WIDTH, HEIGHT);
    //        copyFbo.begin();
    //        ofBackground(0);
    //        srcFbo->draw(0,-(HEIGHT2-HEIGHT)/2);
    //        copyFbo.end();
    //        pastFbos.push_back(copyFbo);
    //        if(pastFbos.size()>nbPastFbos)
    //            pastFbos.erase(pastFbos.begin());
    
    
    /** GLOW **/
//    if(parameterMap[glow]>BOOL_PARAM_EPSILON){ //B4
//        tempFbo2.begin(); //source
//        srcFbo->draw(0,0);
//        tempFbo2.end();
//
//        doGlow(srcFbo, curFbo, &tempFbo, parameterMap[glow], parameterMap[glowIntensity], parameterMap[glowResolution]);
//        ofEnableBlendMode(OF_BLENDMODE_ADD);
//        curFbo->begin();
//        srcFbo->draw(0,0);
//        tempFbo2.draw(0,0);
//        curFbo->end();
//        swapFBOs(srcFbo, curFbo);
//        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//    }
    
    #ifdef USE_FREEZE
    if(app->parameterMap[nFreeze]>1 || app->parameterMap[nBlocks]>=1){
        srcFbo->begin();
        freezer->display();
        srcFbo->end();
    }
#endif
    
#ifdef USE_FREEZE
    if(doGlitches){
        for (int i=0; i<app->parameterMap[nBlocks]; i++)
            freezer->ablock();
        
        for (int j=0; j<app->parameterMap[nFreeze]; j++)
            freezer->shift(srcFbo);
    }
#endif

    
    if(parameterMap[borderMask]>BOOL_PARAM_EPSILON){
        curFbo->begin();
        //        maskShader.load("../shaders/dmask");
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
    
    if(parameterMap[omg3D]>BOOL_PARAM_EPSILON){
        displayOmg3D(srcFbo, curFbo);
        swapFBOs(srcFbo, curFbo);
    }
    
//    if(parameterMap[omg3D2]>BOOL_PARAM_EPSILON){
//        displayOmg3D2(srcFbo, curFbo, scenes[0]!=feedbackScene, parameterMap[omg3D2Speed],
//                      parameterMap[multiFbos]>BOOL_PARAM_EPSILON, parameterMap[omg3D2Rotation], parameterMap[omg3D2Dist], parameterMap[omg3D2Symetry],
//                      parameterMap[omg3D2FreeRotation]);
//        swapFBOs(srcFbo, curFbo);
//    }
    
//    if(parameterMap[omg3D2]>BOOL_PARAM_EPSILON){
//        displayOmg3D2(srcFbo, curFbo, scenes[0]!=feedbackScene, parameterMap[omg3D2Speed],
//                      parameterMap[multiFbos]>BOOL_PARAM_EPSILON, parameterMap[omg3D2Rotation], parameterMap[omg3D2Dist], parameterMap[omg3D2Symetry],
//                      parameterMap[omg3D2FreeRotation]);
//        swapFBOs(srcFbo, curFbo);
//    }
//    
//    if(abs(parameterMap[invertCircle])>BOOL_PARAM_EPSILON|| (!parameterMap[strobe] && parameterMap[_invert]>BOOL_PARAM_EPSILON) || (parameterMap[strobe] && (ofGetFrameNum()%2)!=parameterMap[_invert])){
//        doInvert(srcFbo, curFbo);
//        swapFBOs(srcFbo, curFbo);
//    }

    
    if(parameterMap[glow]>BOOL_PARAM_EPSILON){
        tempFbo2.begin(); //source
        srcFbo->draw(0,0);
        tempFbo2.end();
        
        doGlow(srcFbo, curFbo, &tempFbo, parameterMap[glow], parameterMap[glowIntensity], parameterMap[glowResolution]);
        swapFBOs(srcFbo, curFbo);
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        curFbo->begin();
        ofBackground(0);
        srcFbo->draw(0,0);
        tempFbo2.draw(0,0);
        curFbo->end();
        swapFBOs(srcFbo, curFbo);
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    }
    
    if(parameterMap[omg3D2]>BOOL_PARAM_EPSILON){
        displayOmg3D2(srcFbo, curFbo, scenes[0]!=feedbackScene, parameterMap[omg3D2Speed],
                      parameterMap[multiFbos]>BOOL_PARAM_EPSILON, parameterMap[omg3D2Rotation], parameterMap[omg3D2Dist], parameterMap[omg3D2Symetry],
                      parameterMap[omg3D2FreeRotation]);
        swapFBOs(srcFbo, curFbo);
    }
    
    if(abs(parameterMap[invertCircle])>BOOL_PARAM_EPSILON|| (!parameterMap[strobe] && parameterMap[_invert]>BOOL_PARAM_EPSILON) || (parameterMap[strobe] && (ofGetFrameNum()%2)!=parameterMap[_invert])){
        doInvert(srcFbo, curFbo);
        swapFBOs(srcFbo, curFbo);
    }
    
    //un etage de omg3D peut etre pas mal en cas d'utilisation d'omg3D2Y
    
    if(parameterMap[randHHide]>BOOL_PARAM_EPSILON){
        randHide(srcFbo, curFbo, false);
        swapFBOs(srcFbo, curFbo);
    }
    if(parameterMap[randVHide]>BOOL_PARAM_EPSILON){
        randHide(srcFbo, curFbo, true);
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
    //ofTranslate(w/2, h/2);
    
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
        ofDrawBitmapString(str.str(), 50, h/4);
        else
        font2.drawString(str.str(), 50, h/4);
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
            ofDrawBitmapString(str, 50, h/2);
        else
            font2.drawString(str, 50, h/2);
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

        if(secondDisplay==0)
        ofDrawBitmapString(str.str(), 50, 25);
        else
            font2.drawString(str.str(), 50, 25);
        ofSetColor(ofColor::white);
    }

    ofPopMatrix();
}

void eqkoscope::displayDualScenes(){
    ofPushMatrix();
    ofTranslate(xOffset,yOffset);
//    blendShader.load("../shaders/blend");
    
    ofSetColor(255);
    
    ofPopMatrix();
    
    if(scenes[1]!=0){
        bool crust = int(parameterMap[blendType])==MULTIPLY || int(parameterMap[blendType])==ADD||  int(parameterMap[blendType])==SCREEN || int(parameterMap[blendType])==SUBTRACT || int(parameterMap[blendType])==INCRUSTATION;
        
        if(crust){
            tempFbo.begin();
            ofBackground(0);
            ofSetColor(255);
            srcFbo->draw(0, -(HEIGHT2-HEIGHT)/2);
            tempFbo.end();
        }
        
        curFbo->begin();
        ofTranslate(0, -(HEIGHT2-HEIGHT)/2);
        
        if(crust){
        }else{
            ofSetColor(255);
            srcFbo->draw(0, 0);
        }
        
        blendShader.begin();
        blendShader.setUniform1i("mode", int(parameterMap[blendType]));
        blendShader.setUniform1f("prollageFreq", parameterMap[prollage]);
        blendShader.setUniform1f("WIDTH", WIDTH);
        blendShader.setUniform1f("HEIGHT", HEIGHT);
        blendShader.setUniform1f("trapeze", trapeze);
        if(crust)
            blendShader.setUniformTexture("tex1", tempFbo.getTextureReference(), 1);
        ofPushMatrix();
        scenes[1]->draw();
        ofSetColor(255);
        ofPopMatrix();
        blendShader.end();
        
//        srcFbo->draw(0, -(HEIGHT2-HEIGHT)/2);
//        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//        ofPushMatrix();
//        scenes[1]->draw();
//        ofSetColor(255);
//        ofPopMatrix();
//        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
  
        
        curFbo->end();
        swapFBOs(srcFbo,curFbo);
    
    }
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
    if(app->parameterMap[roundMask]){
        ofPushMatrix();
    ofTranslate(WIDTH/2, HEIGHT2/2);
    ofScale(1/app->parameterMap[roundMask], 1/app->parameterMap[roundMask]);
    roundMaskImg.draw(-WIDTH/2, -HEIGHT/2, WIDTH, HEIGHT);
    ofPopMatrix();
    }
    curFbo->end();
    curFbo->draw(0, 0);
}

void eqkoscope::doPaint(){
    if(parameterMap[paint]){ //random calls are pretty fast
        srcFbo->readToPixels(pix);
        if(pix.getPixels()!=NULL){
            float r;
            float xx, yy;
            int fboIndex = ofGetFrameNum() % paintFbos.size();
            paintFbos[fboIndex]->begin();
            
            paintPass(60, 0.75, 1, &pix); //different passes for different sizes
            paintPass(20, 0.25, 0.75, &pix);
            
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
//            ofRotate(r);
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


void eqkoscope::mouseMoved(int x, int y){
    if(scenes[parameterMap[currentScene]]==drawscene)
        drawscene->mouseMoved(x,y);
    if(featuredParameter>=0)
        deltaMap[featuredParameter] = parameterMap[featuredParameter] = ofMap(ofGetMouseX(), 0, float(WIDTH), 0, 1);
}
void eqkoscope::mouseDragged(int x, int y, int button){
    if(scenes[parameterMap[currentScene]]!=0)
        scenes[parameterMap[currentScene]]->mouseDragged(x,y,button);
    if(featuredParameter>=0)
        deltaMap[featuredParameter] = parameterMap[featuredParameter] = ofMap(ofGetMouseX(), 0, float(WIDTH), 0, 1);
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
//    ildaFrame.addPoly();
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
        
        if(!f.getExtension().compare("csv")){
            controlFile = f.getFileName();
            if(controlFile.compare(""))
                loadMapping(ofBufferFromFile("parameterMappings/"+controlFile).getText(), false, true);
        }else{
            if(!f.getExtension().compare("xml")){
                loadMacro(f.getAbsolutePath());
            }else{
                if(!f.getExtension().compare("mp4") || !f.getExtension().compare("webm") || !f.getExtension().compare("mov")
                   || !f.getExtension().compare("avi")){
                    if(scenes[app->parameterMap[currentScene]]==cinemas[0])
                        cinemas[0]->loadDirectMovie(f.getFileName());
                    else{
                        if(scenes[app->parameterMap[currentScene]]!=cinema)
                            loadSoloScene(cinema);
                        cinema->loadDirectMovie(f.getFileName());
                    }
                }else{
                    if(!f.getExtension().compare("jpg") || !f.getExtension().compare("jpeg") || !f.getExtension().compare("gif")
                       || !f.getExtension().compare("png")){
                        if(scenes[app->parameterMap[currentScene]]==uzis[0]){
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
    sceneMutex.lock();
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
    logfile.close();
    
    if(tcpServer.isConnected())
        tcpServer.close();
    
    if(is_launchpad)
        for(int i=0;i<128;i++)
            launchpadOut.sendNoteOn(1, i, 0);
}

void eqkoscope::reset(){
    audioAutos.clear();
    timedAutos.clear();
    leapAutos.clear();
    oscAutos.clear();
    midiCCAutos.clear();
    midiNoteonAutos.clear();
    
    initParameters();
    initMidi();
    loadMacroMap();
    analyzeMacros();
    
    for(int i=0;i<N_PARAM;i++){
        deltaMap[i] = parameterMap[i];
    }
}



