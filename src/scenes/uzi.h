//
//  uzi.h
//  emptyExample
//
//  Created by Raphaël on 05/11/2014.
//
//

#ifndef emptyExample_uzi_h
#define emptyExample_uzi_h

#include "ofxMidi.h"
#include "scene.h"
#include "utils.h"
#include <ofBaseTypes.h>
#include <iostream>
#include <ofFbo.h>
#include <vector>
#include <string>
#include "ofMesh.h"
#include "distortion.h"

#define SWAP_TINT_PEAKS 5

using namespace std;

class Uzi : public Scene
{
    
public:
    Uzi(AbstractApp* app);
    
    void setup(){};
    
    void exit(){}
    
    void draw();
    
    void capture(ofFbo* fbo){
        
    }
    
    void mask(){
        if (app->parameterMap[backMask])
            circleMask.draw(0,0, WIDTH, HEIGHT);
    }
    
    void loadImage(string path);
    
    void update();
    
    void mousePressed(int x, int y, int button){}
    void mouseDragged(int x, int y, int button){}
    void mouseMoved(int x, int y){}
    
    void touchMoved(ofTouchEventArgs &touch){}
    
    void midiEvent(ofxMidiMessage& eventArgs);
    
    void uziSwitch();
    
    void manualSwitch();
    void manualNext();

    void load();
    
    void loadDirectImage(string path);
    
    void keyPressed(int key);
    
    void oscEvent(std::string header, std::string arg){}
    void oscEvent(std::string header, std::vector<float> args){}
    
    void saveMacro(stringstream *xml);
    void loadMacro(TiXmlHandle *xml);
    
    string getCurrentMedia(){
        return currentMedia;
    }
    
    void setDirty(){
        imgDirty = true;
    }
    
    std::string getInfo();
    
    void setResolution(){
        delete source;
        delete image;
        
        source = new ofFbo();
        image = new ofImage;
        
        source->allocate(WIDTH,HEIGHT, GL_RGBA);
        image->allocate(WIDTH,HEIGHT, OF_IMAGE_COLOR);
    }
    
    void loadMesh(string path);

    void loadFromPreloadedImgs();

private:
    ofMutex imgMutex;

    ofFbo* source;
    ofImage depthMap;
    ofImage img;
    ofImage circleMask;
    string currentMedia;

    ofImage depth;


    //VIDEO
    std::vector<std::vector<string> > strdb;
    std::vector<std::vector<ofImage*> > imgdb;
    ofImage *image=0;
    ofImage* rawImage=0;
    int dbIndex = 0;
    int imgIndex = 0;
    
    //MESH
    std::map<std::string, ofMesh> meshdb;
    std::map<ofMesh*, ofImage > texturedb;
    ofMesh* currentMesh = 0;
    ofImage* currentTexture = 0;

    
    ofImage roundMaskImg;
    
    float swapTintsIn[255];
    float swapTintsRef[255];
    float swapTintPeaksIn[SWAP_TINT_PEAKS];
    float swapTintPeaksRef[SWAP_TINT_PEAKS];
    float swapTintMap[255];
    ofShader swapTintShader;
    
    bool directChange = false;
    
    bool imgDirty = true;
    bool scheduleLoad = true;
    
    int lastSwitchFrame = 0;
    int lastSwitchLimit = 3;
    
    ofShader depthShader, blendShader;
    
    std::vector<ofVec4f> zs;
    ofImage tripImg;
    
    string clipboardRawContents = "";
    
    bool preload = true;
    map<string, ofImage> preloadedImages;
};

#endif
