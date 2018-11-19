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
        blendShader.load("../shaders/blend");
        hBackShader.load("../shaders/hBack");
        
        
        shapeMesh.setMode(OF_PRIMITIVE_LINE_LOOP);
        
        vidPlayer = new ofVideoPlayer;
        
        randomColor.reserve(100);
        updateRandomColor();
            }
    
    void setup(){
        updateShape();
    }
    
    void draw(){
        source->begin();
        
            ofSetColor(ofColor::black);
            ofRect(0,0,WIDTH,HEIGHT2);
            ofSetColor(ofColor::white);
        
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
        
        if (imageUzi)
            events.push_back("changeImage");
        
        ofPushMatrix();{
            ofTranslate(WIDTH/2, HEIGHT2/2);
//            ofTranslate(app->parameterMap[offx], app->parameterMap[offy]);
            ofRotateX(app->parameterMap[pitchRot]);
            ofRotate(app->parameterMap[rot]);
            ofScale(app->parameterMap[scale], app->parameterMap[scale]);
            
            ofPushMatrix();
            
            ofSetColor(app->parameterMap[feedbackRemanence] * 255);
           
            remanentFbo->draw(-WIDTH/2,-HEIGHT2/2);
            
            ofScale(1/app->parameterMap[scale], 1/app->parameterMap[scale]);
            ofTranslate(-WIDTH/2, -HEIGHT2/2);
            drawNested();
            ofTranslate(WIDTH/2, HEIGHT2/2);
            ofScale(app->parameterMap[scale], app->parameterMap[scale]);

            if(app->parameterMap[blackCenter]){
                ofSetColor(ofColor::black);
                ofCircle(WIDTH/2, HEIGHT2/2, 5*(app->parameterMap[scale]-0.7));
                ofSetColor(ofColor::white);
            }

            ofPopMatrix();

            ofRotate(-app->parameterMap[rot]);
            ofRotateX(-app->parameterMap[pitchRot]);
            
            if(!app->parameterMap[noSource]){
                
//                if (drawImg) {
//                    imgSrc->draw(-imgSrc->width/2, -imgSrc->height/2);
//                    if(!imageUzi)
//                        drawImg = false;
//                }
                
//                if(drawVid && vidPlayer!=0){
//                    ofEnableBlendMode(OF_BLENDMODE_ADD); //ADD VIDEO SOURCE
//                    //TODO : modulate alpha by brightness ? (in shader)
//                    vidPlayer->draw(-vidPlayer->width/2,-vidPlayer->height/2);
//                    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//                }
                
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

        source->end();
        
        if(app->parameterMap[erode])
        {
            applyErosion(source, source2);
            applyThreshold(source2, source);
            source->draw(0,0);
        }
        else
        {
            applyThreshold(source, source2);
            source2->draw(0,0);
        }
        
        bool invert = app->parameterMap[f_invertFrame]
        || (app->parameterMap[f_strobe] && app->parameterMap[doubleInversion]);
        if(invert)
        {
            applyInvert(source2, source);
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
            thresholdShader->setUniform1f("saturation", 1);
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
        drawNested();
        remanentFbo->end();
    }
    
    void drawNested(){
        if(nestedScene!=0){
            //            blendShader.load("../shaders/blend");
            
            int mode = app->parameterMap[blendType];
            if(mode==13)
                blendShader.setUniformTexture("tex1", *remanentFbo, 0);
            
            blendShader.begin();
            blendShader.setUniform1i("mode", mode);
            blendShader.setUniform1f("thresh", 0.3);
            //            blendShader.setUniform1f("feedbackCompensation", 1.33);
            
            ofPushMatrix();
            nestedScene->draw();
            ofPopMatrix();
            blendShader.end();
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
        
//        if(app->deltaMap[scale]<0.7)     app->deltaMap[scale] = 0.7;
//        if(app->deltaMap[scale]>2.7)    app->deltaMap[scale] = 2.7;
        
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
    
        if(shapeMeshNb != app->deltaMap[shapeNbPts])
            updateShape();
    
    }
    void mousePressed(int x, int y, int button){}

    void mouseDragged(int x, int y, int button){
    }
    void mouseMoved(int x, int y){}
    
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
    }
    
    void loadDirectImage(string path){
        events.push_back("changeImage");
        imageUzi=true;
        ofImage img;
        img.loadImage(path);
        img.resize(WIDTH,HEIGHT);
        imgs.push_back(img);
        imgChoice = imgs.size()-1;
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

            circleMask.loadImage("assets/maskCircle1280x720.png");

        circleMask.resize(app->FINALWIDTH, app->FINALHEIGHT);
    }

    void setResolution(){
        delete img;
        delete remanentFbo;
        delete source;
        delete source2;
        img = new ofImage;
        img->allocate(WIDTH, HEIGHT2, OF_IMAGE_COLOR);
        remanentFbo = new ofFbo;
        remanentFbo->allocate(WIDTH, HEIGHT2);
        source = new ofFbo;
        source->allocate(WIDTH, HEIGHT2);
        source2 = new ofFbo;
        source2->allocate(WIDTH, HEIGHT2);
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
            int h = (int( ofRandom(-app->parameterMap[tintAmp]/2,app->parameterMap[tintAmp]/2))*255 + 255) %  255 ;
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
    
    void setResolution(int res){
        delete img;
        delete remanentFbo;
        delete source;
        delete source2;
        img = new ofImage;
        img->allocate(WIDTH, HEIGHT2, OF_IMAGE_COLOR);
        remanentFbo = new ofFbo;
        remanentFbo->allocate(WIDTH, HEIGHT2);
        source = new ofFbo;
        source->allocate(WIDTH, HEIGHT2);
        source2 = new ofFbo;
        source2->allocate(WIDTH, HEIGHT2);
    }
    
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
    int shapeMeshNb = 0;
    vector<ofColor> randomColor;
    
    vector<EPoint> points;
    
    //NESTED
    Scene *nestedScene=0;
    ofShader alphaShader, blendShader;
    
    int shapeId = 0;
};

#endif /* defined(__emptyExample__Feedback__) */
