#ifndef emptyExample_Cinema_h
#define emptyExample_Cinema_h

#include "ofxMidi.h"
#include "scene.h"
#include "utils.h"
#include <ofBaseTypes.h>
#include <ofPixels.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <ofUtils.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include "distortion.h"

using namespace std;


#define ALPHABLACK 0
#define SCREEN 1
#define TRUED 2
#define MULTIPLY 3
#define ADD 4
#define SUBTRACT 5
#define INCRUSTATION 6
#define PROLLAGE 7

class Cinema : public Scene
{
    
public:
    Cinema(AbstractApp* app);
    
    void setup(){
        triggerPlayLater = true;
    }
    
    void exit(){
        if(app->parameterMap[embedCinema]==0){
            unloadLater = true;
            for(int i=0;i<players.size();i++)
                if(players[i]!=0)
                    players[i]->stop();
        }
    }
    
    void draw();
    void randomJump();
    
    void capture(ofFbo* f){
//        remanentFbo.begin();
//        f->draw(0,0);
//        remanentFbo.end();
    }
    
    void mask(){ //post traitement
        ofSetColor(ofColor::white);
//        remanentFbo.draw(0, HEIGHT2, WIDTH, -HEIGHT2); //flip the image
        
//        if (app->parameterMap[backMask])
//            circleMask.draw(0,0, WIDTH, HEIGHT);
    }
    
    void update();
    
    void pause(bool b);
    void setMixer(int i, float f);
    void loadMovie(int player, int dbIndex, int i, bool randomPos);
    void mousePressed(int x, int y, int button){}
    void mouseDragged(int x, int y, int button){
    }
    void mouseMoved(int x, int y){}
    
    void touchMoved(ofTouchEventArgs &touch){
        
    }
    
    void setSleep(int s){
        sleep = s;
    }
    
    ofTexture* getTexture(){
        return &(players[0]->getTextureReference());
        //        return players[0]->getTexture();
    }
    
    
    
    void parseMidi(ofxMidiMessage eventArgs);
    
    void randomPosition(float r);
    void randomScene(string tag);
    void midiEvent(ofxMidiMessage& eventArgs){
        parseMidi(eventArgs);
    }
    
    void keyPressed(int key);
    void oscEvent(std::string header, std::string file){}
    
    void oscEvent(std::string header, std::vector<float> args){}
    
    void loadDirectMovie(string vidPath);
    
    void load();
    void saveMacro(stringstream *xml);
    
    void loadMacro(TiXmlHandle *xml);
    void setPosition(float f);
    std::string getInfo();
    
    void setResolution();
    //VIDEO
    std::vector<std::vector<std::string> > strdb;
    
    std::vector<ofVideoPlayer*> players;
    std::vector<string> playerPaths;
    std::vector<float> playerIntensities;
    std::map<std::string, std::vector<double> > playerScenes;
    std::map<std::string, std::vector<string> > playerScenesMovements;
    int lastSceneIndex = -1;
    int currentVidIndex = 0;
    int dbIndex = 0;
    int vidIndex = 0;
    
    ofShader circleShader;
    
    ofMutex videoMutex;
    
    bool unloadLater = false;
    float laterPosition = -1;
    vector<float> loadLaterMoviePosition;
    int loadLaterIndex = -1;
    
    ofShader alphaShader, contourShader, alphaWhiteShader, alphaScreenShader;
    ofFbo fbo;
    vector<ofFbo> pastFbos;
    int nbPastFbos = 50;
    
    bool triggerPlayLater = false;
    
    ofFbo copyFbo;
    
    int lastloadDate = -10;
    
    float updateMixerValue = 0;
    
    int sleep = 0;
    
    ofFbo* stillFbo;
};

#endif
