//
//  shape.h
//  BORING_ANGEL
//
//  Created by Raphaël on 05/03/2015.
//
//

#ifndef BORING_ANGEL_shape_h
#define BORING_ANGEL_shape_h

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

#define POLY 0
#define LINES 1

using namespace std;

class Shape : public Scene
{
    
public:
    Shape(AbstractApp* app) : Scene(app, "shape"){
        sceneID = "shape";
 img.allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR);
        fbo.allocate(WIDTH, HEIGHT);
    }
    
    void setup(){
    }
    
    void draw(){
        
        //ofFbo fbo;
//        double cxmin=-10;
//        double cxmaax=15;
//        double cymin=-20;
//        double cymaax=20; //the rect to draw in the complex plane
//        unsigned int maax_iterations = 2;
//        std::size_t const ixsize = WIDTH;
//        std::size_t const iysize = HEIGHT;
//            for (std::size_t ix = 0; ix < ixsize; ++ix)
//                for (std::size_t iy = 0; iy < iysize; ++iy)
//                {
//                    std::complex<double> c(cxmin + ix/(ixsize-1.0)*(cxmaax-cxmin), cymin + iy/(iysize-1.0)*(cymaax-cymin));
//                    std::complex<double> z = 0;
//                    unsigned int iterations;
//                    
//                    for (iterations = 0; iterations < maax_iterations && std::abs(z) < 2.0; ++iterations)
//                        z = z*z + c;
//                    ofSetColor((iterations == maax_iterations) ? ofColor::white : ofColor::black);
//                    ofRect(ix, iy, 1, 1);
//                }
        
        /*ofTranslate(0,0,-z);
        switch(renderMode){
            case POLY:{
                ofSetPolyMode(OF_POLY_WINDING_NONZERO);
                ofBeginShape();
                ofSetColor(ofColor::white);
                ofFill();
                for(int i=0;i<pts.size();i++){
                    ofVec3f p1 = pts[i];
                    for(int j=i;j<pts.size();j++){
                        ofVec3f p2 = pts[j];
                        ofVertex(p2);
                        float d = dist(p1.x,p1.y,p2.x,p2.y);
                         //if(d > 50 && d < 250){
                         //ofLine(p1.x,p1.y, p2.x, p2.y);
                         //}
                    }
                }
                ofEndShape();
            }break;
            case LINES:{
                ofSetColor(ofColor::white);
                for(int i=0;i<pts.size();i++){
                    ofVec3f p1 = pts[i];
                    for(int j=i;j<pts.size();j++){
                        ofVec3f p2 = pts[j];
                        float d = dist(p1.x,p1.y,p2.x,p2.y);
                        if(d > 50 && d < 250){
                            ofLine(p1.x,p1.y, p1.z, p2.x, p2.y, p2.z);
                        }
                    }
                }
            }break;
        }
        
        for(int i=0;i<l1.size();i++){
            ofVec3f p1 = l1[i];
            ofVec3f p2 = l2[i];
            ofLine(p1.x,p1.y, p1.z, p2.x, p2.y, p2.z);
        }*/
    }

    
    void capture(){
    }
    
    void mask(){
    }
    
    void update() {
//        z+=zSpeed;
        z-=zSpeed;

        for(int i=0;i<5;i++){
        int x = ofRandom(WIDTH);
        int y = ofRandom(HEIGHT);
        l1.push_back(ofVec3f(x, y, z));
        l2.push_back(ofVec3f(x, y, z-ofRandom(50,150)));
        }

        while(pts.size()>maxPts)
            pts.erase(pts.begin());
        
        while(l1.size()>maxLines){
            l2.erase(l2.begin());
            l1.erase(l1.begin());
        }
        
    }
    
    void addPoints(vector<ofVec3f> al) {
        
    }
    
    
    void mouseDragged(int x, int y, int button){
        pts.push_back(ofVec3f(x,y, z-100));
        
    }
    
    void touchMoved(ofTouchEventArgs &touch){
        
    }
    
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
        return &img;
    }
    
    void setImage(ofImage*){
    }
    
    void keyPressed(int key){};
    void oscEvent(std::string header, std::string arg){}
    void oscEvent(std::string header, std::vector<float> args){
        if(args.size()<3)
            return;
        if(!header.compare("/Leap/HandL/Pos") || !header.compare("/Leap/HandR/Pos")){
            sample++;
            if(sample%sampleRate == 0)
                pts.push_back(ofVec3f(WIDTH/2 + WIDTH/4*args[0],HEIGHT-HEIGHT/2*args[1], z));
        }
    }
    
    float dist(float x, float y, float x1, float y1){
        return sqrt((x-x1)*(x-x1) + (y-y1)*(y-y1));
    }
    
    void saveMacro(stringstream *xml){
        (*xml) << "<shape>" << endl;
        
        saveParameter(xml, "sampleRate", sampleRate);
        saveParameter(xml, "sample", sample);
        saveParameter(xml, "maxPts", maxPts);
        saveParameter(xml, "renderMode", renderMode);
        saveParameter(xml, "z", z);
        saveParameter(xml, "zSpeed", zSpeed);

        (*xml) << "</shape>" << endl;
    }
    
    
    void loadMacro(ofXml *xml){

        xml->getFloatValue("sampleRate");
        xml->getFloatValue("sample");
        maxPts = xml->getFloatValue("maxPts");
        renderMode = xml->getFloatValue("renderMode");
        z = xml->getFloatValue("z");
        zSpeed = xml->getFloatValue("zSpeed");
 }
    
    std::string getInfo(){
        return "";
    }
    
    void exit();
    
private:
    ofImage img;
    ofFbo fbo;
    
    vector<ofVec3f> pts;
    
    vector<ofVec3f> l1,l2;
    int sampleRate = 3;
    int sample = 0;
    
    int maxPts = 8;
    int maxLines = 1000;

    
    int renderMode = LINES;
    
    int z = 0;
    int zSpeed = 20;
    
};

#endif

