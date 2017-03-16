//
//  Scene.h
//  emptyExample
//
//  Created by Raphaël on 27/10/2014.
//
//

#ifndef emptyExample_Scene_h
#define emptyExample_Scene_h

#include <ofBaseTypes.h>
#include <string>
#include "AbstractApp.h"

class Scene{
    
public:
    
    Scene(AbstractApp* app, std::string id){ this->app = app;
        this->sceneID==id;}
    void initParameters(){
    }
    virtual void setup()=0;
    virtual void exit()=0;
    virtual void draw()=0;
    virtual void mask()=0;
    virtual void capture(ofFbo* fbo)=0;
    virtual void update()=0;
    
    virtual void mouseDragged(int x, int y, int button)=0;
    virtual void touchMoved(ofTouchEventArgs &touch)=0;
    virtual void keyPressed(int key)=0;
    
    virtual void midiEvent(ofxMidiMessage& eventArgs)=0;
    virtual void oscEvent(std::string header, std::string arg)=0;
    virtual void oscEvent(std::string header, std::vector<float> args)=0;
    
    virtual void saveMacro(stringstream *xml)=0;
    virtual void loadMacro(ofXml *xml)=0;
    
    virtual std::string getInfo()=0;
    
    bool isBackground(){return back;}
  
    AbstractApp* app;
    bool back=true;
    
    std::string sceneID = "";
    
//    std::map<std::string, float> parameterMap;
//    std::map<std::string, float> deltaMap;
};

#endif
