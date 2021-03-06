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
#include <ofVideoGrabber.h>
#include "distortion.h"

#define CENTRAL 0
#define LEFT 1
#define RIGHT 2
#define DOUBLE 3

#define GEO 0
#define HOR 1

#define MODE_FEEDBACK 0
#define MODE_GLITCH 1

// log : removed imageUzi etc

class Feedback : public Scene
{
    
public:
    
    Feedback(AbstractApp* app) : Scene(app, "feedback"){
        sceneID = "feedback";
        
        remanentFbo = new ofFbo;
        remanentFbo->allocate(WIDTH, HEIGHT2 );
        source = new ofFbo;
        source->allocate(WIDTH, HEIGHT2);
        source2 = new ofFbo;
        source2->allocate(WIDTH, HEIGHT2);

        loadImgs();
        
        invertShader.load("../shaders/invert");
        shader.load("../shaders/colorthresholdsaturation");
        erodeShader.load("../shaders/erode");
        
        alphaShader.load("../shaders/alphaBlack");
        blendShader.load("../shaders/blend");
        maskShader.load("../shaders/blend");
        
//        feedbackShader.load("../shaders/feedback");
//        feedbackShaderImage.loadImage("../shaders/feedbackImage.jpg");
//        feedbackShaderImage.resize(WIDTH, HEIGHT);
        
        shapeMesh.setMode(OF_PRIMITIVE_LINE_LOOP);
        
        randomColor.reserve(100);
        updateRandomColor();
        
        #ifdef CAMERA
#ifdef VERBOSE
        grabby.listDevices();
#endif
#endif
    }
    
    void setup(){
        updateShape();
    }

    void processEvents(){
        for (int e=0;e<events.size();e++) {

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
    }
    
    void draw(){
        processEvents();

        source->begin();{
            ofPushMatrix();{
                       ofTranslate(WIDTH/2, HEIGHT2/2);
                       ofTranslate(app->parameterMap[mediaX]*HEIGHT, app->parameterMap[mediaY]*HEIGHT);
                       ofRotateX(app->parameterMap[pitchRot]);
                       ofRotate(app->parameterMap[rot]);
                       ofScale(app->parameterMap[scale], app->parameterMap[scale]);

                       ofPushMatrix();

                       ofSetColor(app->parameterMap[feedbackRemanence] * 255);

                       remanentFbo->draw(-WIDTH/2,-HEIGHT2/2);

                       if(app->parameterMap[doubleFeedback]==1){ // double feedback
                           ofPushMatrix();
                           float sss= 1/app->parameterMap[scale];
                           sss *= sss;
                           ofScale(sss, sss);

                           blendShader.begin();
                           blendShader.setUniform1i("mode", app->parameterMap[blendType]);
                           blendShader.setUniform1f("thresh", 0.3);
                           remanentFbo->draw(-WIDTH/2,-HEIGHT2/2); //blend ?
                           blendShader.end();
                           ofPopMatrix();
                       }

                       if(app->deltaMap[taijin]){ //taijin : a taoist inspired idea
                           app->deltaMap[backMask] = 1;
                           for(int i=-1;i<=1;i+=2){
                               ofPushMatrix();
                           ofTranslate(0, i*1.5*HEIGHT/6);
                           ofScale(-1/6.0, 1/6.0);

                               maskShader.begin();
                               maskShader.setUniform1f("thresh", 1); //dunno why 1 works
                               maskShader.setUniform1f("WIDTH", WIDTH);
                               maskShader.setUniform1f("HEIGHT", HEIGHT);
                               maskShader.setUniform1i("invert", 1);
                               maskShader.setUniform1i("alpha", 1);
                               maskShader.setUniform1i("circleMode", 1);
                               remanentFbo->draw(-WIDTH/2,-HEIGHT2/2);
                               maskShader.end();

                               ofPopMatrix();
                           }
                       }

                       ofPushMatrix();
                       {
                           ofScale(1/app->parameterMap[scale], 1/app->parameterMap[scale]);
                           ofTranslate(-WIDTH/2, -HEIGHT2/2);

                           ofTranslate(app->parameterMap[mediaX]*WIDTH, -app->parameterMap[mediaY]*HEIGHT, app->parameterMap[mediaZ]*HEIGHT);
                           ofRotateX(app->parameterMap[mediaRotX]);
                           ofRotateY(app->parameterMap[mediaRotY]);
                           ofRotateZ(app->parameterMap[mediaRotZ]);

           //                drawNested();

           #ifdef CAMERA
                           if(app->parameterMap[camera]>0 && grabby.isInitialized()){
                               blendShader.begin();
           //                    int mode = app->parameterMap[blendType];
                               blendShader.setUniform1i("mode", 0);
                               blendShader.setUniform1f("thresh", 0.3);
                               grabby.draw(0, 0, WIDTH, HEIGHT);
                               blendShader.end();
                           }
           #endif

                       }
                       ofPopMatrix();

                       ofPushMatrix();
                       {
                           ofScale(1/app->parameterMap[scale], 1/app->parameterMap[scale]);
                           ofTranslate(-WIDTH/2, -HEIGHT2/2);

           //                drawNested();

                           ofTranslate(app->parameterMap[mediaX]*WIDTH, -app->parameterMap[mediaY]*HEIGHT, app->parameterMap[mediaZ]*HEIGHT);
                           ofRotateX(app->parameterMap[mediaRotX]);
                           ofRotateY(app->parameterMap[mediaRotY]);
                           ofRotateZ(app->parameterMap[mediaRotZ]);

                       }ofPopMatrix();

                       if(app->parameterMap[blackCenter]){
                           ofSetColor(ofColor::black);
                           ofTranslate(WIDTH/2, HEIGHT2/2, 5*(app->parameterMap[scale]-0.7));
                           ofSetColor(ofColor::white);
                       }

                       ofPopMatrix();

                       ofRotate(-app->parameterMap[rot]);
                       ofRotateX(-app->parameterMap[pitchRot]);

                       if(!app->parameterMap[noSource]){

                           if(app->deltaMap[shapeNbPts]>=1)
                               drawShape();

                           if(app->parameterMap[nbPoints]>0)
                               drawPoints();

                           ofRotate(app->parameterMap[curShapeRot]);
                           ofRotateX(app->parameterMap[mediaRotX]);
                           ofRotateY(app->parameterMap[mediaRotY]);
                           ofRotateZ(app->parameterMap[mediaRotZ]);
                           ofTranslate(app->parameterMap[mediaX]*WIDTH,app->parameterMap[mediaY]*HEIGHT,app->parameterMap[mediaZ]*HEIGHT);
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
                           if(shapeId != app->deltaMap[shapeNbPts] + app->deltaMap[shapeStyle]*1000) //draw a new shape
                               shapeId = app->deltaMap[shapeNbPts] + app->deltaMap[shapeStyle]*1000;
                       }
                   }ofPopMatrix();

                       drawNested();

                       ofPushMatrix();

                       ofTranslate(WIDTH/2, HEIGHT2/2);
                       ofTranslate(app->parameterMap[mediaX]*HEIGHT, app->parameterMap[mediaY]*HEIGHT);
                       ofRotateX(app->parameterMap[pitchRot]);
                       ofRotate(app->parameterMap[rot]);
                       ofScale(app->parameterMap[scale], app->parameterMap[scale]);


                       ofSetColor(app->parameterMap[feedbackRemanence] * 255);
                       ofTranslate(-WIDTH/2, -HEIGHT2/2);
                       alphaBlend(remanentFbo);

                       ofPopMatrix();


                   }source->end();

           //        applyFeedback(source, source2);
           //        ofFbo* tmp = source;
           //        source = source2;
           //        source2 = tmp;

                   applyThreshold(source, source2);

                   if(app->parameterMap[erode]){
                       ofFbo* tmp = source;
                       source = source2;
                       source2 = tmp;
                       applyErosion(source, source2);
                   }

                   ofFbo* tmp = source;
                   source = source2;
                   source2 = tmp;

                   source->draw(0,0);
    }
    
//    void applyFeedback(ofFbo* src,ofFbo* dest){
//        LOAD_ASYNC         feedbackShader.load("../shaders/feedback");
//
//        dest->begin();
//        ofBackground(0);
//        feedbackShader.begin();
//        feedbackShader.setUniformTexture("tex1", feedbackShaderImage.getTextureReference(), 1);
//        src->draw(0,0);
//        feedbackShader.end();
//        dest->end();
//    }
    
    void applyInvert(ofFbo* src,ofFbo* dest){
        dest->begin();
        ofBackground(0);
        invertShader.begin();
        src->draw(0,0);
        invertShader.end();
        dest->end();
    }
    
    
    void applyThreshold(ofFbo* src,ofFbo* dest){
        dest->begin();
        {
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
            thresholdShader->setUniform1f("saturation", app->parameterMap[tintSaturation]);
        }
        
        ofSetColor(255,255,255);
        src->draw(0,0);
        
        
        if(app->parameterMap[blackCenter]>=1-BOOL_PARAM_EPSILON){
            ofSetColor(ofColor::black);
            ofTranslate(WIDTH/2, HEIGHT2/2, 5*(app->parameterMap[scale]-0.7));
            ofSetColor(ofColor::white);
        }
        
        if(app->parameterMap[threshold]>=BOOL_PARAM_EPSILON){
            thresholdShader->end();
        }
        
        app->parameterMap[doubleInversion] = !app->parameterMap[doubleInversion];
        app->parameterMap[f_invertFrame] = false;
        }
        dest->end();
    }
    
    void applyErosion(ofFbo* src, ofFbo* dest){
        dest->begin();
        ofBackground(0);
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
        shapeMeshNb = app->deltaMap[shapeNbPts];
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
                ofTranslate(0, 0, s);
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
            //            glLineWidth(app->parameterMap[shapeWeight]);
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
                ofDrawRectangle(x-app->parameterMap[shapeWeight]*10/2, -HEIGHT2/2, app->parameterMap[shapeWeight]*10, HEIGHT2);
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
        fbo->draw(0, HEIGHT, WIDTH, -HEIGHT);
        remanentFbo->end();
    }
    
    void drawNested(){  
        if(nestedScene!=0){
            alphaShader.begin();
            alphaShader.setUniform1f("alphaOffset", 1);
            ofPushMatrix();
            nestedScene->draw();
            ofPopMatrix();
            alphaShader.end();
        }
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
        
        app->parameterMap[curShapeRot] += app->parameterMap[shapeRot];
        app->deltaMap[curShapeRot] = app->parameterMap[curShapeRot] ;
        
        if(nestedScene!=0)
            nestedScene->update();
        
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
        
        if(shapeMeshNb != app->deltaMap[shapeNbPts])
            updateShape();
        
        #ifdef CAMERA
        if(app->parameterMap[camera] > 0){
            if(!grabby.isInitialized()){
                grabby.setVerbose(true);
                grabby.setDeviceID(int(app->parameterMap[camera]));
                grabby.initGrabber(WIDTH, HEIGHT);
                grabby.setVerbose(false);
            }else{
                grabby.update();
            }
        }
#endif
        
    }
    void mousePressed(int x, int y, int button){}
    
    void mouseDragged(int x, int y, int button){
    }
    void mouseMoved(int x, int y){}
    
    void touchMoved(ofTouchEventArgs &touch){
        
    }

    
    void midiEvent(ofxMidiMessage& eventArgs){
        if(nestedScene!=0){
            if (eventArgs.pitch>=20 && eventArgs.pitch < 36)
                nestedScene->midiEvent(eventArgs);
        }
    }
    
    void loadImgs() {
        circleMask.loadImage("assets/maskCircle1280x720.png");
        
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
        }
    }
    
    void updateRandomColor(){
        for(int i=0;i<15;i++){
            int h = (int(ofRandom(-app->parameterMap[tintAmp]/2,app->parameterMap[tintAmp]/2))*255 + 255) %  255 ;
            randomColor[i] = ofColor::fromHsb(h, 0.8*255, 150);
        }
    }
    
    void oscEvent(std::string header, std::string file){
    }
    
    void saveMacro(stringstream *xml){
        (*xml) << "<feedback>" << endl;
        
        (*xml) << "</feedback>" << endl;
    }
    
    
    void loadMacro(TiXmlHandle *xml){
        updateShape();
    }
    
    void oscEvent(std::string header, vector<float> args){
    }
    
    void exit(){}
    

    void setResolution(){
        delete remanentFbo;
        delete source;
        delete source2;
        remanentFbo = new ofFbo;
        remanentFbo->allocate(WIDTH, HEIGHT2, GL_RGBA);
        source = new ofFbo;
        source->allocate(WIDTH, HEIGHT2);
        source2 = new ofFbo;
        source2->allocate(WIDTH, HEIGHT2);
    }
    
public:
    ofFbo* source,*source2;
    
    ofFbo* remanentFbo;
    ofImage circleMask;
    
    vector<string> events;
    
    string crtImgPath = "",crtVidPath="";
    ofImage crtImg;
    ofShader shader, invertShader,erodeShader, maskShader;
    
    //SHAPES
    ofMesh shapeMesh;
    int shapeMeshNb = 0;
    vector<ofColor> randomColor;
    
    vector<EPoint> points;
    
    //NESTED
    Scene *nestedScene=0;
    ofShader alphaShader, blendShader;
//    ofShader feedbackShader;
//    ofImage feedbackShaderImage;

    int shapeId = 0;
    
#ifdef CAMERA
    ofVideoGrabber grabby;
#endif
};

#endif /* defined(__emptyExample__Feedback__) */
