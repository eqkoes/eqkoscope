//#pragma once

#ifndef OFAPP_
#define OFAPP_

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxOsc.h"
#include <ofBaseTypes.h>
#include "utils.h"
#include "feedback.h"
#include "print.h"
#include "lines.h"
#include "cinema.h"
#include "uzi.h"
#include "pointback.h"
#include "scene.h"
#include "distortion.h"
#include "draw.h"
#include "freezer.h"
#include "fractal.h"
#include "shape.h"
#include "glitch.h"
#include "AbstractApp.h"
#include <sstream>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <ofXml.h>
#include <algorithm>
#include <vector>
#include "three.h"
#include "agents.h"
#include "Mapped.h"
#include "ofxGifEncoder.h"
#include "auto.h"
#include "ofSerial.h"
#include "LedStrip.h"
#include <GLFW/glfw3.h>
#include <ofxTCPClient.h>
#include <ofxTCPServer.h>
#include "simple_midi_reader.h"
//#include "ofxGrt.h"
#include "genetic.h"
#include "ofxVideoRecorder.h"
#include "ofxGui.h"


#ifdef INSTALL_THOUGHTS
#include "thoughts.h"
#endif

#ifdef KINECT
#include "ofxKinect.h"
#endif

#ifdef LEAPMOTION
#include "ofxLeapMotion.h"
#endif

#ifdef LAZERS
//#include "ofxLaserManager.h"
#include "ofxIldaFrame.h"
#include "ofxEtherdream.h"
#endif

class PSlider : public ofxSlider<float>
{
public:
    PSlider(AbstractApp* app, int p) : ofxSlider(){
        this->app = app;
        param = p;
        setDefaultFillColor(ofColor(0,255,255,100));
        setDefaultBackgroundColor(ofColor(0,0,0,100));
    }
    bool mouseDragged(ofMouseEventArgs &args){
        ofxSlider::mouseDragged(args);
        app->deltaMap[param] = value.get();
    }
    AbstractApp* app;
    int param;
};

class eqkoscope : public ofBaseApp,  public ofxMidiListener, public AbstractApp
{
public:
    
    void setup();
    void reset();
    
    void loadMapping(std::string str, bool factory, bool erase);
    void eraseControlMapping(bool factory);
    void addCommand(string cmd, bool factory);
    void loadConfig();
    void loadMacroMap();
    void loadShaders();
    
    void update();
    void updateSerial();
    
    void draw();
    
    void applyFXChain(ofFbo* srcFbo, ofFbo* curFbo);
    void displayInfo(int w, int h);
    void doPaint();
    
    void paintPass(int resolution, float minSize, float maxSize,ofPixels* pix);
    void paintPass3(int resolution, float minSize, float maxSize,ofPixels* pix);
    void paintPass2(int resolution, float minSize, float maxSize, float randSize, ofPixels* pix);
    
    void swapFBOs(ofFbo* a, ofFbo* b);
    
    void keyPressed(int key);
    void keyReleased(int key);
    void updatePromptValue();
    
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseScrolled(int x, int y);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h){}
    void dragEvent(ofDragInfo dragInfo);
    void touchMoved(ofTouchEventArgs &touch);
    
    void newMidiMessage(ofxMidiMessage& eventArgs);
    void saveMacro(string path);
    void saveCurrentMacro();
    void loadMacro(string path);
    
    /** MIDI **/
    void initMidi();
    void sendControlChange(int channel, int control, int value);
    void liveMidiEffect(int code, float intensity, bool on);
    void updateMidi();
    void resetMidi();
    void openPorts();
    void initOfflineMIDI();
    void updateOfflineMIDI();
    
    /** MACROS **/
    void parseMidi(ofxMidiMessage eventArgs);
    void parseMacro(string macro);
    void loadNextMacro();
    void loadFirstMacro();
    void loadPrevMacro();
    void reloadMacro();
    void setMacroPage(int page);
    string getMacroFromMIDI(int pitch);
    void analyzeMacros();
    void paintMacros();
    void updateMacros();
    void setMacroCode(int c);

    /** AUDIO **/
    void initAudio();
    void audioRequested(float * output, int bufferSize, int nChannels);
    void audioReceived(float *input, int bufferSize, int nChannels);
    void analyzeAudio();
    bool detectCircle(int x, int y, int rad);
    bool detectLine(int x1, int y1, int x2, int y2);
    
    /** SCENE LOAD **/
    void loadScene(Scene* s, int layer);
    void loadSoloScene(Scene* s);
    
    void doStressTest();
    
    void exit();
    
    /** OSC **/
    void initOSC();
    void manageOSC();
    bool manageOSCParam(ofxOscMessage *m);
    
    void oscout(std::string head, float value);
    void oscout(std::string head, std::string value);
    void oscout(std::string head, vector<float> value);
    void oscout(std::string head, string values[], int length);
    
    void updateLeap();
    
    void tapTempo();
    
    void displayDualScenes();
    void applyPostProcess();
    void applyMask();
    
    void setResolution(int i);
    
    void niceRandom(int x);
    
    void parameterChanged(int & circleResolution);

protected:
    /** MACROS **/
    bool saveMacros = false;
    string saveMacroStr = "";
    std::vector<string> pendingMacros;
    ofMutex macroMutex;
    int currentMacroCode = -1;
    std::string currentMacroStr = "";

    
    /** AUDIO **/
    bool analyzeAudioB = false;
    int soundBufferOffset = 0;
//    ofImage i, grayi, audioImg;
    float slowrms=0;
    float fastrms=0;
    float audioEasing = 0.2;
    float audioFilter = 1;
    float currentRms[10]; //max stereo
    
    int nbFramesSinceAudioStart = 0;
    
    std::stringstream stderrBuffer;
    
    /** MIDI **/
    string MIDIMapPath = "MIDIMap.csv";
    
    std::vector<ofxMidiIn> midiIns;
    ofxMidiMessage midiMessage;
    std::vector<ofxMidiMessage> midiMsgs;
    ofxMidiOut launchpadOut;
    
    bool is_mpd24 = false;
    bool is_launchControl = false;
    bool is_io2 = false;
    bool is_external_device = false;
    bool is_novation = false;
    bool is_launchpad = false;
    bool is_nano = false;
    
    bool enforce_mpd24 = false;
    bool enforce_launchControl = false;
    bool enforce_io2 = false;
    bool enforce_external_device = false;
    bool enforce_novation = false;
    bool enforce_launchpad = false;
    bool enforce_nano = false;

    /** SCENES **/
    std::vector<Scene*> scenes;
    std::vector<Scene*> allScenes;
    PointBack* pointback;
    Feedback* feedbackScene;
    Fractal* fractal;
    Cinema* cinema;
    Uzi* uzi;
    Shape* shape;
    Draw* drawscene;
    Three* three;
    Lines* lines;
    Print* print;
    Mapped* mapped;
    Glitch* glitch;
    Agents* agents;
    
    /** FREEZER */
    Freezer* freezer;
    
    /** SHADERS **/
    ofShader testShader, blendShader;
    ofShader trapezeShader;
        
    ofShader alphaShader,hueShader,threedeeShader,gradientShader,iThreshShader, threshShader,hBackShader,pixelHShader, lineShader, sharpenShader, alphaWhiteShader, saturationShader,disposeShader,hueShiftShader,maskShader, mandalaShader,squareMandalaShader,hueCrazeShader, hueFilterShader, psyShiftShader, chromaPointShader,bwShader;
    
    /** DATA **/
    ofFbo fbo,fbo2, tempFbo, tempFbo2, psyFbo;
    ofFbo* curFbo, *srcFbo;
    ofFbo *grabFbo;
    std::vector<ofFbo> sceneFbos;

    ofShader squareShader;
    
    /** OSC */
    ofxOscReceiver receiver;
    ofxOscSender sender;
    std::vector<ofVec3f> kinectPoints;
    ofMesh kinectMesh, kinectMesh2, mesh;
    ofImage kinectMask;
    std::vector<ofVec2f> oldVectors;
    
    ofThread dbVideoMutex;
    
    bool texturing = false;
    bool cinemaBackground = false;
    bool cinemaTexture = false;
    
    ofFile logfile;
    
    bool newFeatures = true;
    
    ofImage roundMaskImg,interMask,mappingMask;
    
    bool loadColorWithMacro = true;
        
    bool black = false;

    float modWheel = 0;
    bool verticalPitchBend = false; //orientation of pitch bend effect
    
    float kinectGrid[640][480];
    
    ofVbo vbo;
    
    float yyy=0;
    
    bool automode = false;
    bool autoOnsets = false;

    bool fullscreen = liveMode;
    
    ofEasyCam cam;
    float posX = 0, posY = 0;
    
    //BPM
    std::vector<unsigned long long> lastTaps;

    bool test = true;
    
    std::vector<ofFbo*> paintFbos;
    ofFbo paintFbo;
    
    std::vector<ofImage*> brushstrokes;
    ofPixels pix;
    
    Utils *utils;
    
    vector<ofFbo> pastFbos;
    int nbPastFbos = 100;
    ofFbo copyFbo;
    

    int GIF_WIDTH = crt_WIDTH;
    int GIF_HEIGHT = crt_HEIGHT;
    int gifIndex = 0;
    bool isGifRecording = false;
    
    
    // !!!

    bool BYPASS_ALL_PARAMETERS = false;

    

    /** STRESS TEST **/
    std::vector<ofxMidiMessage> stressPending;
    
    std::vector<Cinema*> cinemas;
    std::vector<Uzi*> uzis;
    bool centerTrium = true;
    
    ofMutex sceneMutex;
    ofMutex midiMutex;
    vector<vector<int> > midiControlValues;
    
    ofTrueTypeFont font, font2;
    
    bool displayFrameRate = false;
    
    int coffseta = 0;
    
    // LEAP
#ifdef LEAPMOTION
    ofxLeapMotion leap;
    vector<float> leapX,leapY,leapZ,leapDX,leapDY,leapDZ,leapRoll;
    vector <ofxLeapMotionSimpleHand> simpleHands;
    vector <int> fingersFound;
    float leapMeanX = 0;
    float leapMeanY = 0;
    float leapMeanZ = 0;
#endif
    
    bool musicMode = false;
        
    int nbKicks = 0;
    vector<float> kickDates;
    bool saveFrame = false;
    
    /** PROMPT **/
    bool isPrompt = false;
    bool isPromptValue = false;
    std::string promptStr = "";
    std::string quickPromptStr = "";
    int quickPrmptIndex = 0;
    std::string promptValue = "";
    std::string lastPromptStr = "";
    float lastPromptValue = 0;
    
    /** SPECIAL **/
    
    bool stressTestFilter = true;
    Genetic* genetic;
    
    bool debugMapping = false;
        
    
    std::string macroPath = "";
    int leapXParameter = -1;
    int leapYParameter = -1;
    int leapZParameter = -1;
    int leapDXParameter = -1;
    int leapDYParameter = -1;
    int leapDZParameter = -1;
   int leapRollParameter = -1;
    
    int assignChannel = 3;
    int assignCtrl = 0;
    
    
    bool skewMidiMode = false;
    
    map<int,string> macroMap;
    
    vector<ofFbo*> echoFbos;
    int MAX_NB_ECHOES = 75;
    int currentEcho = 0;
    int lastEcho = 0;


    float drawMicros, updateMicros, autoMicros;
    float microsEasing = 0.03;
    
    float audioCapture = 480;
    
    ofMutex srcFboMutex;
    
    int macroPage = 0;
    string macroNames[128*MAX_MACRO_PAGES];
    float macroMorphing = 0;
    float macroMorphEvo = 1;
    float macroMorphParameters[N_PARAM];

    ofShader fxShader;
    
    vector<Auto*> audioAutos;
    vector<Auto*> oscAutos;
    vector<Auto*> midiNoteonAutos;
    vector<Auto*> midiCCAutos;
    vector<Auto*> leapAutos;
    vector<Auto*> timedAutos;
    
    vector<Auto*> factorymidiNoteonAutos;
    vector<Auto*> factorymidiCCAutos;
    
    vector<Auto*> ccAutoFastMap[16][127];
    vector<Auto*> noteAutoFastMap[16][127];
    
    string autoComments;


#ifdef LAZERS
//    ofxLaser::Manager laser;
    
    ofxEtherdream etherdream;
#endif
    
    LedStrip *strip = 0;

    bool analyzeImg = false;

    bool kinectRunning = false;
    
    
    GLFWwindow* secondDisplay=0, *mainDisplay=0;

/** TCP **/
    ofxTCPClient tcpClient;
    ofxTCPServer tcpServer;
    ofxMidiMessage tcpMIDIMsg;
    
    /** MIDI **/
    vector<ofxMidiMessage> offlineMsgs;
    vector<double> offlineMsgDates;
    double offlineDate = 0;
    int offlineMsgsIndex = 0;
    
    
    /** HACK **/
    float lastPerturbation = 0;
    
#ifdef KINECT
    ofxKinect kinectDevice;
    float kinectCalibration[640][480];
#endif
    
#ifdef INSTALL_THOUGHTS
    Thoughts *thougts;
#endif
    
    int shadowFrames = 0;
    bool shadowFrame = false;
    long trueFrameNum = 0;
    
    /** VIDEO GRABBY THINGY **/
    int sampleRate;
    int channels;
    float dummyAudio[AUDIO_BUFFER_SIZE];
    ofxVideoRecorder vidRecorder;
    int vidRecorderIndex = 0;
    string fileExt = ".mov"; // ffmpeg uses the extension to determine the container type. run 'ffmpeg -formats' to see supported formats
    bool hasTakenVideo = true;
    
    ofxPanel gui;
    std::vector<PSlider*> sliders;
    bool guiVisible = false;

};

#endif
