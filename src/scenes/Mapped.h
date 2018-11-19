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
#include "distortion.h"

#define WHITE 0
#define BLINK 1
#define BLINK_ONE 19
#define RAND_ 2
#define LINERAND 3
#define LINESYNC 4
#define GLITCH 5
#define RANDCOLOR 6
#define BREATH 7
#define ELLIPSE 8
#define NESTED 9
#define EXPAND 10
#define FILL_SEQ 11
#define FILL_SEQ_CENTER 12
#define COCHON 101
#define EVT 100
#define THOUGHTS 200

#define NO_SYNC 0
#define SYNC_ALL 1
#define SYNC_INDIV 2
#define SYNC_SEQ 3

#define SHAPE_WHITE 0
#define SHAPE_RAY 1
#define SHAPE_EXP 2

// moirages sur un dos + moirages projetés

class Mapped: public Scene
{
public:
    Mapped(AbstractApp* app):Scene(app, "mapped"){
        sceneID = "mapped";
        
        maskShader.load("../shaders/mask");
        blendShader.load("../shaders/blend");
        erodeShader.load("../shaders/erode");
        
        this->app = app;
        load();
        
        for(int i=0;i<maps.size();i++){
            maskedMaps[i].begin();
            ofBackground(0);
            maskedMaps[i].end();
        }
        
        mainFbo.allocate(WIDTH,HEIGHT, GL_RGB);
        shapeFbo.allocate(WIDTH,HEIGHT, GL_RGB);
        maskFbo.allocate(WIDTH,HEIGHT, GL_RGB);
        
        for(int i=0;i<maps.size();i++){
            transitions.push_back(0);
        }
        i.allocate(WIDTH,HEIGHT, OF_IMAGE_COLOR);
    }
    
    
    void setup(){
        //        if(nestedScene!=0)
        //            nestedScene->setup();
    }
    
    void exit(){
    }
    
    //redraw on event
    void draw(){
        
        if(dotCalibration && calibrationStage>=1){
            drawCalibration();
            return;
        }
        
        if((app->parameterMap[map_pace]>=0) || app->parameterMap[map_event]){
            app->parameterMap[map_event] = 0;
            
            ofPushMatrix();
            mainFbo.begin();
            ofBackground(0);
            
            int n = (app->parameterMap[map_sync] == SYNC_INDIV) ? maps.size() : 1; //number of fill fbos
            
            switch(int(app->deltaMap[map_mode])){
                case WHITE:{
                    for(int i=0;i<maps.size();i++)
                        dmapColors[i] = 255;
                }break;
                case BLINK:{
                    for(int i=0;i<maps.size();i++)
                        dmapColors[i] = ofColor((ofRandom(1)<0.5) * 255);
                }break;
                case BLINK_ONE:{
                    int the_index = ofRandom(maps.size());
                    for(int i=0;i<maps.size();i++)
                        dmapColors[i] = ofColor((i==the_index) * 255);
                }break;
                case RANDCOLOR:{
                    for(int i=0;i<maps.size();i++)
                        dmapColors[i] = ofColor::fromHsb(ofRandom(255), ofRandom(180,255), 255);
                }break;
                case RAND_:{
                    for(int i=0;i<maps.size();i++)
                        dmapColors[i] = ofColor(ofRandom(255));
                }break;
                case FILL_SEQ:{
                    for(int i=0;i<maps.size();i++)
                        dmapColors[i] = ofColor((app->parameterMap[map_prog] > i/(float)(maps.size()+1)) ? 255 : 0);
                }break;
                case FILL_SEQ_CENTER:{
                    for(int i=0;i<maps.size();i++){
                        dmapColors[i] = ofColor((app->parameterMap[map_prog]/2.0 > abs(ofDist((mapBoxes)[i].x, (mapBoxes)[i].y, WIDTH/2, HEIGHT/2))/float(WIDTH)) ? 255 : 0);
                    }
                }break;
                case 20:{
                    for(int i=0;i<maps.size();i++){
                        //                        dmapColors[i] = ofColor((ofDist(app->parameterMap[user1]*WIDTH, app->parameterMap[user2]*HEIGHT, (mapBoxes)[i].x, (mapBoxes)[i].y) < HEIGHT/4) ? 255 : 0);
                        dmapColors[i] = ofColor(ofMap(ofDist(app->parameterMap[user1]*WIDTH, app->parameterMap[user2]*HEIGHT, (mapBoxes)[i].x, (mapBoxes)[i].y), 0, HEIGHT/4, 255, 0, true));            //TODO MODE SIN WAVES !
                    }
                }break;
                case 21:{
                    for(int i=0;i<maps.size();i++){
                        dmapColors[i] = ofColor((abs(ofDist((mapBoxes)[i].x, (mapBoxes)[i].y, (mapBoxes)[i].x+(mapBoxes)[i].width, (mapBoxes)[i].y+(mapBoxes)[i].height) - app->parameterMap[user1]*HEIGHT/4)<app->parameterMap[user2]*HEIGHT/4) ? 255 : 0);
                    }
                }break;
                case 22:{
                    for(int i=0;i<maps.size();i++){
                        dmapColors[i] = ofColor((ofDist(app->parameterMap[user1]*WIDTH, app->parameterMap[user2]*HEIGHT, (mapBoxes)[i].x, (mapBoxes)[i].y) / float( HEIGHT/4)) * 255);            //TODO MODE SIN WAVES !
                    }
                }break;
                case EVT:{
                    for(int i=0;i<maps.size();i++)
                        dmapColors[i] *= .95f - app->parameterMap[user2];
                    
                    if(app->deltaMap[user4]>0){
                        for(int i=0;i<=dmapColors.size()/3;i++)
                            dmapColors[ofRandom(dmapColors.size())] = ofColor(255);
                        app->deltaMap[user4] =0;
                    }
                    if(event>0 || app->deltaMap[user3]>0){
                        for(int i=0;i<=app->parameterMap[user1];i++)
                            dmapColors[ofRandom(dmapColors.size())] = ofColor(255);
                        event --;
                        app->deltaMap[user3] --;
                    }
                }break;
                case COCHON:{
                    for(int i=0;i<maps.size();i++)
                        dmapColors[i] *= .95f - app->parameterMap[user2];
                    
                    if(app->deltaMap[user4]>0){
                        for(int i=0;i<=dmapColors.size()/3;i++)
                            dmapColors[ofRandom(dmapColors.size())] = ofColor(255);
                        app->deltaMap[user4] =0;
                    }
                    if(event>0 || app->deltaMap[user3]>0){
                        for(int i=0;i<=app->parameterMap[user1];i++)
                            dmapColors[ofRandom(dmapColors.size())] = ofColor(255);
                        event --;
                        app->deltaMap[user3] --;
                    }
                    
                    for(int i=0;i<=maps.size()/25;i++){
                        //                        float delta = ofClamp(ofColor(ofRandom(25,75)), 0, 75 - dmapColors[ofRandom(dmapColors.size())]);
                        //                            dmapColors[ofRandom(dmapColors.size())] += ofColor(delta);
                        //                        dmapColors[ofRandom(dmapColors.size())] = ofColor(ofRandom(25, 50));
                    }
                    
                }break;
                    
                case THOUGHTS:{
                    for(int i=0;i<maps.size();i++)
                        dmapColors[i] *= .95f;
                    
                    int alpha = app->parameterMap[user1] * dmapColors.size();
                    int diameter = app->parameterMap[user2] * dmapColors.size();
                    for(int i = alpha - diameter/2 ; i < alpha + diameter/2 ; i++)
                        if(i>0 && i<dmapColors.size())
                            dmapColors[(int) i ] = ofColor(255 * abs(sin(i + ofGetFrameNum()/100.0)));
                }break;
            }
            
            mainFbo.end();
            ofPopMatrix();
        }
        
//        renderShape(NESTED, 0);
       renderShape(WHITE, 0);
        
        for(int i=0;i<maps.size();i++)
            if( mapColors[i] != dmapColors[i] )
                mapColors[i] = mapColors[i].lerp(dmapColors[i], 1-app->parameterMap[map_remanence]);
        
        /// fill the maskedMaps
        for(int i=0;i<maps.size();i++){
            if( mapColors[i].getBrightness() > 0 ){
                maskedMaps[i].begin();
                
                ofSetColor(mapColors[i]);
                //            (maps)[i].draw(0,0);
                
                ofPushMatrix();
                ofRotateZ(i%25 * 360/25);
                shapeFbo.draw(-mapBoxes[i].x - i%10 * 10,
                              -mapBoxes[i].y - (i*3)%10 * 10);
                ofPopMatrix();
                
                maskShader.begin();
                (maps)[i].draw(0,0);
                maskShader.end();
                
                maskedMaps[i].end();
            }
        }
        
        
        ///draw the mainFbo
        mainFbo.begin();
        ofBackground(0,0,0);
        ofSetColor(ofColor::white);
        for(int i=0;i<maskedMaps.size();i++)
            if( mapColors[i].getBrightness() > 0 )
            {
                ofPushMatrix();
                ofTranslate((WIDTH - img.getWidth()) / 2 + WIDTH/2,
                            (HEIGHT - img.getHeight()) / 2 + HEIGHT/2);
                
                ofTranslate(app->parameterMap[mediaX]*WIDTH, -app->parameterMap[mediaY]*HEIGHT, app->parameterMap[mediaZ]*HEIGHT);
                ofRotateX(app->parameterMap[mediaRotX]);
                ofRotateY(app->parameterMap[mediaRotY]);
                ofRotateZ(app->parameterMap[mediaRotZ]);
                
                if(app->parameterMap[divergence]>0)
                    ofTranslate(HEIGHT * app->parameterMap[divergence] * (ofNoise(mapBoxes[i].x, mapBoxes[i].y)-0.5),
                                HEIGHT * app->parameterMap[divergence] * (ofNoise(mapBoxes[i].y, mapBoxes[i].x)-0.5));
                
                blendShader.begin();
                blendShader.setUniform1i("mode", 100);
                blendShader.setUniform1f("thresh", 0.05);
                (maskedMaps)[i].draw((mapBoxes)[i].x - WIDTH/2, (mapBoxes)[i].y - HEIGHT/2);
                blendShader.end();
                ofPopMatrix();
            }
        mainFbo.end();
        
        ofSetColor(ofColor::white);
        
        ofBackground(0);
        
        mainFbo.draw(0,0);
    }
    
    void renderShape(int shape, int n){
        shapeFbo.begin();
        switch(abs(shape)){
            case SHAPE_RAY:{
                ofBackground(0);
                ofSetColor(ofColor::white);
                for(int i=0;i<25;i++)
                    ofDrawRectangle(0, app->parameterMap[map_prog]*10*HEIGHT/25 + i*HEIGHT/25, (WIDTH), (HEIGHT)/50);
            }break;
            case WHITE:case EXPAND:{
                ofBackground(255);
            }break;
            case RANDCOLOR:{
                ofSetColor(app->getRandomColor());
                // ofBackground(app->getRandomColor());
                ofDrawRectangle(0, 0, (WIDTH), (HEIGHT));
                ofSetColor(ofColor::white);
            }break;
            case LINERAND:{
                ofBackground(ofColor(0,0,0,(1-app->parameterMap[map_remanence])*255));
                ofSetColor(ofColor::white);
                ofDrawRectangle(0, ofRandom(7*HEIGHT/8), WIDTH, ofRandom(HEIGHT/8));
            }break;
            case LINESYNC:{
                ofBackground(ofColor(0,0,0,(1-app->parameterMap[map_remanence])*255));
                ofSetColor(ofColor::white);
                ofDrawRectangle(0, app->parameterMap[map_prog] * 7*HEIGHT/8, WIDTH, HEIGHT/8);
            }break;
            case GLITCH:{
                ofBackground(ofColor(0,0,0,(1-app->parameterMap[map_remanence])*255));
                for(int i=0;i<30;i++){
                    ofSetColor(ofRandom(1)<0.5?0:255);
                    int x = ofRandom(WIDTH/2);
                    int y = ofRandom(WIDTH/2);
                    ofDrawRectangle(x, y, ofRandom(WIDTH-x), ofRandom(HEIGHT-y));
                }
            }break;
            case BREATH:{
                ofBackground(0);
                ofSetColor(ofColor::white);
                float w = app->parameterMap[map_prog]*(25+(app->parameterMap[map_sync]==NO_SYNC?n*3:0)) * WIDTH;
                ofDrawRectangle(WIDTH/2-w/2, 0, w, HEIGHT);
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
                    ofDrawRectangle(0, 0, WIDTH, HEIGHT);
            }break;
        }
        shapeFbo.end();
    }
    
    void mask(){
        if(calibrationStage<1){
            
            if(ofGetFrameNum()==3){ //don't know why I have to do this async. don't sweat it, don't delete it.
                maskFbo.begin();
                ofBackground(0,0,0);
                blendShader.begin();
                blendShader.setUniform1i("mode", 0);
                //                    blendShader.setUniform1f("_mix", 1);
                blendShader.setUniform1f("thresh", 0.5);
                ofSetColor(ofColor::white);
                for(int i=0;i<maps.size();i++)
                    (maps)[i].draw((mapBoxes)[i].x, (mapBoxes)[i].y);
                blendShader.end();
                maskFbo.end();
            }
            if(app->parameterMap[map_mask]>0){
                ofTranslate((WIDTH - img.getWidth()) / 2,
                            (HEIGHT - img.getHeight()) / 2);
                
                maskShader.begin();
                maskFbo.draw(0,0);
                maskShader.end();
            }
        }
    }
    
    void capture(ofFbo* fbo){
        
    }
    void update(){
        if(app->parameterMap[map_pace] > 0)
            app->deltaMap[map_prog] += app->deltaMap[map_pace];
        else
            if(app->parameterMap[map_pace] == 0)
                app->deltaMap[map_prog] += app->deltaMap[map_event] * 1 / (float) (maps.size());
        //        else
        //            app->parameterMap[map_prog] += app->parameterMap[bpm]*60;
        
        app->deltaMap[map_prog] = fmod(app->deltaMap[map_prog], 1);
        
        app->parameterMap[map_prog] = app->deltaMap[map_prog];
        
        if(nestedScene!=0)
            nestedScene->update();
    }
    
    void randomEvent(){ }
    
    void load(){
        
        originalmaps.clear();
        originalmapBoxes.clear();
        
        ofDirectory dir("maps/");
        dir.listDir();
        
        int count = 0, fboCount = 0;
        for(int i=0;i<dir.size();i++){
            string picpath = dir.getPath(i);
            if(picpath.find(".jpg")!=string::npos
               || picpath.find(".png")!=string::npos
               || picpath.find(".gif")!=string::npos
               || picpath.find(".JPG")!=string::npos
               || picpath.find(".PNG")!=string::npos
               || picpath.find(".GIF")!=string::npos
               || picpath.find(".tiff")!=string::npos)
            {
                if(picpath.find("#.png")==string::npos)
                {
                    count ++;
                    this->picpath = picpath;
                }
            }
        }
        
        for(int i=0;i<dir.size();i++){
            string path = dir.getPath(i);
            if(path.find(".jpg")!=string::npos
               || path.find(".png")!=string::npos
               || path.find(".gif")!=string::npos
               || path.find(".JPG")!=string::npos
               || path.find(".PNG")!=string::npos
               || path.find(".GIF")!=string::npos
               || path.find(".tiff")!=string::npos)
            {
                if(path.find("#.png")!=string::npos)
                {
                    if( ofFile(path).getBaseName().find(ofFile(picpath).getBaseName()) != string::npos )
                        fboCount ++;
                    else
                        ofFile(path).remove();
                }
            }
        }
        
        
        if(count==1){
            this->picpath = picpath;
            img.loadImage(picpath);
            ofSetColor(ofColor::white);
            if( fboCount > 0 ){
                resizeImg(&img);
                string csvTxt = ofBufferFromFile(csvpath).getText();
                vector <string> csvLines = ofSplitString(csvTxt, "\n");
#ifdef VERBOSE
                cout << "Loading mapping textures from " << picpath << "..." << endl;
#endif
                for(int i=0;i<dir.size();i++){
                    string path = dir.getPath(i);
                    if(path.find("#.png")!=string::npos){
                        int minX, minY, maxX = 0, maxY = 0;
                        vector <string> split_ = ofSplitString(path, "#");
                        vector <string> champs = ofSplitString(split_[split_.size()-2], "_");
                        minX = ofToInt(champs[champs.size()-4]);
                        minY = ofToInt(champs[champs.size()-3]);
                        maxX = ofToInt(champs[champs.size()-2]);
                        maxY = ofToInt(champs[champs.size()-1]);
                        ofFbo fbo;
                        fbo.allocate(maxX-minX, maxY-minY);
                        ofImage img;
                        img.loadImage(path);
                        //                        cout << "Loading mapping texture " << path << endl;
                        fbo.begin();
                        ofSetColor(255,0,0,1);
                        ofDrawRectangle(0,0,maxX,maxY);
                        ofSetColor(ofColor::white);
                        img.draw(0,0);
                        fbo.end();
                        originalmaps.push_back(fbo);
                        originalmapBoxes.push_back(ofRectangle(minX,minY,maxX-minX,maxY-minY));
                    }
                }
                maps = originalmaps;
                mapBoxes = originalmapBoxes;
                initValues();
            }else{
                if(autoCalibration || dotCalibration)
                {
                    calibrate(&img);
                    if(dotCalibration)
                        return; //waiting for user input
                }
                resizeImg(&img);
                analyzeRawImg(0.2);
            }
        }else{
            autoCalibration = false;
            dotCalibration = false;
            for(int i=0;i<dir.size();i++){
                ofImage img;
                int x=10000,y=10000,w=-1,h=-1;
                string picpath = dir.getPath(i);
                if(picpath.find(".jpg")!=string::npos || picpath.find(".png")!=string::npos || picpath.find(".gif")!=string::npos||picpath.find(".JPG")!=string::npos || picpath.find(".PNG")!=string::npos || picpath.find(".GIF")!=string::npos
                   || picpath.find(".tiff")!=string::npos){
                    img.loadImage(picpath);
                    resizeImg(&img);
                    for(int xx=0;xx<img.getWidth();xx++){
                        for(int yy=0;yy<img.getHeight();yy++){
                            if(img.getColor(xx,yy).getBrightness()>15){
                                x = min(x,xx);
                                y = min(y,yy);
                            }
                        }
                    }
                    for(int xx=img.getWidth()-1;xx>=0;xx--){ //analysis (size)
                        for(int yy=img.getHeight()-1;yy>=0;yy--){
                            if(img.getColor(xx,yy).getBrightness()>15){
                                w = max(w, xx);
                                h = max(h, yy);
                            }
                        }
                    }
                    ofFbo fbo;
                    fbo.allocate(img.getWidth(), img.getHeight(), GL_RGB);
                    fbo.begin();
                    ofBackground(255, 0, 0);
                    img.draw(0,0);
                    fbo.end();
                    originalmaps.push_back(fbo);
                    originalmapBoxes.push_back(ofRectangle(x,y,w-x,h-y));
                }
            }
            
            maps = originalmaps;
            mapBoxes = originalmapBoxes;
            
            initValues();
        }
    }
    
    void analyzeRawImg(float threshold){
        
        DEBUG_TIME_1
        int resolution = 2;
        int minZone = 100;
        int maxZone = 10000;
        originalmaps.clear();
        originalmapBoxes.clear();
        
        int offsetX = (WIDTH - img.getWidth()) / 2;
        int offsetY = (HEIGHT - img.getHeight()) / 2;
        
        
        bool visited[(int)img.getWidth()][(int)img.getHeight()];
        int visitedCount = 0;
        
        float hueThreshold = 0.05;
        
        for(int xx=0;xx<img.getWidth();xx+=resolution)
            for(int yy=0;yy<img.getHeight();yy+=resolution)
                visited[xx][yy] = false;
        while(visitedCount<img.getWidth()/resolution * img.getHeight()/resolution && originalmaps.size()<10000){
            int curX = -1, curY = -1;
            int minX, minY, maxX = 0, maxY = 0;
            int color[(int)img.getWidth()][(int)img.getHeight()]; // 0 : not visited, 1: white, 2: analyzed neighbors
            for(int xx=0;xx<img.getWidth();xx+=resolution)
                for(int yy=0;yy<img.getHeight();yy+=resolution)
                    color[xx][yy] = 0;
            bool ok = false;
            vector<ofPoint> whitePts;
            ofColor crtColor;
            for(int xx=0;xx<img.getWidth();xx+=resolution){ //search for one blob
                if(whitePts.size()>=maxZone)
                    break;
                for(int yy=0;yy<img.getHeight();yy+=resolution){
                    if(whitePts.size()>=maxZone)
                        break;
                    if(!visited[xx][yy]){
                        visited[xx][yy] = true;
                        visitedCount++;
                        bool white;
                        //                        white = img.getColor(xx,yy).getLightness()>threshold*255; //old
                        if(curX<0){
                            white = (includeWhite || img.getColor(xx,yy).getSaturation()>0.2*255)
                            && img.getColor(xx,yy).getLightness()>threshold*255;
                            if(curX<0){ //first encounter
                                crtColor = img.getColor(xx,yy);
                            }
                        }
                        //                        if(!analyzeWhite) white = !white;
                        if(curX<0){ //first encounter
                            if(white){
                                ok = true; //found blob
                                curX = minX = xx;
                                curY = minY = yy;
                                maxX = 0;
                                maxY = 0;
                                color[xx][yy] = 1;
                                whitePts.push_back(ofPoint(xx,yy));
                                bool found = true;
                                while(found){
                                    found = false;
                                    for(int ax=0;ax<img.getWidth();ax+=resolution){ //scan all pixels in the zone
                                        if(whitePts.size()>=maxZone)
                                            break;
                                        for(int ay=0;ay<img.getHeight();ay+=resolution){
                                            if(whitePts.size()>=maxZone)
                                                break;
                                            switch(color[ax][ay]){
                                                case 2:continue;
                                                case 1://analyse neighbors
                                                    for(int x=-resolution;x<=resolution;x+=resolution){
                                                        if(whitePts.size()>=maxZone)
                                                            break;
                                                        for(int y=-resolution;y<=resolution;y+=resolution){
                                                            if((x==0 && y==0) || ax+x < 0 || ax+x >= img.getWidth() || ay+y < 0 || ay+y >= img.getHeight())
                                                                continue;
                                                            ofColor c = img.getColor(x+ax,y+ay);
                                                            white = (includeWhite || c.getSaturation()>0.2*255) && (c.getLightness() > 0.8*255
                                                                                                                    || (c.getLightness() > threshold*255
                                                                                                                        && abs(c.getHue() - crtColor.getHue()) < hueThreshold*255));
                                                            if(white && color[x+ax][y+ay]==0){ //OPT
                                                                color[x+ax][y+ay] = 1;
                                                                found = true;
                                                                minX = min(minX, x+ax);
                                                                minY = min(minY, y+ay);
                                                                maxX = max(maxX, x+ax);
                                                                maxY = max(maxY, y+ay);
                                                                if(!visited[x+ax][y+ay]){
                                                                    visited[x+ax][y+ay] = true;
                                                                    visitedCount++;
                                                                }
                                                                whitePts.push_back(ofPoint(x+ax,y+ay));
                                                                //no boucle !
                                                                for(int xxx=1;xxx<resolution;xxx++)
                                                                    for(int yyy=1;yyy<resolution;yyy++){
                                                                        whitePts.push_back(ofPoint(x+ax+xxx,y+ay+yyy));
                                                                        whitePts.push_back(ofPoint(x+ax,y+ay+yyy));
                                                                        whitePts.push_back(ofPoint(x+ax+xxx,y+ay));
                                                                        whitePts.push_back(ofPoint(x+ax-xxx,y+ay-yyy));
                                                                        whitePts.push_back(ofPoint(x+ax,y+ay-yyy));
                                                                        whitePts.push_back(ofPoint(x+ax-xxx,y+ay));                                                                    }
                                                                if(whitePts.size()>=maxZone)
                                                                    break;
                                                            }
                                                        }
                                                    }
                                                    continue;
                                                case 0: continue;
                                            }
                                        }
                                    }
                                    color[xx][yy] = 2; //end of analysis for this pixel
                                }
                                break;
                            }else{
                                continue;
                            }
                        }
                    }
                }
                if(ok) break;
            }
            //add mapped zones
            if(curX>0 && (maxX-minX > 0 || maxY-minY > 0)
               && whitePts.size()>minZone && whitePts.size()<maxZone){
                ofFbo fbo;
                fbo.allocate(maxX-minX, maxY-minY);
                fbo.begin();
                ofBackground(0,0,0);
#ifdef VERBOSE
                cout << "adding mapping zone " << whitePts.size() << endl;
#endif
                for(ofPoint pt : whitePts){
                    if(useRealColors)
                        ofSetColor(img.getColor(pt.x, pt.y));
                    ofDrawRectangle(pt.x - minX, pt.y - minY, 1, 1);
                }
                fbo.end();
                ofImage saveImg;
                ofPixels pix;
                pix.clear();
                fbo.readToPixels(pix);
                pix.mirror(1, 0);
                saveImg.setFromPixels(pix);
                saveImg.saveImage(picpath+"#"
                                  + ofToString(originalmaps.size())
                                  + "_"
                                  + ofToString(minX)
                                  + "_"
                                  + ofToString(minY)
                                  + "_"
                                  + ofToString(maxX)
                                  + "_"
                                  + ofToString(maxY)
                                  + "#.png");
                originalmaps.push_back(fbo);
                originalmapBoxes.push_back(ofRectangle(minX, minY, maxX-minX, maxY-minY));
                ofSetColor(ofColor::white);
                
                fbo.draw(minX, minY);
            }
        }
        
        maps = originalmaps;
        mapBoxes = originalmapBoxes;
        
        initValues();
        DEBUG_TIME_2
    }
    
    
    void postTreatment(){
        
    }
    
    void loadDirectImage(string path){
        load();
    }
    
    void initValues(){
        
        maskFbo.begin();
        ofBackground(0,0,0);
        ofSetColor(ofColor::white);
        for(int i=0;i<maps.size();i++)
            (maps)[i].draw((mapBoxes)[i].x, (mapBoxes)[i].y);
        maskFbo.end();
        
        mapColors.clear();
        dmapColors.clear();
        mapColors.resize(maps.size());
        dmapColors.resize(maps.size());
        for(int i=0;i<maps.size();i++){
            mapColors[i] = ofColor::black;
            dmapColors[i] = ofColor::black;
        }
        
        maskedMaps.clear();
        for(int i=0;i<maps.size();i++){
            maskedMaps.push_back(ofFbo());
            maskedMaps[i].allocate(mapBoxes[i].width, mapBoxes[i].height);
        }
        
    }
    
    void subdivideMasks(float sub){
        std::vector<ofFbo> maps2;
        std::vector<ofRectangle> mapBoxes2;
        int count = maps.size();
        for(int i=0;i<count;i++){
            for(int j=0;j<sub;j++){
                ofFbo fbo;
                fbo.allocate((mapBoxes)[i].width, (mapBoxes)[i].height/sub, GL_RGB);
                ofBackground(0, 0, 0);
                ofPushMatrix();
                fbo.begin();
                ofBackground(0,0,0, 0);
                (maps)[i].draw(0, - (mapBoxes)[i].height/sub*j);
                fbo.end();
                ofPopMatrix();
                maps2.push_back(fbo);
                mapBoxes2.push_back(ofRectangle((mapBoxes)[i].x,(mapBoxes)[i].y + (mapBoxes)[i].height/sub*j,(mapBoxes)[i].width,(mapBoxes)[i].height/sub));
            }
        }
        maps = maps2;
        mapBoxes = mapBoxes2;
        initValues();
    }
    
    void erode(){
        std::vector<ofFbo> maps2;
        std::vector<ofRectangle> mapBoxes2;
        int count = maps.size();
        for(int i=0;i<count;i++){
            ofFbo fbo;
            fbo.allocate((mapBoxes)[i].width, (mapBoxes)[i].height, GL_RGB);
            ofBackground(0, 0, 0);
            ofPushMatrix();
            fbo.begin();
            ofBackground(0,0,0, 0);
            erodeShader.begin();
            (maps)[i].draw(0,0);
            erodeShader.end();
            fbo.end();
            ofPopMatrix();
            maps2.push_back(fbo);
            //                mapBoxes2.push_back(mapBoxes);
        }
        maps = maps2;
        //        mapBoxes = mapBoxes2;
        initValues();
        
    }
    
    void drawCalibration(){
        ofBackground(0);
        ofDrawBitmapString("Waiting for Calibration Input...", WIDTH/2, HEIGHT/2);
        ofSetColor(ofColor::yellow);
        //        ofEllipse(ofGetMouseX()-5, ofGetMouseY()-5, 11, 11);
        ofEllipse(ofGetMouseX(), ofGetMouseY(), 10, 10);
        ofSetColor(ofColor::black);
        //        ofEllipse(ofGetMouseX()-4, ofGetMouseY()-4, 9, 9);
        ofEllipse(ofGetMouseX(), ofGetMouseY(), 9, 9);
        ofSetColor(ofColor::yellow);
        //        ofEllipse(ofGetMouseX()-1, ofGetMouseY()-1, 3, 3);
        ofEllipse(ofGetMouseX(), ofGetMouseY(), 1, 1);
        ofSetColor(ofColor::red);
        if(calibrationStage<=2){
            ofEllipse(x3, y3, 7, 7);
            ofSetColor(ofColor::blue);
            ofEllipse(x3, y3, 3, 3);
            
            ofPushMatrix();
            ofSetColor(ofColor::white);
            float scaleX = (x4)/(x2-x1);
            float scaleY = (y4)/(y2-y1);
            ofScale(scaleX, scaleY);
            ofTranslate(x3/scaleX-x1, y3/scaleY-y1);
            img.draw(0,0);
            //                    int xx = (x-x3)/scaleX +x1 ;
            //                    int yy = (y-y3)/scaleY +y1 ;
            ofPopMatrix();
        }
        if(calibrationStage<=1){
            ofEllipse(x4+x3, y4+y3, 7, 7);
        }
        ofSetColor(ofColor::white);
    }
    
    void calibrate(ofImage *img){
        if(dotCalibration){
            switch(calibrationStage){
                case -1:{            //auto-detecter les gommettes
                    detectDots(img);
                    calibrationStage = 2;
                }break;
                case 0:{
                    ofImage dest;
                    dest.allocate(WIDTH,HEIGHT,OF_IMAGE_COLOR);
                    float scaleX = (x4)/(x2-x1);
                    float scaleY = (y4)/(y2-y1);
                    for(int y=0;y<img->getHeight();y++){
                        for(int x=0;x<img->getWidth();x++){
                            int xx = (x-x3)/scaleX +x1 ;
                            int yy = (y-y3)/scaleY +y1 ;
                            xx = ofClamp(xx, 0, WIDTH-1);
                            yy = ofClamp(yy, 0, HEIGHT-1);
                            dest.setColor(x, y, img->getColor(xx, yy));
                        }
                    }
                    dest.update();
                    *img = dest;
                }break;
            }
            
            //manuellement insérer les gomettes
            
        }else{
            //analysis
            analyzeRectangle(img);
            //anamorphosis
            ofImage dest;
            dest.allocate(WIDTH,HEIGHT,OF_IMAGE_COLOR);
            for(int y=0;y<HEIGHT;y++){
                for(int x=0;x<WIDTH;x++){
                    //                    int xx = ofMap(y, 0, HEIGHT, x1 + x*(x2-x1)/WIDTH, x3 + x*(x4-x3)/WIDTH); //TODO
                    //                    int yy = ofMap(x, 0, WIDTH, y1 + y*(y3-y1)/HEIGHT, y2 + y*(y4-y2)/HEIGHT);
                    //                    yy = ofClamp(yy, 0, HEIGHT-1);
                    int xx = ofMap(x, 0, WIDTH-1, ofMap(y, 0, HEIGHT-1, x1, x3) , ofMap(y, 0, HEIGHT-1, x2, x4), true);
                    int yy = ofMap(y, 0, HEIGHT-1, ofMap(x, 0, WIDTH-1, y1, y2) , ofMap(x, 0, WIDTH-1, y3, y4), true);
                    dest.setColor(x, y, img->getColor(xx, yy));
                }
            }
            dest.update();
            img->resize(WIDTH, HEIGHT);
#ifdef OF_10
            img->setFromPixels(dest.getPixels());
#else
            img->setFromPixels(dest.getPixels(), WIDTH, HEIGHT, OF_IMAGE_COLOR);
#endif
            img = new ofImage(dest);
        }
    }
    
    void resizeImg(ofImage *img){
        if(img->getWidth() != WIDTH || img->getHeight()!= HEIGHT){
            int w = (img->getWidth() > img->getHeight()) ? WIDTH : img->getWidth()*HEIGHT/img->getHeight();
            int h = (img->getWidth() < img->getHeight()) ? HEIGHT : img->getHeight()*WIDTH/img->getWidth();
            img->resize(w,h);
        }
    }
    
    void detectDots(ofImage* img){
        img->resize(WIDTH,HEIGHT);
        bool found1=false, found2=false;
        for(int y=0;y<img->getHeight();y++){
            for(int x=0;x<img->getWidth();x++){
                float h = img->getColor(x,y).getHue();
                if((h < 10 || h > 250) && img->getColor(x,y).getSaturation()>180
                   && img->getColor(x,y).getBrightness()>180){
                    if(!found1){
                        x1 = x;
                        y1 = y;
                        found1 = true;
                    }else{
                        if(!found2 && ofDist(x, y, x1, y1) > 50){ //@arbitrary
                            x2 = x;
                            y2 = y;
                            found2 = true;
                        }
                    }
                    if(found1 && found2)
                        break;
                }
            }
            if(found1 && found2)
                break;
        }
    }
    
    // from white rectangle
    // detremine x3, y3, x4, y4
    void analyzeRectangle(ofImage *img){
        float minDist = 100000;
        float thresh = 127;
        for(int y=0;y<img->getHeight()/2;y++){
            for(int x=0;x<img->getWidth()/2;x++){
                bool isWhite = img->getColor(x, y).getBrightness()>thresh;
                float d = ofDist(x, y, 0, 0);
                if(isWhite && d<minDist){
                    minDist = d;
                    x1 = x;
                    y1 = y;
                }
            }
        }
        minDist = 100000;
        for(int y=img->getHeight()/2;y<img->getHeight();y++){
            for(int x=0;x<img->getWidth()/2;x++){
                bool isWhite = img->getColor(x, y).getBrightness()>thresh;
                float d = ofDist(x, y, 0, img->getHeight());
                if(isWhite && d<minDist){
                    minDist = d;
                    x3 = x;
                    y3 = y;
                }
            }
        }
        minDist = 100000;
        for(int y=0;y<img->getHeight()/2;y++){
            for(int x=img->getWidth()/2;x<img->getWidth();x++){
                bool isWhite = img->getColor(x, y).getBrightness()>thresh;
                float d = ofDist(x, y, img->getWidth(), 0);
                if(isWhite && d<minDist){
                    minDist = d;
                    x2 = x;
                    y2 = y;
                }
            }
        }
        minDist = 100000;
        for(int y=img->getHeight()/2;y<img->getHeight();y++){
            for(int x=img->getWidth()/2;x<img->getWidth();x++){
                bool isWhite = img->getColor(x, y).getBrightness()>thresh;
                float d = ofDist(x, y, img->getWidth(), img->getHeight());
                if(isWhite && d<minDist){
                    minDist = d;
                    x4 = x;
                    y4 = y;
                }
            }
        }
    }
    
    void mouseDragged(int x, int y, int button){
        app->deltaMap[user1] = ofGetMouseX()/float(WIDTH);
        app->deltaMap[user2] = ofGetMouseY()/float(HEIGHT);
        
        //        if(x>WIDTH/2){
        //            x4 = x;
        //            y4 = y;
        //            calibrationStage = 0;
        //            calibrate(&img);
        //            analyzeRawImg(0.5);
        //        }else{
        //        x3 = x;
        //        y3 = y;
        //        calibrationStage = 0;
        //        calibrate(&img);
        //        analyzeRawImg(0.5);
        //        }
    }
    
    void mousePressed(int x, int y, int  button){
        if(dotCalibration && calibrationStage>=1){
            switch(calibrationStage){
                case 2:{
                    x3 = x;
                    y3 = y;
                    calibrationStage--;
                }break;
                case 1:{
                    x4 = x-x3;
                    y4 = y-y3; //wait for ENTER input
                }break;
            }
        }
    }
    
    void mouseMoved(int x, int y){
        if(dotCalibration && calibrationStage>=1){
            switch(calibrationStage){
                case 2:{
                    x3 = x;
                    y3 = y;
                }break;
                case 1:{
                    if(app->keyIsDown[OF_KEY_SHIFT]){
                        x3 = x;
                        y3 = y;
                    }else{
                        if(app->keyIsDown[OF_KEY_COMMAND]){
                            x4 = x-x3;
                            y4 = y-y3;
                        }
                    }
                }break;
            }
        }
    }
    
    void touchMoved(ofTouchEventArgs &touch){}
    
    void keyPressed(int key){
        switch(key){
            case '-':scale-=0.05;break;
            case '_':scale+=0.05;break;
            case ':':  analyzeRawImg(ofGetMouseX()/float(WIDTH)); break;
            case 'X': subdivideMasks(2);break;
            case '"': erode();break;
            case ' ':{
                if(calibrationStage==1){
                    calibrationStage--;
                    calibrate(&img);
                    analyzeRawImg(0.1);
                }else{
                    event += 10;
                }
            }break;
            case 'r': load(); break;
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
    
    void loadMacro(TiXmlHandle *xml){
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
    
    std::vector<ofFbo> maps;
    std::vector<ofFbo> maskedMaps;
    std::vector<ofRectangle> mapBoxes;
    std::vector<ofColor> mapColors;
    std::vector<ofColor> dmapColors;
    
    std::vector<ofFbo> originalmaps;
    std::vector<ofRectangle> originalmapBoxes;
    ofShader maskShader;
    ofFbo mainFbo;
    ofFbo maskFbo;
    ofFbo shapeFbo;
    
    std::vector<float> transitions; //TODO
    ofImage i;
    
    bool autoCalibration = false; // from the white rectangle
    bool analyzeWhite = false; //white as foreground color
    bool dotCalibration = false; // with red dots
    int calibrationStage = -1;
    float x1,x2,x3,x4=WIDTH;
    float y1,y2,y3,y4=HEIGHT;
    ofImage img;
    string picpath = "", csvpath = "";
    
    ofShader erodeShader;
    
    bool useRealColors = true;
    bool includeWhite = false;
    
    int shape = NESTED;
    ofShader blendShader;
    
    float event = 0;
};

#endif /* defined(__BORING_ANGEL__Mapped__) */
