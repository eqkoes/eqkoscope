//
//  pointback.h
//  emptyExample
//
//  Created by Raphaël on 06/11/2014.
//
//

#ifndef emptyExample_pointback_h
#define emptyExample_pointback_h

#include "ofxMidi.h"
#include "Scene.h"
#include "Utils.h"
#include <math.h>
#include <ofGraphics.h>
#include <ofColor.h>
#include <ofBaseTypes.h>
#include <iostream>
#include "cinema.h"

#define CONNECT 0
#define CONCENTRIC 2
#define SHAPES 3
#define LINES 1
#define POINTS 4

using namespace std;

class PointBack : public Scene
{
    
public:
    PointBack(AbstractApp* app) : Scene(app, "pointback"){
        sceneID = "pointback";
 img.allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR);
        sobelShader.load("../shaders/sobel");
    }
    
    void setup(){
       // ofSetFrameRate(14);
    }
    
    void exit(){}
    
    void draw(){
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        ofSetColor(0,0,0);
        ofRect(0,0,WIDTH,HEIGHT);
        
        if(nestedScene!=0)
            pointsFromNestedScene();
//        else
//            add();
        
        ofTranslate(WIDTH/2+offx, HEIGHT/2+offy);//temp
        
        //RENDERING
        if (render == POINTS){
            ofSetColor(255);
            for (int i=0;i<pts.size();i++) {
                float x = pts.at(i).x*cos(pts.at(i).y);
                float y = pts.at(i).x*sin(pts.at(i).y);
                ofCircle(x, y, 4);
            }
        }
        if (render == CONNECT || render == LINES) {
            ofSetColor(255);
            for (int i=0;i<pts.size();i++) {
                float x = pts.at(i).x*cos(pts.at(i).y);
                float y = pts.at(i).x*sin(pts.at(i).y);
                if(render == CONNECT)
                    ofCircle(x, y, 4);
                for (int j=0;j<pts.size();j++) {
                    if (i!=j) {
                        float x2 = pts.at(j).x*cos(pts.at(j).y);
                        float y2 = pts.at(j).x*sin(pts.at(j).y);
                        float d = sqrt((x-x2)*(x-x2) + (y-y2)*(y-y2));
                        if (d<T+t && d > T-t)
                            ofLine(x, y, x2, y2);
                    }
                }
            }
        }
    }
    
    std::string getInfo(){
        return "";
    }

    
    void capture(ofFbo* fbo){    }
    void mask(){    }
//    void setImage(ofImage*){}
    
    void update(){
        if(dScale<0.7)
            dScale = 0.7;
        if(dScale>2.7)
            dScale = 2.7;
        scale += (dScale - scale)*0.7;//hagnew
        
        offx += doffx*pace;
        doffx -= pace*doffx;
        offy += doffy*pace;
        doffy -= pace*doffy;
        
        rot += upRot;
        offx += -offx*centerEase;
        offy += -offy*centerEase;
        
        while (pts.size() > MAX_PTS){
            pts.erase(pts.begin());
        }
        
        while (colors.size() > MAX_PTS){
            colors.erase(colors.begin());
        }

        cout << scale << endl;
        for(int i=0;i<pts.size();i++){
            pts[i].x *= scale;
            pts[i].y += upRot;
        }
    }
    
    
    void add(){
        float phi;
        if(randomPhi)
            phi = ofRandom(360);
        else
            phi = 0;
        int n = NB_PTS_ADD;
        if(n>2 || pause){
            for (int i=2;i<n;i++)
                pts.push_back(ofVec3f(HEIGHT/2, phi + i*(360/(n-1))));
        }
    }
    
    void addPoint(float x, float y){
        float a = fmod(360 + 90 + 180*(atan2(y, x) - M_PI/2)/M_PI,360)/360.0*2*M_PI;
        float d = ofDist(0, 0, x, y);
        pts.push_back(ofVec3f(d*HEIGHT, a));
    }
    
    void pointsFromNestedScene(){
        int reso = 4;
        pts.clear();
        sobelShader.begin();
        sobelShader.setUniform1i("fast", 1);
        nestedScene->draw();
        sobelShader.end();
        img.grabScreen(0, 0, WIDTH, HEIGHT);
        int pad = 15;
        int pace = 2;
        for(int x=pad;x<WIDTH-pad;x+=pace)
            for(int y=pad;y<HEIGHT-pad;y+=pace){
                float d = sqrt((x-ofGetMouseX())*(x-ofGetMouseX())+(y-ofGetMouseY())*(y-ofGetMouseY()));
                if(d<325)
                
                if(ofRandom(1)<0.5 && img.getColor(x,y).getLightness()>200){
                    pts.push_back(ofVec3f(x, HEIGHT-y)); //WHY -HEIGHT ??
                    y += 5;
                }
            }
        
    }

    void mousePressed(int x, int y, int button){}
    void mouseDragged(int x, int y, int button){}
    void mouseMoved(int x, int y){}

    void midiEvent(ofxMidiMessage& eventArgs){
        float value = eventArgs.value;
        switch(eventArgs.status){
            case MIDI_CONTROL_CHANGE:{
                switch(eventArgs.control){
                    case 1 :
                        upRot = 0.001 * 360 *(value-63)/127;
                        break;
                    case 2 :
                        dScale = 0.7 + 1 * sqrt(value)/sqrt(127);//hagnew
                        break;
                    case 13 :
                        doffx = 2*(value-65) - offx;
                        break;
                    case 14 :
                        doffy = 2*(value-65) - offy;
                        break;
                    case 7:
                        MAX_PTS = int(100*value/127);
                        break;
                    case 8:
                        T = t + HEIGHT * value/127;
                        break;
                    case 28:
                        rot = 0;
                        upRot = 0;
                        break;
                    case 29:
                        scale = 1;
                        dScale = 0;
                        break;
                    default:;
                }
            }
            case MIDI_NOTE_ON:{
                switch(eventArgs.pitch){
                    case 48:
                        NB_PTS_ADD=3;
                        break;
                    case 49:
                        NB_PTS_ADD=4;
                        break;
                    case 50:
                        NB_PTS_ADD=5;
                        break;
                    case 40:
                    render= POINTS;
                        break;
                    case 41:
                    render= LINES;
                        break;
                    case 42:
                    render= CONNECT;
                        break;
                    case 44:
                        pause = !pause;
                        break;
                    case 51:
                        randomPhi = !randomPhi;
                        break;
                    case 37:
                        erode = !erode;//fade out
                        break;
                    default:;
                }
            }
            case MIDI_NOTE_OFF:{
                switch(eventArgs.pitch){
                    case 40:
                        break;
                    default:;
                }
            }
            default:;
        }
    }

    
//    ofImage* getImage(){
//        return &img;
//    }
    
    void keyPressed(int key){
        switch(key){
            case '9':
                ((Cinema*)nestedScene)->randomJump();
                break;
        }
    };
    void oscEvent(std::string header, std::string arg){}
    void oscEvent(std::string header, std::vector<float> args){
        if(!header.compare("/Touch")){
        if(args.size()>=2){
            float x = args[0];
            float y = args[1];
            addPoint(x-0.5, y-0.5);
        }
        }
        if(!header.compare("/Pan")){
            if(args.size()>=2){
                float d = args[0];
                T += d;
            }
        }
    }

    void saveMacro(stringstream *xml){
        (*xml) << "<pointback>" << endl;
        
        saveParameter(xml, "upRot", upRot);
        saveParameter(xml, "scale", scale);
        saveParameter(xml, "dScale", dScale);
        saveParameter(xml, "doffx", doffx);
        saveParameter(xml, "doffy", doffy);
        saveParameter(xml, "pace", pace);
        saveParameter(xml, "offx", offx);
        saveParameter(xml, "offy", offy);
        saveParameter(xml, "rot", rot);
        saveParameter(xml, "inverted", inverted?1:0);
        saveParameter(xml, "erode", erode?1:0);
        saveParameter(xml, "randomPhi", randomPhi?1:0);
        saveParameter(xml, "render", render);
        saveParameter(xml, "pause", pause?1:0);
        saveParameter(xml, "offset", offset);
        saveParameter(xml, "MAX_LAYERS", MAX_LAYERS);
        saveParameter(xml, "MAX_PTS", MAX_PTS);
        saveParameter(xml, "NB_PTS_ADD", NB_PTS_ADD);
        saveParameter(xml, "T", T);
        saveParameter(xml, "t", t);
        saveParameter(xml, "centerEase", centerEase);

        (*xml) << "</pointback>" << endl;
    }
    
    void loadMacro(ofXml *xml){

        upRot = xml->getFloatValue("upRot");
        scale = xml->getFloatValue("scale");
        dScale = xml->getFloatValue("dScale");
        doffx = xml->getFloatValue("doffx");
        doffy = xml->getFloatValue("doffy");
        pace = xml->getFloatValue("pace");
        offx = xml->getFloatValue("offx");
        offy = xml->getFloatValue("offy");
        rot = xml->getFloatValue("rot");
        inverted = xml->getFloatValue("inverted")==1;
        erode = xml->getFloatValue("erode")==1;
        randomPhi = xml->getFloatValue("randomPhi")==1;
        render = xml->getFloatValue("render");
        pause = xml->getFloatValue("pause")==1;
        offset = xml->getFloatValue("offset");
        MAX_LAYERS = xml->getFloatValue("MAX_LAYERS");
        MAX_PTS = xml->getFloatValue("MAX_PTS");
        NB_PTS_ADD = xml->getFloatValue("NB_PTS_ADD");
        T = xml->getFloatValue("T");
        t = xml->getFloatValue("t");
        centerEase = xml->getFloatValue("centerEase");
            }
    
    ofImage img;
    ofImage circleMask;
    ofPixels pixes;
    
    vector<string> events;
    vector<ofVec3f> pts;
    vector<ofColor> colors;
    
    float upRot = 0., scale = 0.99;
    float dScale = scale, doffx=0, doffy=0;
    float pace = 0.1f;
    float offx=0, offy=0;
    float rot = 0; //rotation
    
    bool inverted = false;
    bool erode = false;
    bool randomPhi = false;
    int render = CONNECT;
    
    ofColor strokeColor = 0;
    
    bool pause = false;
    bool offset = true;
    
    int MAX_LAYERS = 25;
    int MAX_PTS = 100;
    int NB_PTS_ADD = 4;
    
    float T = 50; //distance thresholds for point connecting
    float t = 25;
    
    float centerEase = 0.04; //bo back to center easing
    
    Scene *nestedScene=0;
    ofShader sobelShader;

};

#endif
