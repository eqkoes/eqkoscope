//
//  Mapped.h
//  BORING_ANGEL
//
//  Created by Raphaël on 23/09/2015.
//
//

#ifndef __BORING_ANGEL__Mapped__
#define __BORING_ANGEL__Mapped__

#include <iostream>
#include "scene.h"

#define WHITE 0
#define BLINK 1
//#define STARS 2
#define LINERAND 3
#define LINESYNC 4
#define GLITCH 5
#define RANDCOLOR 6
#define BREATH 7
#define ELLIPSE 8
#define NESTED 9
#define EXPAND 10
#define FILL_SEQ 11

#define NO_SYNC 0
#define SYNC_ALL 1
#define SYNC_INDIV 2
#define SYNC_SEQ 3

class Mapped: public Scene
{
public:
    Mapped(AbstractApp* app):Scene(app, "mapped"){ this->app = app;
        load();
        
        mainFbo.allocate(WIDTH,HEIGHT);
        
        for(int i=0;i<maps.size();i++){
            fillingFbos.push_back(ofFbo());
            fillingFbos[i].allocate(WIDTH,HEIGHT);
        }
        
        for(int i=0;i<maps.size();i++){
            ofFbo f;
            f.allocate(WIDTH,HEIGHT,GL_RGBA);
            mapFbos.push_back(f);
            transitions.push_back(0);
        }
        maskShader.load("../shaders/mask");
        i.allocate(WIDTH,HEIGHT, OF_IMAGE_COLOR);
    }
    
    void setup(){
    }
    
    void exit(){
    }
    
    //redraw on event
    void draw(){
        if((app->parameterMap[map_pace]>=0) || app->parameterMap[map_event]){
            app->parameterMap[map_event] = 0;
            
            ofPushMatrix();
            mainFbo.begin();
            
            int n = (app->parameterMap[map_sync] == SYNC_INDIV) ? maps.size() : 1; //number of fill fbos
            for(int i=0;i<n;i++){
                fillingFbos[i].begin();
                if(nestedScene!=0)
                    nestedScene->draw();
                else
                    renderShape(app->parameterMap[map_mode], i);
                fillingFbos[i].end();
            }
            
            for(int m=0;m<maps.size();m++){
                mapFbos[m].begin();
                ofPushMatrix();
                if(app->parameterMap[map_sync]!=SYNC_ALL)
                    ofTranslate((mapsBoxes[m].x+(mapsBoxes[m].width)/2), (mapsBoxes[m].y+(mapsBoxes[m].height)/2));
                else
                    ofTranslate(WIDTH/2, HEIGHT/2);
                if(app->parameterMap[map_sync]==SYNC_INDIV){
                    ofScale((float) mapsBoxes[m].width / (float) WIDTH,  (float) mapsBoxes[m].height / (float) HEIGHT);
                }
                fillingFbos[(app->parameterMap[map_sync]!=SYNC_INDIV) ? 0 : m ].draw(-WIDTH/2,-HEIGHT/2);
                ofPopMatrix();
                maskShader.begin();
                ofPushMatrix();
                ofTranslate(WIDTH/2, HEIGHT/2);
                maps[m].draw(-WIDTH/2, -HEIGHT/2, WIDTH, HEIGHT);
                ofPopMatrix();
                maskShader.end();
                mapFbos[m].end();
            }
            
            ofSetColor(ofColor::white);
            
            ofBackground(0, 0, 0);
            ofTranslate(WIDTH/2, HEIGHT/2);
            ofScale(scale,scale);
            ofEnableBlendMode(OF_BLENDMODE_ADD);
            for(int m=0;m<maps.size();m++)
                mapFbos[m].draw(-WIDTH/2,-HEIGHT/2);
            ofEnableBlendMode(OF_BLENDMODE_ALPHA);
            
            mainFbo.end();
            ofPopMatrix();
        }
        
        ofBackground(0);
        ofTranslate(app->parameterMap[mediaX]*WIDTH, -app->parameterMap[mediaY]*HEIGHT, app->parameterMap[mediaZ]*HEIGHT);
        ofRotateX(app->parameterMap[mediaRotX]);
        ofRotateY(app->parameterMap[mediaRotY]);
        ofRotateZ(app->parameterMap[mediaRotZ]);
        mainFbo.draw(0,0);
    }
    
    void renderShape(int shape, int n){
        switch(abs(shape)){
            case WHITE:case EXPAND:{
                ofBackground(255);
            }break;
            case BLINK:{
                ofBackground(ofRandom(1)<0.5?0:255);
            }break;
            case RANDCOLOR:{
                ofBackground(app->getRandomColor());
            }break;
            case LINERAND:{
                ofBackground(ofColor(0,0,0,(1-app->parameterMap[map_remanence])*255));
                ofSetColor(ofColor::white);
                ofRect(0, ofRandom(7*HEIGHT/8), WIDTH, ofRandom(HEIGHT/8));
            }break;
            case LINESYNC:{
                ofBackground(ofColor(0,0,0,(1-app->parameterMap[map_remanence])*255));
                ofSetColor(ofColor::white);
                ofRect(0, app->parameterMap[map_prog] * 7*HEIGHT/8, WIDTH, HEIGHT/8);
            }break;
            case GLITCH:{
                ofBackground(ofColor(0,0,0,(1-app->parameterMap[map_remanence])*255));
                for(int i=0;i<30;i++){
                    ofSetColor(ofRandom(1)<0.5?0:255);
                    int x = ofRandom(WIDTH/2);
                    int y = ofRandom(WIDTH/2);
                    ofRect(x, y, ofRandom(WIDTH-x), ofRandom(HEIGHT-y));
                }
            }break;
            case BREATH:{
                ofBackground(0);
                ofSetColor(ofColor::white);
                float w = app->parameterMap[map_prog]*(25+(app->parameterMap[map_sync]==NO_SYNC?n*3:0)) * WIDTH;
                ofRect(WIDTH/2-w/2, 0, w, HEIGHT);
            }break;
            case ELLIPSE:{
                ofBackground(0);
                ofSetColor(ofColor::white);
                float t;
                switch((int) app->parameterMap[map_sync]){
                    case NO_SYNC: //fake desync
                        t = (n/(float)maps.size() + app->parameterMap[map_prog]) * WIDTH;
                        break;
                    default:
                        t = app->parameterMap[map_prog] * WIDTH;
                }
                ofEllipse(WIDTH/2, HEIGHT/2, t, t);
            }break;
            case NESTED:{
                ofBackground(0);
                ofSetColor(ofColor::white);
                if(nestedScene!=0)
                    nestedScene->draw();
            }break;
            case FILL_SEQ:{
                ofBackground(0);
                ofSetColor(ofColor::white);
                if(app->parameterMap[map_prog] > n/(float)(maps.size()+1))
                    ofRect(0, 0, WIDTH, HEIGHT);
            }break;
        }
    }
    
    
    void mask(){}
    void capture(ofFbo* fbo){}
    void update(){
        if(app->parameterMap[map_pace] > 0)
            app->parameterMap[map_prog] += app->parameterMap[map_pace];
        else
            app->parameterMap[map_prog] += app->parameterMap[map_event] * 1 / (float) (maps.size());
        
        app->parameterMap[map_prog] = fmod(app->parameterMap[map_prog], 1);
        
        app->deltaMap[map_prog] = app->parameterMap[map_prog];
        
        if(app->parameterMap[map_mode]==NESTED && nestedScene!=0)
            nestedScene->update();
    }
    
    void randomEvent(){
        
    }
    
    void load(){
        ofDirectory dir("maps/");
        dir.listDir();
        for(int i=0;i<dir.numFiles();i++){
            ofImage img;
            int x=10000,y=10000,w=-1,h=-1;
            string picpath = dir.getPath(i);
            if(picpath.find(".jpg")!=string::npos || picpath.find(".png")!=string::npos || picpath.find(".gif")!=string::npos||picpath.find(".JPG")!=string::npos || picpath.find(".PNG")!=string::npos || picpath.find(".GIF")!=string::npos
               || picpath.find(".tiff")!=string::npos){
                img.loadImage(picpath);
                cout << "loaded " << picpath << endl;
                for(int xx=0;xx<img.width;xx++){
                    for(int yy=0;yy<img.height;yy++){
                        if(img.getColor(xx,yy).getBrightness()>15){
                            x = min(x,xx);
                            y = min(y,yy);
                        }
                    }
                }
                for(int xx=img.width-1;xx>=0;xx--){ //analysis (size)
                    for(int yy=img.height-1;yy>=0;yy--){
                        if(img.getColor(xx,yy).getBrightness()>15){
                            w = max(w, xx);
                            h = max(h, yy);
                        }
                    }
                }
                maps.push_back(img);
                mapsBoxes.push_back(ofRectangle(x,y,w-x,h-y));
            }
        }
    }
    
    void mouseDragged(int x, int y, int button){}
    
    void touchMoved(ofTouchEventArgs &touch){
    }
    
    void keyPressed(int key){
        switch(key){
            case ' ':{
                app->parameterMap[map_mode]  = (int)ofRandom(11);
            }break;
            case ':':{
                app->parameterMap[map_sync]  = ((int)app->parameterMap[map_sync] + 1) % 3;
            }break;
            case ';':{
                app->parameterMap[map_event] = 1;
            }break;
            case '-':scale-=0.05;break;
            case '_':scale+=0.05;break;
        }
        
    }
    
    void midiEvent(ofxMidiMessage& eventArgs){
        switch(eventArgs.channel){
            case 10:{
                if(eventArgs.status==MIDI_NOTE_ON){
                    switch(eventArgs.pitch){
                        case 20:case 60:
                            app->parameterMap[map_event] = 1;
                            break;
                    }
                }
            }break;
            case 3:{
                if(eventArgs.status==MIDI_NOTE_ON){
                    int pitch = eventArgs.pitch;
                    switch(pitch){
                        case 48:{
                            int tmpSync = app->parameterMap[map_sync] ;
                            do{
                                app->parameterMap[map_sync]  = int(ofRandom(3));
                            }while(app->parameterMap[map_sync]!=tmpSync);
                        }break;
                        default:
                            if(pitch>=36 && pitch <48){
                                app->parameterMap[map_mode]  = pitch - 36;
                            }
                    }
                }
                if(eventArgs.status==MIDI_CONTROL_CHANGE){
                    int ctrl = eventArgs.control;
                    float value = eventArgs.value;
                    switch(ctrl){
                        case 1: app->parameterMap[map_pace] = (value/127.0f); break;
                        case 2: app->parameterMap[map_remanence]  = value/127.0f; break;
                        case 12: app->parameterMap[map_sync]  = int(value/127.0f * 3); break;
                    }
                }
            }
            }
        }
        void oscEvent(std::string header, std::string arg){}
        void oscEvent(std::string header, std::vector<float> args){}
        
        void saveMacro(stringstream *xml){
            
            (*xml) << "<mapped>" << endl;
            
            
            (*xml) << "</mapped>" << endl;
            
        }
        void loadMacro(ofXml *xml){
        }
        
        bool isBackground(){return false;}
    
        std::string getInfo(){return "";}
        
    Scene* nestedScene = 0;
    
    private:
        int MAX_Z = 2500;
        int speed = 10;
        int MAX_STARS = 1500;
        int size = 4;
        float scale = 1;
        
        std::vector<ofImage> maps;
        std::vector<ofRectangle> mapsBoxes;
        std::vector<ofFbo> fillingFbos;
        std::vector<ofFbo> mapFbos;
        ofShader maskShader;
        ofFbo mainFbo;
        
        std::vector<float> transitions; //TODO
        ofImage i;
    
    };
    
#endif /* defined(__BORING_ANGEL__Mapped__) */
