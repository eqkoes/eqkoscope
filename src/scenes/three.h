//
//  three.h
//  eqkoscope
//
//  Created by Raphaël on 02/12/2015.
//
//

#ifndef eqkoscope_three_h
#define eqkoscope_three_h

#include "abstractapp.h"
#include "scene.h"
#include <ofMesh.h>
#include <ofTypes.h>

using namespace std;

class Three: public Scene
{
public:
    Three(AbstractApp* app)
    :Scene(app, "three"){
        sceneID = "three";
 cam = ofEasyCam();
        
        for(int i=0;i<1000;i++){
            points.push_back(ofVec3f(ofRandom(-1,1),ofRandom(-1,1),ofRandom(-1,1)));
            lines.push_back(ofVec3f(ofRandom(-1,1),ofRandom(-1,1),ofRandom(-1,1)));
            lines.push_back(ofVec3f(ofRandom(-1,1),ofRandom(-1,1),ofRandom(-1,1)));
        }
        
        sphere = ofMesh::icosphere(5);
        
    }
    
    void setup(){
        
    }
    void exit(){
        
    }
    void draw(){
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        ofSetColor(0,0,0);
        ofRect(0,0,WIDTH,HEIGHT);
        
        ofTranslate(WIDTH/2, HEIGHT/2);
        
        cam.setPosition(ofPoint(0,0,zoom));
        cam.setOrientation((ofVec3f((ofGetMouseY()-HEIGHT/2)/3.0, -(ofGetMouseX()-WIDTH/2)/3.0, 0)));
        cam.begin();
        ofSetColor(ofColor(255,255,255,255));
        for(int i=0;i<points.size();i++){
            ofPushMatrix();
            ofSetColor(ofColor(ofRandom(255)));
            ofTranslate(HEIGHT/2*points[i].x, HEIGHT/2*points[i].y, HEIGHT/2*points[i].z);
            sphere.draw();
            ofPopMatrix();
        }
        for(int i=0;i<lines.size();i++){
            ofSetColor(ofColor(ofRandom(255)));
            ofLine(HEIGHT/2*lines[i*2].x, HEIGHT/2*lines[i*2].y, HEIGHT/2*lines[i*2].z,HEIGHT/2*lines[i*2+1].x, HEIGHT/2*lines[i*2+1].y,HEIGHT/2*lines[i*2+1].z);
        }
        cam.end();
        
    }
    void mask(){}
    void capture(ofFbo* fbo){}
    void update(){
        zoom += 0.5;
    }
    
    void setImage(ofImage*){}
    ofImage* getImage(){return 0;}
    
    void mouseDragged(int x, int y, int button){
       // zoom += (y-HEIGHT/2) / 10.0;
    }
    
    void touchMoved(ofTouchEventArgs &touch){
        
    }
    
     void keyPressed(int key){
         switch(key){
     case 357:
         zoom += 1;
         break;
     case 359:
         zoom -= 1;
         break;
         }
     }
    
     void midiEvent(ofxMidiMessage& eventArgs){}
     void oscEvent(std::string header, std::string arg){}
     void oscEvent(std::string header, std::vector<float> args){}
    
     void saveMacro(stringstream *xml){}
     void loadMacro(ofXml *xml){}
    
    std::string getInfo(){
        return "";
    }
    
private:
    vector<ofVec3f> points;
    vector<ofVec3f> lines;
    ofMesh sphere;
    ofEasyCam cam;
    float zoom = 0;
};

#endif
