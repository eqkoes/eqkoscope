//
//  uzi.h
//  emptyExample
//
//  Created by Raphaël on 05/11/2014.
//
//

#ifndef emptyExample_uzi_h
#define emptyExample_uzi_h

#include "ofxMidi.h"
#include "scene.h"
#include "utils.h"
#include <ofBaseTypes.h>
#include <iostream>
#include <ofFbo.h>
#include <vector>
#include <string>
#include "ofMesh.h"

#define SWAP_TINT_PEAKS 5

using namespace std;

class Uzi : public Scene
{
    
public:
    Uzi(AbstractApp* app) : Scene(app, "uzi"){
        sceneID = "uzi";

        imgChanged = true;
        
        source = new ofFbo();
        source->allocate(WIDTH,HEIGHT, GL_RGBA);
        
        circleMask.loadImage("assets/maskCircle1280x720.png");
        circleMask.resize(WIDTH,HEIGHT);
        
        load();
        image = new ofImage;
        image->allocate(WIDTH,HEIGHT, OF_IMAGE_COLOR);
        image->loadImage(strdb[0][0]);
        imgChanged = true;
        
        swapTintShader.load("../shaders/swapTint");
        
        initParameters();
    }
    
    void setup(){
        forceLoad = true;
        if(dbIndex>=0 && dbIndex < strdb.size())
            app->blackNWhiteMedia = strdb[dbIndex][0].find("_b&w_")!=string::npos;
    }
    
    void exit(){}
    
    void draw(){
        if(image->width==0 || image->height==0)
            return;
        
        if(app->parameterMap[autoSwitchImg]){
            app->parameterMap[autoSwitchImg] =  app->deltaMap[autoSwitchImg] = 0;
            app->parameterMap[switchImg] = true;
            manualSwitchImg = true;
            int t = imgIndex;
            do {
                imgIndex = int(ofRandom(strdb.at(dbIndex).size()));
            }while (imgIndex == t && strdb.at(dbIndex).size()>1);
        }
        
        
        int w = (!app->parameterMap[resize] || image->width > image->height) ? WIDTH : image->width*HEIGHT/image->height;
        int h = (!app->parameterMap[resize] || image->width < image->height) ? HEIGHT : image->height*WIDTH/image->width;
        
        if(imgChanged ||
           (!sleep && (app->parameterMap[autoRot]>0 || app->parameterMap[ak47Mode] || 1==1)
           )){
            
            source->begin();
            if(app->parameterMap[transluscentUzi])
            ofBackground(ofColor(0,0,0,0));
else
            ofBackground(ofColor(0,0,0));
            if(app->parameterMap[ak47Mode] && !app->parameterMap[ak47Frame]){
            }else{
                ofPushMatrix();
                ofTranslate(WIDTH/2, HEIGHT/2);
                  if(!sleep){
                      ofTranslate(app->parameterMap[mediaX]*WIDTH, -app->parameterMap[mediaY]*HEIGHT, app->parameterMap[mediaZ]*HEIGHT);
                ofRotateX(app->parameterMap[mediaRotX]);
                ofRotateY(app->parameterMap[mediaRotY]);
                ofRotateZ(app->parameterMap[mediaRotZ]);
                  }
                
                if(app->parameterMap[randomUzi]>1){
                    int p = (int)max(1.0f, app->parameterMap[uziPeriod]);
                    ofTranslate(0,0, app->parameterMap[randomUzi]*(ofGetFrameNum()%p / float(p)));
                }
                
                if(app->parameterMap[vMirror])
                    ofScale(1,-1);
                if(app->parameterMap[hMirror])
                    ofScale(-1,1);
                
                if(app->parameterMap[autoRot]>0)
                    ofRotate(45*app->parameterMap[autoRot]*sin(app->parameterMap[autoRotFreq]*ofGetFrameNum()/15.0));
               
                if(app->parameterMap[resize])
                image->draw(-w/2, -h/2, (w), h);
                else
                    image->draw(-image->width/2, -image->height/2);

                ofPopMatrix();
                
                mesh.clear();
            }
            source->end();
            
            imgChanged = false;
            
            /*if(app->parameterMap[parallax]){
                mesh.clearColors();
                int res = app->parameterMap[parallax_res];
                for(int x=0;x<WIDTH;x+=res)
                    for(int y=0;y<HEIGHT2;y+=res){
                        int xx= x*w/WIDTH;
                        int yy= y*h/HEIGHT2;
                        ofColor c = image->getColor(xx,yy);
                        mesh.addColor(c);
                    }
            }*/
            
           /* if(app->parameterMap[swapTint]){
                int res = 10;
                
                for(int t=0;t<255;t++)
                    swapTintsIn[t] = 0;
                
                for(int x=0;x<image->width;x+=res)
                    for(int y=0;y<image->height;y+=res){
                        ofColor c = image->getColor(x,y);
                        swapTintsIn[(int) c.getHue()]++;
                    }
            }
            
            if(app->parameterMap[swapTint]){
                int res = 10;
                
                for(int t=0;t<255;t++)
                    swapTintsRef[t] = 0;
                
                for(int x=0;x<depth.width;x+=res)
                    for(int y=0;y<depth.height;y+=res){
                        ofColor c = depth.getColor(x,y);
                        swapTintsRef[(int) c.getHue()]++;
                    }
                vector<float> prevPeaksIn;
                for(int i=0;i<SWAP_TINT_PEAKS;i++){
                    bool found = false;
                    float maxIn = 0;
                    float tmaxIn = 0;
                    while (!found){
                        for(int t=0;t<255;t++){
                            if(swapTintsIn[t]>maxIn && find(prevPeaksIn.begin(), prevPeaksIn.end(), t)==prevPeaksIn.end()){
                                maxIn = swapTintsIn[t];
                                tmaxIn = t;
                                found = true; //maybe add distance guard
                            }
                        }
                    }
                    prevPeaksIn.push_back(tmaxIn);
                    swapTintPeaksIn[i] = tmaxIn;
                }
                vector<float> prevPeaksRef;
                for(int i=0;i<SWAP_TINT_PEAKS;i++){
                    bool found = false;
                    float maxRef = 0;
                    float tmaxRef = 0;
                    while (!found){
                        for(int t=0;t<255;t++){
                            if(swapTintsRef[t]>maxRef && find(prevPeaksRef.begin(), prevPeaksRef.end(), t)==prevPeaksRef.end()){
                                maxRef = swapTintsRef[t];
                                tmaxRef = t;
                                found = true; //maybe add distance guard
                            }
                        }
                    }
                    prevPeaksRef.push_back(tmaxRef);
                    swapTintPeaksRef[i] = tmaxRef;
                }
                for(int t=0;t<255;t++){
                    int inPeak = 0;
                    float dMin = 10000;
                    for(int p=0;p<SWAP_TINT_PEAKS;p++){
                        float d = abs(swapTintPeaksIn[p] - t);
                        if(dMin > d){
                            dMin = d;
                            inPeak = p;
                        }
                    }
                    int refPeak = 0;
                    dMin = 10000;
                    for(int p=0;p<SWAP_TINT_PEAKS;p++){
                        float d = abs(swapTintPeaksRef[p] - t);
                        if(dMin > d){
                            dMin = d;
                            refPeak = p;
                        }
                    }
                    swapTintMap[t] = fmod(255 + t + (swapTintPeaksRef[refPeak] - swapTintPeaksIn[inPeak]), 255);
                }
            }*/
        }
        
        ofTranslate(0, (HEIGHT2-HEIGHT)/2);
        
     /*   if(app->parameterMap[parallax]){
            int wd = (!app->parameterMap[resize] || depth.width > depth.height) ? WIDTH : depth.width*HEIGHT/depth.height;
            int hd = (!app->parameterMap[resize] || depth.width < depth.height) ? HEIGHT : depth.height*WIDTH/depth.width;
            
            mesh.clearVertices();
            float ffff = ofGetMouseX();
            float t = fmod(ofGetFrameNum() / 100.0, 1.0);
            int res = app->parameterMap[parallax_res];
            for(int x=0;x<WIDTH;x+=res)
                for(int y=0;y<HEIGHT2;y+=res){
                    int xx = x*wd/WIDTH;
                    int yy = y*hd/HEIGHT2;
                    float d = (depth.getColor(xx,yy).getBrightness()-50)/255 * ffff;// * (t-0.5); //can optimize getcolor
                    //                mesh.addVertex(ofVec3f(x+(t-0.5)*100, y, d));
                    mesh.addVertex(ofVec3f(x+(t-0.5)*d, y, 0));
                }
            mesh.setMode(OF_PRIMITIVE_POINTS);
            //        mesh.setMode(OF_PRIMITIVE_LINES);
        }*/
        
        imgMutex.unlock();
        
        
        if(app->parameterMap[swapTint]){
//            swapTintShader.load("../shaders/swapTint");
            swapTintShader.begin();
            swapTintShader.setUniform1fv("tintMap", swapTintMap);
        }
        source->draw(0,(HEIGHT2 - HEIGHT)/2);
        if(app->parameterMap[swapTint]){
            swapTintShader.end();
        }
        
        if(app->parameterMap[parallax])
            mesh.draw();
        
        
        
        if(scheduleSwitchImgOff){
            app->parameterMap[switchImg] = false;
            manualSwitchImg = false;
            scheduleSwitchImgOff = false;
        }
    }
    
    
    void capture(ofFbo* fbo){
        
    }
    
    void mask(){
        if (app->parameterMap[backMask])
            circleMask.draw(0,0, WIDTH, HEIGHT);
    }
    
    void update(){
        if (dbIndex >= strdb.size()) //range check
            dbIndex = strdb.size() - 1;
        if (imgIndex >= strdb.at(dbIndex).size())
            imgIndex = strdb.at(dbIndex).size() - 1;
        
//        if(directChange){
//            imgChanged = true;
//            directChange = false;
//        }else{
        if(!sleep || forceLoad){
                forceLoad = false;
                if ((app->parameterMap[randomUzi] && ofGetFrameNum()%((int)max(1.0f, app->parameterMap[uziPeriod])) == 0) || app->parameterMap[switchImg] || manualSwitchImg) {
                    app->blackNWhiteMedia = strdb[dbIndex][0].find("_b&w_")!=string::npos;
                    cout << "load " << strdb[dbIndex][imgIndex] << endl;
//                    long s = ofGetElapsedTimeMillis();
                    image->loadImage(strdb[dbIndex][imgIndex]);
                    currentMedia = strdb[dbIndex][imgIndex];
//                    cout << ofGetElapsedTimeMillis()-s << endl;
                    if(app->parameterMap[randomUzi] && strdb[dbIndex].size()>1){
                        int lastIndex = imgIndex;
                        do{
                            imgIndex = (int)ofRandom(strdb[dbIndex].size());
                        }while(lastIndex==imgIndex);
                        
                    }
                    app->parameterMap[_mask] = strdb[dbIndex][imgIndex].find("_round_")!=string::npos;
                    imgChanged = true;
                    scheduleSwitchImgOff = true;
                    app->parameterMap[switchImg] = app->deltaMap[switchImg] = false;
                }else{
                    if (app->parameterMap[nextImg]) { //put dymanic load ?
                        imgIndex = (imgIndex + 1) % strdb.at(dbIndex).size();
                        imgMutex.lock();
                        image->loadImage(strdb[dbIndex][imgIndex]);
                        currentMedia = strdb[dbIndex][imgIndex];
                        app->parameterMap[_mask] = strdb[dbIndex][imgIndex].find("_round_")!=string::npos;
                        imgMutex.unlock();
                        imgChanged = true;
                        app->parameterMap[nextImg] = false;
                    }
                }
            }
//        }
    }
    
    void mousePressed(int x, int y, int button){}
void mouseDragged(int x, int y, int button){
    }
    void mouseMoved(int x, int y){}

    void touchMoved(ofTouchEventArgs &touch){
        
    }
    
    void midiEvent(ofxMidiMessage& eventArgs){
        float value = eventArgs.value;
        switch(eventArgs.channel){
            case 1:{
                if(eventArgs.status==MIDI_CONTROL_CHANGE){
                    switch(eventArgs.control){
                        case 105:{
                            if(eventArgs.value>0)
                                manualSwitch();
                        }break;
//            case 106:{
//                uziSwitch();
//            if(eventArgs.value==0)
//                app->deltaMap[randomUzi] = false;
//            }break;
//                        case 107:app->deltaMap[randomUzi] = true;break;
            }
            }break;
            }
            case 3: case 4:{
                switch(eventArgs.status){
                    case MIDI_CONTROL_CHANGE:{
                        switch(eventArgs.control){
                            case 9:{
                                app->deltaMap[uziPeriod] = app->parameterMap[uziPeriod] = 1 + (int) (30*value/127);
                            }break;
                            case 16:{
                                app->parameterMap[autoRot] = (value/127.0);
                            }break;
                            case 17:{
                                app->parameterMap[autoRotFreq] = 5*(value/127.0);
                            }break;
                            case 47:
                                if(value==127)
                                    app->parameterMap[ak47Mode] = !app->parameterMap[ak47Mode];
                                break;
                        }
                    }break;
                    
                    case MIDI_NOTE_ON:{
                        switch(eventArgs.pitch){
                            case 44:
                                app->deltaMap[randomUzi] = !app->deltaMap[randomUzi];
                                if(app->parameterMap[ak47Mode])
                                    app->deltaMap[ak47Frame] = app->deltaMap[randomUzi];
                                break;
                            case 48:
                                uziSwitch(); break;
                            case 49:{
                                manualSwitch(); break;
                                
                            default:
                                if (eventArgs.pitch >=36 && eventArgs.pitch <46) {
                                    if(!app->parameterMap[ak47Mode]){ //change dB
                                        if(eventArgs.pitch==36){ //change only imgIndex
                                            int nb = eventArgs.pitch - 37;
                                            if(nb > strdb.size()-1)
                                                nb = strdb.size()-1;
                                            dbIndex = nb;
                                        }
                                        app->deltaMap[switchImg] = true;
                                        manualSwitchImg = true;
                                        int t = imgIndex;
                                        do {
                                            imgIndex = int(ofRandom(strdb.at(dbIndex).size()));
                                        }
                                        while (imgIndex == t && strdb.at(dbIndex).size()>1);
                                        break;
                                    }else{
                                        app->deltaMap[ak47Frame] = true;
                                        app->deltaMap[switchImg] = true;
                                        manualSwitchImg = true;
                                        int nb = eventArgs.pitch - 36;
                                        if(nb > strdb[dbIndex].size()-1)
                                            nb = strdb[dbIndex].size()-1;
                                        imgIndex = nb;
                                        app->deltaMap[lastAk47Index] = nb;
                                    }
                                }
                        }
                    }break;
                    case MIDI_NOTE_OFF:{
                        switch(eventArgs.pitch){
                            case 48:
                                app->deltaMap[randomUzi] = false;
                                app->deltaMap[ak47Frame] = false;
                                break;
                            case 49:
                                app->deltaMap[randomUzi] = false;
                                app->deltaMap[ak47Frame] = false;
                                break;
                            default:
                                if (eventArgs.pitch >=36 && eventArgs.pitch <48) {
                                    if(app->parameterMap[ak47Mode]){
                                        int nb = eventArgs.pitch - 36;
                                        if(nb > strdb.size()-1)
                                            nb = strdb.size()-1;
                                        if(app->parameterMap[lastAk47Index]==nb && !app->parameterMap[randomUzi])
                                            app->deltaMap[ak47Frame] = false;
                                    };
                                }
                        }
                    }break;
                    default:;
                }
            }break;
            default:;
        }
    }

    }
    
    void uziSwitch(){
        app->deltaMap[switchImg] = true;
        manualSwitchImg = true;
        app->deltaMap[randomUzi] = true;
        if(app->parameterMap[ak47Mode])
            app->deltaMap[ak47Frame] = true;
    }
    
    void manualSwitch(){
        app->deltaMap[switchImg] = true;
        manualSwitchImg = true;
        int t = imgIndex;
        do {
            imgIndex = int(ofRandom(strdb.at(dbIndex).size()));
        }
        while (imgIndex == t && strdb.at(dbIndex).size()>1);
        cout << imgIndex << endl;
    }
    
    void load(){
        ofDirectory dir("uzi/");
        dir.listDir();
        vector<string> dirs;
        for(int i=0;i<dir.numFiles();i++){
            string d = dir.getPath(i);
            if(d.find("DB")!=string::npos){
                vector<string> strs;
                vector<ofImage*> imgs;
                ofDirectory dir2(d);
                dir2.listDir();
                for(int j=0;j<dir2.numFiles();j++){
                    string picpath = dir2.getPath(j);
                    if(picpath.find(".jpg")!=string::npos || picpath.find(".png")!=string::npos || picpath.find(".gif")!=string::npos||picpath.find(".JPG")!=string::npos || picpath.find(".PNG")!=string::npos || picpath.find(".GIF")!=string::npos
                       || picpath.find(".tiff")!=string::npos){
                        strs.push_back(picpath);
                    }
                }
                strdb.push_back(strs);
                imgdb.push_back(imgs);
            }
        }
        
        depth.loadImage("./uzi/dali.jpg");
    }
    
    void loadDirectImage(string path){
        image->loadImage(path);
        currentMedia = path;
        imgChanged = true;
        
        bool found = false;
        for(int db=0;db<strdb.size();db++){
            for(int i=0;i<strdb[db].size();i++){
                ofFile f1(path);
                ofFile f2(strdb[db][i]);
                if(!f1.getFileName().compare(f2.getFileName())){
                    app->parameterMap[switchImg] = true;
                    manualSwitchImg = true;
                    dbIndex = db;
                    imgIndex = i;
                    found = true;
                    break;
                }
            }
            if(found)
                break;
        }
    }
    
    void keyPressed(int key){
        switch(key){
            case ' ':{
                app->parameterMap[autoSwitchImg] = app->deltaMap[autoSwitchImg] =  1;
               
            }
                break;
        }
    }
    
    void oscEvent(std::string header, std::string arg){}
    void oscEvent(std::string header, std::vector<float> args){}
    
    void saveMacro(stringstream *xml){
        (*xml) << "<uzi>" << endl;
        
        saveParameterXML(xml, "imgpath", strdb[dbIndex][imgIndex]);
        
        (*xml) << "</uzi>" << endl;
    }
    void loadMacro(ofXml *xml){
        
        if(xml->getAttribute("noSleep").compare("")){
            sleep = false;
        }else
            sleep = wasSleep;
        
        string path = xml->getValue("imgpath");
        bool found = false;
        for(int db=0;db<strdb.size();db++){
            for(int i=0;i<strdb[db].size();i++){
                ofFile f1(path);
                ofFile f2(strdb[db][i]);
                if(!f1.getFileName().compare(f2.getFileName())){
                    app->parameterMap[switchImg] = true;
                    manualSwitchImg = true;
                    forceLoad = true;
                    dbIndex = db;
                    imgIndex = i;
                    found = true;
                    currentMedia = path;
                    break;
                }
            }
            if(found)
                break;
            
            app->parameterMap[autoSwitchImg] = app->deltaMap[autoSwitchImg] = false;
        }
    }
    
    string getCurrentMedia(){
        return currentMedia;
    }
    
    void setDirty(){
        imgChanged = true;
    }
    
    void setSleep(int s){
        sleep = s;
        wasSleep = s;
    }
    
    std::string getInfo(){
        return "";
    }
    
    void setResolution(int res){
        delete source;
        delete image;
        
        source = new ofFbo();
        image = new ofImage;
        
        source->allocate(WIDTH,HEIGHT, GL_RGBA);
        image->allocate(WIDTH,HEIGHT, OF_IMAGE_COLOR);
    }
    
private:
    ofMutex imgMutex;
    
    ofFbo* source;
    ofImage img;
    ofImage circleMask;
    string currentMedia;

    ofMesh mesh;
    ofImage depth;
    
    
    //VIDEO
    std::vector<std::vector<string> > strdb;
    std::vector<std::vector<ofImage*> > imgdb;
    ofImage *image=0;
    int dbIndex = 0;
    int imgIndex = 0;
    
    ofImage roundMaskImg;
    
    bool scheduleSwitchImgOff = false;
    bool manualSwitchImg = false;
    bool imgChanged = false;
    
    float swapTintsIn[255];
    float swapTintsRef[255];
    float swapTintPeaksIn[SWAP_TINT_PEAKS];
    float swapTintPeaksRef[SWAP_TINT_PEAKS];
    float swapTintMap[255];
    ofShader swapTintShader;
    
    int sleep = 0;
    bool wasSleep = false;
    
    bool directChange = false;
    
    bool                 forceLoad = false;
    
    
};

#endif
