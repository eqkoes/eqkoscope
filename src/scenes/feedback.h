#ifndef __emptyExample__Feedback__
#define __emptyExample__Feedback__

#include "ofxMidi.h"
#include "scene.h"
#include "utils.h"
#include "point.h"
#include <ofBaseTypes.h>
#include <iostream>
#include <sstream>
#include "math.h"
#include "stdlib.h"

#define CENTRAL 0
#define LEFT 1
#define RIGHT 2
#define DOUBLE 3

#define GEO 0
#define HOR 1

#define MODE_FEEDBACK 0
#define MODE_GLITCH 1

class Feedback : public Scene
{
    
public:
    
    Feedback(AbstractApp* app) : Scene(app, "feedback"){
        sceneID = "feedback";

        img = new ofImage;
        img->allocate(WIDTH, HEIGHT2, OF_IMAGE_COLOR);
        remanentFbo = new ofFbo;
        remanentFbo->allocate(WIDTH, HEIGHT2);
        source = new ofFbo;
        source->allocate(WIDTH, HEIGHT2);
        source2 = new ofFbo;
        source2->allocate(WIDTH, HEIGHT2);
        loadImgs();
        
        invertShader.load("../shaders/invert");
        shader.load("../shaders/colorthresholdsaturation");
        erodeShader.load("../shaders/erode");
        
        alphaShader.load("../shaders/alpha");
        hBackShader.load("../shaders/hBack");
        
        
        shapeMesh.setMode(OF_PRIMITIVE_LINE_LOOP);
        
        vidPlayer = new ofVideoPlayer;
        
        randomColor.reserve(100);
        updateRandomColor();
        
        initParameters();
    }
    
    void setup(){
        updateShape();
        app->blackNWhiteMedia = true;
    }
    
    void draw(){
//        for(map<string,float>::iterator it=app->parameterMap.begin();it!=app->parameterMap.end();it++)
//            cout << it->first << " " << it->second << endl;
        
        
        source->begin();
        
        if (app->deltaMap[feedMode]!=MODE_FEEDBACK){
            ofSetColor(ofColor::black);
            ofRect(0,0,WIDTH,HEIGHT2);
            ofSetColor(ofColor::white);
        }
        
        for (int e=0;e<events.size();e++) {
            if (!events.at(e).compare("changeImage")) {
                if (app->deltaMap[feedMode]==MODE_FEEDBACK){
                    drawImg = true;
                    imgSrc = &(imgs.at(imgChoice));
                    drawVid = false;
                }
                break;
            }
            if (!events.at(e).compare("changeImgPath")) {
                if (app->deltaMap[feedMode]==MODE_FEEDBACK && crtImgPath.compare("")){
                    drawImg = true;
                    crtImg.loadImage(crtImgPath);
                    crtImg.resize(WIDTH, HEIGHT);
                    imgSrc = &crtImg;
                    drawVid = false;
                }
                break;
            }
            if (!events.at(e).compare("changeVidPath")) {
                if (app->deltaMap[feedMode]==MODE_FEEDBACK && crtVidPath.compare("")){
                    drawVid = true;
                    vidPlayer->loadMovie(crtVidPath);
                    vidPlayer->play();
                }
                break;
            }
            if (!events.at(e).compare("invert")) {
                app->parameterMap[f_invertFrame] = true;
                app->parameterMap[doubleInversion] = true;
                break;
            }
            if(!events.at(e).compare("clearRandomShape")){
                shapeMesh.clear();
            }
            if(!events.at(e).compare("randomShape")){
                int n = 3+ofRandom(7);
                shapeMesh.clear();
                for(int i=0;i<n;i++){
                    float x = ofRandom(-WIDTH/3,WIDTH/3);
                    float y = ofRandom(-HEIGHT/3, HEIGHT/3);
                    shapeMesh.addVertex(ofVec3f(x , y , 0));
                    if(app->parameterMap[randomShapeColor])
                        shapeMesh.addColor(randomColor[i]);
                }
            }
            if(!events.at(e).compare("randomShape")){
                int n = 3+ofRandom(7);
                shapeMesh.clear();
                for(int i=0;i<n;i++){
                    float x = ofRandom(-WIDTH/3,WIDTH/3);
                    float y = ofRandom(-HEIGHT/3, HEIGHT/3);
                    shapeMesh.addVertex(ofVec3f(x , y , 0));
                    if(nestedScene==0)
                        shapeMesh.addColor(ofColor::fromHsb((app->parameterMap[tintCenter] + ofRandom(app->parameterMap[tintAmp]))*255, app->parameterMap[tintSaturation]*255, 150));
                }
            }
            if(!events.at(e).compare("randomConstellation")){
                int n = 3+ofRandom(6);
                shapeMesh.clear();
                float xx = -WIDTH/2;
                float yy = -HEIGHT/2;
                for(int i=0;i<n;i++){
                    bool doubl = ofRandom(1)<0.5;
                    float x = ofRandom(-WIDTH/2,WIDTH/2);
                    float y = ofRandom(-HEIGHT/2, HEIGHT/2);
                    xx +=(x+WIDTH/2)/5;
                    yy+= y;
                    shapeMesh.addVertex(ofVec3f(xx , yy , 0));
                    if(doubl){
                        shapeMesh.addVertex(shapeMesh.getVertex(shapeMesh.getNumVertices()-2));
                        shapeMesh.addVertex(ofVec3f(xx , abs(yy-HEIGHT/2) , 0));
                    }
                }
            }
            if(!events.at(e).compare("randomShapeOff")){
                shapeMesh.clear();
            }
        }
        events.clear();
        
        if (imageUzi)
            events.push_back("changeImage");
        
        ofPushMatrix();{
            ofTranslate(WIDTH/2, HEIGHT2/2);
            ofTranslate(app->parameterMap[offx], app->parameterMap[offy]);
            ofRotateX(app->parameterMap[pitchRot]);
            ofRotate(app->parameterMap[rot]);
            ofScale(app->parameterMap[scale], app->parameterMap[scale]);
            
            ofPushMatrix();
            
            ofSetColor(app->parameterMap[feedbackRemanence] * 255);
            if(app->parameterMap[post_traitement])
                remanentFbo->draw(-WIDTH/2,-HEIGHT2/2); //draw the olde image
            else
                remanentFbo->draw(-WIDTH/2,HEIGHT2/2, WIDTH, -HEIGHT2);
            
            
            if(app->parameterMap[blackCenter]){
                ofSetColor(ofColor::black);
                ofCircle(WIDTH/2, HEIGHT2/2, 5*(app->parameterMap[scale]-0.7));
                ofSetColor(ofColor::white);
            }
            //hBackShader.end();
            
            //alphaShader.end();
            ofPopMatrix();
            
            ofRotate(-app->parameterMap[rot]);
            ofRotateX(-app->parameterMap[pitchRot]);
            
            if(!app->parameterMap[noSource]){
                
                if (drawImg) {
                    imgSrc->draw(-imgSrc->width/2, -imgSrc->height/2);
                    if(!imageUzi)
                        drawImg = false;
                }
                
                if(drawVid && vidPlayer!=0){
                    ofEnableBlendMode(OF_BLENDMODE_ADD); //ADD VIDEO SOURCE
                    //TODO : modulate alpha by brightness ? (in shader)
                    vidPlayer->draw(-vidPlayer->width/2,-vidPlayer->height/2);
                    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
                }
                
                if(app->deltaMap[shapeNbPts]>=1)
                    drawShape();
                
                if(app->parameterMap[nbPoints]>0)
                    drawPoints();
                
                ofRotate(app->parameterMap[curShapeRot]);
                if(shapeMesh.getNumVertices()>0){
                    for(int i=0;i<app->parameterMap[shapeWeight];i++){
                        ofTranslate(0,1*i);
                        shapeMesh.draw();
                        ofTranslate(0,-2*i);
                        shapeMesh.draw();
                        ofTranslate(1*i,1*i);
                        shapeMesh.draw();
                        ofTranslate(-2*i,0);
                        shapeMesh.draw();
                        ofTranslate(1*i,0);
                    }
                    shapeMesh.draw();
                }
            }
        }ofPopMatrix();
        
        if(nestedScene!=0){
            ofEnableBlendMode(OF_BLENDMODE_ADD);
            ofSetColor(ofColor(255,255,255, app->parameterMap[mediaAlpha]*255));
            alphaShader.begin();
            ofPushMatrix();
            nestedScene->draw();
            ofPopMatrix();
            alphaShader.end();
            ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        }
        
        if(!app->parameterMap[noSource]){
            if (app->deltaMap[feedMode] == MODE_GLITCH)
                staticGlitch();
        }
        
        source->end();
        
        if(app->parameterMap[erode]){
            applyErosion(source, source2);
            applyThreshold(source2, source);
            source->draw(0,0);
        }else{
            applyThreshold(source, source2);
            source2->draw(0,0);
        }
        
                    bool invert = app->parameterMap[f_invertFrame] || (app->parameterMap[f_strobe] && app->parameterMap[doubleInversion]);
        if(invert){
            applyInvert(source2, source);
//            ofFbo* tmp = source;
//            source = source2;
//            source2 = tmp;
        }
    }
    
    void applyInvert(ofFbo* src,ofFbo* dest){
        dest->begin();
        invertShader.begin();
        src->draw(0,0);
        invertShader.end();
        dest->end();
    }
    
    
    void applyThreshold(ofFbo* src,ofFbo* dest){
        dest->begin();
        
        ofBackground(0);
        
        ofShader* thresholdShader = &shader;
        
        if(app->parameterMap[threshold] >= BOOL_PARAM_EPSILON){
            thresholdShader->begin();
            bool invert = app->parameterMap[f_invertFrame] || (app->parameterMap[f_strobe] && app->parameterMap[doubleInversion]);
            thresholdShader->setUniform1f("threshold", 0.5);
            thresholdShader->setUniform1f("invert", invert);
            thresholdShader->setUniform1f("drywet", app->parameterMap[threshold]);
            thresholdShader->setUniform1f("brightness", 1);
            thresholdShader->setUniform1f("levels", 2);
            thresholdShader->setUniform1f("saturation", app->parameterMap[mediaSaturation]);
        }
        
        ofSetColor(255,255,255);
        src->draw(0,0);
                
        
        if(app->parameterMap[blackCenter]>=1-BOOL_PARAM_EPSILON){
            ofSetColor(ofColor::black);
            ofCircle(WIDTH/2, HEIGHT2/2, 5*(app->parameterMap[scale]-0.7));
            ofSetColor(ofColor::white);
        }
        
        if(app->parameterMap[threshold]>=BOOL_PARAM_EPSILON){
            thresholdShader->end();
        }
        
        app->parameterMap[doubleInversion] = !app->parameterMap[doubleInversion];
        app->parameterMap[f_invertFrame] = false;
        
        dest->end();
    }
    
    void applyErosion(ofFbo* src, ofFbo* dest){
        dest->begin();
        erodeShader.begin();
        erodeShader.setUniform1i("fast", app->parameterMap[fastErode]);
        src->draw(0,0);
        erodeShader.end();
        dest->end();
    }
    
    //attractor : towards center (0,0)
    void modShape(float amp=250, float attractor = 0.005){
        int fn = ofGetFrameNum();
        for(int i=0;i<shapeMesh.getNumVertices();i++){
            ofVec3f v = shapeMesh.getVertex(i);
            v.x += (ofNoise((fn)*0.1+i*10)-0.5)*amp;
            v.y += (ofNoise((fn)*0.1+(i*10)*10)-0.5)*amp;
            v.x -= (v.x)*attractor*ofNoise(fn*0.001);
            v.y -= (v.y)*attractor*ofNoise(fn*0.001);
            if(v.x < -HEIGHT/3)
                v.x = -HEIGHT/3;
            if(v.y < -HEIGHT/3)
                v.y = -HEIGHT/3;
            if(v.x > HEIGHT/3)
                v.x = HEIGHT/3;
            if(v.y > HEIGHT/3)
                v.y = HEIGHT/3;
            shapeMesh.setVertex(i, v);
        }
    }
    
    void updateShape(){
        if(app->deltaMap[shapeStyle]==GEO){
            ofPolyline path;
            int s = WIDTH/4;
            if(app->deltaMap[shapeNbPts]!=666){
                shapeMesh.clear();
                float a;
                for(int i=0;i<=app->deltaMap[shapeNbPts];i++){
                    a = i*PI*2 / (float) app->deltaMap[shapeNbPts];
                    shapeMesh.addVertex(ofVec3f(s*cos(a) , s*sin(a) , 0));
                    if(app->parameterMap[randomShapeColor])
                        shapeMesh.addColor(randomColor[i]);
                    else
                        shapeMesh.addColor(ofColor::white);
                }
            }
        }
    }
    
    void drawShape(){
        ofPushMatrix();
        if(app->deltaMap[shapeStyle]==GEO){
            ofRotate(app->parameterMap[curShapeRot]);
            ofPolyline path;
            int s = WIDTH/4;
            if(app->deltaMap[shapeNbPts]!=666){
                /* shapeMesh.clear();
                 float a;
                 for(int i=0;i<=shapeNbPts;i++){
                 a = i*PI*2 / (float) shapeNbPts;
                 shapeMesh.addVertex(ofVec3f(s*cos(a) , s*sin(a) , 0));
                 if(randomShapeColor)
                 shapeMesh.addColor(randomColor[i]);
                 else
                 shapeMesh.addColor(ofColor::white);
                 }*/
            }else{
                ofNoFill();
                ofSetCircleResolution(100);
                ofCircle(0, 0, s);
                ofPushMatrix();
                ofScale(0.5, 0.5);
                float off = PI/10.0;
                path.lineTo(s*cos(2*PI/5.0+off), s*sin(2*PI/5.0+off));
                path.lineTo(s*cos(2*3*PI/5.0+off), s*sin(2*3*PI/5.0+off));
                path.lineTo(s*cos(2*5*PI/5.0+off), s*sin(2*5*PI/5.0+off));
                path.lineTo(s*cos(2*2*PI/5.0+off), s*sin(2*2*PI/5.0+off));
                path.lineTo(s*cos(2*4*PI/5.0+off), s*sin(2*4*PI/5.0+off));
                path.lineTo(s*cos(2*PI/5.0+off), s*sin(2*PI/5.0+off));
                ofPopMatrix();
            }
            path.close();
            glLineWidth(app->parameterMap[shapeWeight]);
            for(int i=0;i<app->parameterMap[shapeWeight];i++){
                ofTranslate(0,1*i);
                path.draw();
                ofTranslate(0,-2*i);
                path.draw();
                ofTranslate(1*i,1*i);
                path.draw();
                ofTranslate(-2*i,0);
                path.draw();
                ofTranslate(1*i,0);
            }
            path.draw();
        }else{
            int colIndex = 0;
            for(int i=-app->deltaMap[shapeNbPts]/2;i<=app->deltaMap[shapeNbPts]/2;i++){
                float x = WIDTH/2*i/(float)app->deltaMap[shapeNbPts];
                if(app->parameterMap[randomShapeColor])
                    ofSetColor(randomColor[colIndex++]);
                ofRect(x-app->parameterMap[shapeWeight]*10/2, -HEIGHT2/2, app->parameterMap[shapeWeight]*10, HEIGHT2);
            }
        }
        ofPopMatrix();
    }
    
    void drawPoints(){
        ofFill();
        for(int i=0;i<points.size();i++){
            ofSetColor(ofColor(255,255,255,255*sin(PI/2*points[i].intensity)));
            ofEllipse(points[i].x-WIDTH/2, points[i].y-HEIGHT/2,
                      points[i].size*sin(PI/2*points[i].intensity), points[i].size*sin(PI/2*points[i].intensity));
        }
        ofSetColor(ofColor::white);
    }
    
    void capture(ofFbo* fbo){
        remanentFbo->begin();
        fbo->draw(0,0);
        //circleMask2.draw(0,0, WIDTH, HEIGHT2);
        remanentFbo->end();
    }
    
    void mask(){
        if (app->parameterMap[backMask])
            circleMask.draw(0, (HEIGHT2-HEIGHT)/2, app->FINALWIDTH +(ofGetWidth()-app->FINALWIDTH)/2, app->FINALHEIGHT);
    }
    
    void update(){
        
        /** VERY SPECIAL UPDATES FOR ROTATIONS **/
        app->parameterMap[rot] += app->parameterMap[upRot];
        app->deltaMap[rot] = app->parameterMap[rot] ;
        app->parameterMap[pitchRot] += app->deltaMap[pitchRot];
        
//        app->parameterMap[pitchRot] += (app->deltaMap[pitchRot]-app->parameterMap[pitchRot])*0.5;
        
        if(app->deltaMap[scale]<0.7)
            app->deltaMap[scale] = 0.7;
        if(app->deltaMap[scale]>2.7)
            app->deltaMap[scale] = 2.7;
        
//        app->parameterMap[scale] += (app->deltaMap[scale] - app->parameterMap[scale])*0.7;//hagnew
//        app->parameterMap[offx] += (app->deltaMap[offx]-app->parameterMap[offx])*0.1;
//        app->parameterMap[offy] += (app->deltaMap[offy]-app->parameterMap[offy])*0.1;
        
        app->parameterMap[curShapeRot] += app->parameterMap[shapeRot];
        app->deltaMap[curShapeRot] = app->parameterMap[curShapeRot] ;

        if(nestedScene!=0)
            nestedScene->update();
        
        if(vidPlayer!=0 && drawVid)
            vidPlayer->update();
        
        for(int i=0;i<points.size();i++){
            points[i].update();
            if(points[i].intensity<=0)
                points.erase(points.begin()+i);
        }
        
        if(points.size()>app->parameterMap[nbPoints])
            for(int i=0;i<points.size();i++)
                if(points.size()>app->parameterMap[nbPoints]){
                    points.erase(points.begin()+i);
                }else{
                    break;
                }
        else
            while(points.size()<app->parameterMap[nbPoints])
                points.push_back(EPoint(ofVec3f(ofRandom(WIDTH), ofRandom(HEIGHT), 0), ofRandom(25)));
    }
    
    void mouseDragged(int x, int y, int button){
        app->parameterMap[pointx] = x;
        app->parameterMap[pointy] = y;
    }
    
    void touchMoved(ofTouchEventArgs &touch){
        
    }
    
    void staticGlitch() {
        ofBackground(0);
        int res = 1;
        int x, y, h, w;
        for (int i=0;i<100;i++) {
            h = int(ofRandom(10));
            x = int(ofRandom(WIDTH));
            y = int(ofRandom(HEIGHT2));
            ofSetColor(ofRandom(220));
            if (x<WIDTH/2)
                ofRect(x, y, WIDTH, y+h);
            else
                ofRect(0, y, x, y+h);
        }
    }
    
    void midiEvent(ofxMidiMessage& eventArgs){
        if(nestedScene!=0){
            if (eventArgs.pitch>=20 && eventArgs.pitch < 36)
                nestedScene->midiEvent(eventArgs);
        }
        
        if(eventArgs.channel==1)
            return;
        
        float value = eventArgs.value;
        switch(eventArgs.status){
        case MIDI_CONTROL_CHANGE:{
            if(eventArgs.channel!=4)
                return;
            switch(eventArgs.control){
            case 1:
                app->deltaMap[upRot] = 0.01 * 360 *(value-63)/127;
                break;
            case 2:
                app->deltaMap[scale] = 0.7 + 1 * sqrt(value)/sqrt(127);
                break;
            case 4:
                        app->parameterMap[offx] = 2*(value-65);
                break;
            case 5:
                app->parameterMap[offy] = 2*(value-65);
                break;
            case 6 :
                app->parameterMap[feedbackRemanence] = value / 127.0;
                break;
            case 7:
                app->parameterMap[nbPoints] = value/127.0*25;
                break;
            case 8:
                app->deltaMap[pitchRot] = 0.5 * 360 *(value-63)/127;
                break;
                case 9:
                    app->deltaMap[shapeWeight] = value/127.0*20;
                    break;
            case 12:
                app->deltaMap[shapeNbPts]= (int)value/127.0*20;
                app->deltaMap[shapeStyle] = HOR;
                break;
            case 13:
                app->parameterMap[shapeWeight] = value/127.0*2;
                break;
            case 18:
                app->parameterMap[mediaAlpha] = value/127.0;
                break;
            case 19:
                app->parameterMap[mediaSaturation] = value/127.0;
                break;

            case 71:
                app->parameterMap[rot] = app->parameterMap[upRot] = app->deltaMap[upRot] = 0;
                break;
            case 72:
                app->parameterMap[scale] = app->deltaMap[scale]  = 1;
                break;
            case 35:
                app->deltaMap[shapeRot] = 0;
                break;
            case 77:
                app->parameterMap[pitchRot] = app->deltaMap[pitchRot] = 0;
                break;
            case 91:
                app->parameterMap[randomShapeColor] = value>64;
                if(app->parameterMap[randomShapeColor])
                    updateRandomColor();
                break;

            default:;
            }
        }break;
        case MIDI_NOTE_ON:{
            switch(eventArgs.channel){
            case 3:case 4:{
                switch(eventArgs.pitch){
                /** PNG SHAPES **/
                case 52:
                    imgChoice = 8;
                    events.push_back("changeImage");
                    imageUzi=true;
                    break;
                case 53:
                    imgChoice = 7;
                    events.push_back("changeImage");
                    imageUzi=true;
                    break;
                case 54:
                    imgChoice = 2;
                    events.push_back("changeImage");
                    imageUzi=true;
                    break;
                case 55:
                    imgChoice = 3;
                    events.push_back("changeImage");
                    imageUzi=true;
                    break;


                    /** ALGO SHAPES **/
                case 56:
                    imgChoice = 5;
                    events.push_back("changeImage");
                    imageUzi=true;
                    break;
                case 57:
                    imgChoice = 6;
                    events.push_back("changeImage");
                    imageUzi=true;
                    break;

                case 40:
                    app->deltaMap[shapeNbPts] = (app->deltaMap[shapeNbPts]==2&&app->deltaMap[shapeStyle]==GEO)?0:2;
                    app->deltaMap[shapeStyle] = GEO;
                    updateShape();
                    break;
                case 41:
                    app->deltaMap[shapeNbPts] = (app->deltaMap[shapeNbPts]==3&&app->deltaMap[shapeStyle]==GEO)?0:3;
                    app->deltaMap[shapeStyle] = GEO;
                    updateShape();
                    break;
                case 42:
                    app->deltaMap[shapeNbPts] = (app->deltaMap[shapeNbPts]==4&&app->deltaMap[shapeStyle]==GEO)?0:4;
                    app->deltaMap[shapeStyle] = GEO;
                    updateShape();
                    break;
                case 43:
                    app->deltaMap[shapeNbPts] = (app->deltaMap[shapeNbPts]==6&&app->deltaMap[shapeStyle]==GEO)?0:6;
                    app->deltaMap[shapeStyle] = GEO;
                    updateShape();
                    break;
                case 47:
                    app->deltaMap[shapeNbPts] = (app->deltaMap[shapeNbPts]==666&&app->deltaMap[shapeStyle]==GEO)?0:666;
                    app->deltaMap[shapeStyle] = GEO;
                    break;
                case 36:
                    app->deltaMap[shapeNbPts] = (app->deltaMap[shapeNbPts]==1&&app->deltaMap[shapeStyle]==HOR)?0:1;
                    app->deltaMap[shapeStyle] = HOR;
                    break;
                case 36+1:
                    app->deltaMap[shapeNbPts] = (app->deltaMap[shapeNbPts]==2&&app->deltaMap[shapeStyle]==HOR)?0:2;
                    app->deltaMap[shapeStyle] = HOR;
                    break;
                case 36+2:
                    app->deltaMap[shapeNbPts] = (app->deltaMap[shapeNbPts]==3&&app->deltaMap[shapeStyle]==HOR)?0:3;
                    app->deltaMap[shapeStyle] = HOR;
                    break;
                case 36+3:
                    app->deltaMap[shapeNbPts] = (app->deltaMap[shapeNbPts]==3&&app->deltaMap[shapeStyle]==HOR)?0:4;
                    app->deltaMap[shapeStyle] = HOR;
                    break;

                case 44:case 44+16:case 44+32:
                    app->deltaMap[feedMode] = MODE_GLITCH;
                    break;

                case 45:case 45+16:case 45+32:
                    app->deltaMap[backMask] = !app->deltaMap[backMask];
                    break;


                case 50:case 50+16:case 50+32:
                    app->parameterMap[f_strobe] = !app->parameterMap[f_strobe];
                    break;
                case 48:case 48+16:case 48+32:
                    app->deltaMap[erode] = app->parameterMap[erode] = true;
                    break;
                case 49:case 49+16:case 49+32:
                    app->parameterMap[noSource] = true;
                    break;
                    case 46:{
                        if(shapeMesh.getNumVertices()>0)
                            shapeMesh.clear();
                        else
                    events.push_back("randomShape");
                    } break;
                }
            }
                break;
            }
            break;
        }
        case MIDI_NOTE_OFF:{
            switch(eventArgs.channel){
                case 3:case 4:{
                switch(eventArgs.pitch) {
                case 44:case 44+16:case 44+32:
                    app->deltaMap[feedMode] = MODE_FEEDBACK;
                    break;
                case 49:case  49+16:case  49+32:
                    app->parameterMap[noSource] = false;
                    break;
                case 48:case 48+16:case 48+32:
                    app->deltaMap[erode] = app->parameterMap[erode] = false;//fade out
                    break;
                case 36:
                case 36+16:
                case 37:
                case 37+16:
                case 38:
                case 38+16:
                case 39:
                case 39+16:
                case 40:
                case 40+16:
                case 41:
                case 41+16:
                case 42:
                case 43:
                case 42+16:
                case 43+16:
                    imageUzi=false;
                    break;
                default:;
                }
            }
                break;
            }
        default:;
            }
        }
    }
    
    void loadImgs() {
        for (int i=0;i<9;i++) {
            stringstream s;
            s << "./circle/img" << i << ".png";
            ofImage im;
            im.loadImage(s.str());
            im.resize(WIDTH, HEIGHT);
            imgs.push_back(im);
        }
        if(HEIGHT==768){
            circleMask.loadImage("assets/maskCircle1280x768.png");
        }else if(HEIGHT==720){
            circleMask.loadImage("assets/maskCircle1280x720.png");
        }
        circleMask.resize(app->FINALWIDTH, app->FINALHEIGHT);
    }
    
    std::string getInfo(){
        return "";
    }
    
    void keyPressed(int key){
        switch(key){
        case ' ':
                updateShape();
            break;
        case 'c':{
            app->parameterMap[randomShapeColor] = !app->parameterMap[randomShapeColor];
            if(app->parameterMap[randomShapeColor])
                updateRandomColor();
        } break;
        }
    }
    
    void updateRandomColor(){
        for(int i=0;i<15;i++){
            int h = (int(app->parameterMap[tintCenter] + ofRandom(-app->parameterMap[tintAmp]/2,app->parameterMap[tintAmp]/2))*255 + 255) %  255 ;
            randomColor[i] = ofColor::fromHsb(h, 0.8*255, 150);
        }
    }
    
    void oscEvent(std::string header, std::string file){
    }
    
    void saveMacro(stringstream *xml){
        (*xml) << "<feedback>" << endl;
        
        (*xml) << "</feedback>" << endl;
    }
    
    
    void loadMacro(ofXml *xml){
//        remanentFbo->begin();
//        ofBackground((0));
//        remanentFbo->end();
        //if(!xml-> .getAttribute("version").compare("v2")){

//            for(int ch=0;ch<xml->getNumChildren();ch++){
//                xml->setToChild(0);
//                float v = xml->getFloatValue();
//                app->parameterMap[xml->getName()] = v;
//                app->deltaMap[xml->getName()] = v;
//                xml->setToParent();
//            }
        
        updateShape();
    }
    
    void oscEvent(std::string header, vector<float> args){
    }
    
    void exit(){}
    
public:
    ofFbo* source,*source2;

    ofImage* img;
    ofImage* imgSrc;
    ofFbo* remanentFbo;
    ofImage circleMask;
    vector<ofImage> imgs;
    int imgChoice=0;

    vector<string> events;
    bool imageUzi = false;
    bool drawImg = false, drawVid = false;
    
    string crtImgPath = "",crtVidPath="";
    ofImage crtImg;
    ofShader shader, invertShader,erodeShader, hBackShader;
    ofVideoPlayer* vidPlayer=0;
    
    //SHAPES
    ofMesh shapeMesh;
    vector<ofColor> randomColor;
    
    vector<EPoint> points;
    
    //NESTED
    Scene *nestedScene=0;
    ofShader alphaShader;
};

#endif /* defined(__emptyExample__Feedback__) */
