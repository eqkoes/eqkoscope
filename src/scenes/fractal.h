//
//  fractal.h
//  eqkoscope
//
//  Created by Raphaël on 03/12/2016.
//
//

#ifndef eqkoscope_fractal_h
#define eqkoscope_fractal_h

#include "scene.h"

class Fractal: public Scene{
    
public:
    
    Fractal(AbstractApp* app)
    :Scene(app, "fractal"){
        sceneID = "fractal";

        fractalShader.load("../shaders/fractals");
        fbo.allocate(WIDTH, HEIGHT2);
        fbo2.allocate(WIDTH, HEIGHT2);
    }
    
    void setup(){
        
    }
    void exit(){}
    
    void draw(){
        ofSetColor(ofColor::black);
        ofRect(0,0,WIDTH,HEIGHT2);
        ofSetColor(ofColor::white);
        
        fractalShader.load("../shaders/fractals");
        
        fractalShader.begin();
        fractalShader.setUniform1f("param1", app->parameterMap[fractalP1]);
        fractalShader.setUniform1f("param2", app->parameterMap[fractalP2]);
        fractalShader.setUniform1f("scale", app->parameterMap[fractalScale]);
        fractalShader.setUniform1f("mode", (int)app->parameterMap[fractalMode]);
        fractalShader.setUniform1i("iter", (int)app->parameterMap[fractalIterations]);
        fbo.draw(0,0);
        fractalShader.end();
    }
    
    void mask(){
    }
    void capture(ofFbo* fbo){
    }
    
    void update(){
        
    }
    
    void mouseDragged(int x, int y, int button){
        
    }
    
    void touchMoved(ofTouchEventArgs &touch){
        
    }
    
    void keyPressed(int key){
    }
    
    void midiEvent(ofxMidiMessage& eventArgs){
        if(eventArgs.channel==1)
            return;
        
        float value = eventArgs.value;
        switch(eventArgs.status){
            case MIDI_CONTROL_CHANGE:{
                switch(eventArgs.control){
                }
            }
            default:;
        }
    }
    
    void oscEvent(std::string header, std::string arg){
        
    }
    void oscEvent(std::string header, std::vector<float> args){
        
    }
    
    void saveMacro(stringstream *xml){
        (*xml) << "<fractal>" << endl;
        
        (*xml) << "</fractal>" << endl;
    }
    
    void loadMacro(ofXml *xml){
    }
    
    std::string getInfo(){
        return "";
    }
    
    bool isBackground(){return back;}
    
    protected :
    ofShader fractalShader;
    ofFbo fbo,fbo2;
    
};


#endif
