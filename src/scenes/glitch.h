//
//  glitch.h
//  eqkoscope
//
//  Created by Raphaël on 15/07/2016.
//
//

#ifndef eqkoscope_glitch_h
#define eqkoscope_glitch_h

#include "ofxMidi.h"
#include "scene.h"
#include "utils.h"
#include "math.h"
#include <ofBaseTypes.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <complex>

#define RECTS 0
#define LINES 1
#define PTs 2

using namespace std;

class Glitch : public Scene
{
    
public:
    Glitch(AbstractApp* app) : Scene(app, "glitch"){
        sceneID = "glitch";

    }
    
    void setup(){
    }
    
    void draw(){
        switch(int(app->parameterMap[glitch_mode] )){
            case RECTS:
                staticGlitch();
                break;
            case LINES:
                staticLines();
                break;
            case PTs:
                staticPoints();
                break;
        }
        ofSetColor(255);
    }
    
    void staticGlitch() {
        ofBackground(0);
        int res = 1;
        int x, y, h, w;
        for (int i=0;i<100;i++) {
            h = int(ofRandom(HEIGHT2/4));
            x = int(ofRandom(WIDTH));
            y = int(ofRandom(HEIGHT2));
            setRandomColor();
            
            if (x<WIDTH/2)
                ofDrawRectangle(x, y-h/2, WIDTH, h);
            else
                ofDrawRectangle(0, y-h/2, x, h);
        }
    }
    
    void staticLines() {
        ofBackground(0);
        int res = 1;
        int x, y, h, w;
        for (int i=0;i<150;i++) {
            h = HEIGHT;
            w = int(ofRandom(25));
            x = int(ofRandom(WIDTH));
            y = 0;
            setRandomColor();

                ofDrawRectangle(x-w/2, y, w, h);
        }
    }
    
    void staticPoints() {
        //ofBackground(0);
        int res = 1;
        int x, y, h, w;
        for (int i=0;i<1000;i++) {
            h = 5;
            w = 5;
            x = int(ofRandom(WIDTH));
            y = int(ofRandom(HEIGHT2));;
            setRandomColor();
            
            ofDrawRectangle(x-w/2, y-h/2, w, h);
        }
    }
    
    void setRandomColor(){
        if(app->parameterMap[glitch_thresh])
            ofSetColor(int(ofRandom(2)) * 255);
        else
            ofSetColor(ofRandom(255));
    }
    
    
    void capture(){
    }
    
    void mask(){
    }
    
    void update() {
    }
    
    void capture(ofFbo* fbo){}
    void mousePressed(int x, int y, int button){}
    void mouseDragged(int x, int y, int button){}
    void mouseMoved(int x, int y){}
 void touchMoved(ofTouchEventArgs &touch){}
    
    void midiEvent(ofxMidiMessage& eventArgs){
        float value = eventArgs.value;
        switch(eventArgs.status){
            case MIDI_NOTE_ON:{
                switch(eventArgs.pitch){
                    default:;
                }
            }
            case MIDI_NOTE_OFF:{
                switch(eventArgs.pitch){
                }
            }
            case MIDI_CONTROL_CHANGE:{
                switch(eventArgs.control){
                }
            }
            default:;
        }
    }
    
    ofImage* getImage(){
        return NULL;
    }
    
    void setImage(ofImage*){
    }
    
    
    void keyPressed(int key){
        switch(key) {
            case ' ':
                app->parameterMap[glitch_mode] = int(ofRandom(4));
                app->parameterMap[glitch_thresh] = int(ofRandom(2));
            break;
        }
    };
    void oscEvent(std::string header, std::string arg){}
    void oscEvent(std::string header, std::vector<float> args){
    }
    
    
    void saveMacro(stringstream *xml){
        (*xml) << "<glitch>" << endl;
        
        (*xml) << "</glitch>" << endl;
    }
    
    
    void loadMacro(TiXmlHandle *xml){
        
    }
    
    std::string getInfo(){
        return "";
    }
    
    void exit(){}
    
private:

    
};


#endif
