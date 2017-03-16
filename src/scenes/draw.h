

#ifndef emptyExample_Draw_h
#define emptyExample_Draw_h

#include <ofBaseTypes.h>
#include <string>
#include "cinema.h"
#include "AbstractApp.h"
#include "scene.h"
#include <stdio.h>
#include <stdlib.h>

//TODO intertie tant qu'il n'y a pas d'autre mouvement (leap)
//TODO agents ?

#define DRAWNPTS 1000.0 //default

#define MATCH 0
#define EXPLODE 1
#define IMPLODE 2

//Todo tout mettre dans un mesh
//record, + augmenter le gestes

enum TRANSFORMATIONTYPE {ANGLE, DISTANCE};
enum FRACTAL_TYPE {FRACTAL_LINE, FRACTAL_TRIANGLE, FRACTAL_VON_KOCH, FRACTAL_SQUARE};

//TODO
//BIPOINTS
//destroy current transited points (dest) (always destroy dest if interpolate >=1 ?)
//uneven pts number mapping
//transi mid ++ transi fewer points

//angular sorting I guess
static bool dist(ofVec3f a, ofVec3f b){
    return fmod(2*PI + atan2 (a.y,a.x), 2*PI) > fmod(2*PI + atan2 (b.y,b.x), 2*PI);
}

static bool dist_polar(ofVec3f a, ofVec3f b){
    return fmod(2*PI + a.x, 2*PI) > fmod(2*PI + b.x, 2*PI);
}

typedef vector<ofVec3f>::iterator iter;

class Draw: public Scene{
    
public:
    
    Draw(AbstractApp* app):Scene(app, "draw"){
        sceneID = "draw";

        back=true;
        load();
        computeDistances();
        
        trait.loadImage("assets/trait.png");
        
        src = &(allpts[0]);
        
        for(int i=0;i<currentDrawNb;i++){
            drawnPts.addVertex(ofVec3f(0,0));
            perturbations.push_back(ofVec3f(0,0));
        }
        
        preparePoints(1);
        
        
        if(importedMesh){
            string t = ofBufferFromFile("/Users/Raph/Desktop/muscletri.obj").getText();
                        vector<string > v1,v2,v3;
            vector<ofVec3f > vertices, normals, textures;
            if(t.compare("")){
                vector<string> lines = ofSplitString(t, "\n");
                for(string line: lines){
                    if(!line.substr(0,2).compare("v ")){
                        vector<string > fields = ofSplitString(line, " ");
                        if(fields.size()<4)
                            continue;
                        vertices.push_back(ofVec3f(ofToFloat(fields[1]), ofToFloat(fields[2]), ofToFloat(fields[3])));
                        continue;
                    }
                    if(!line.substr(0,2).compare("vn")){
                        vector<string > fields = ofSplitString(line, " ");
                        if(fields.size()<4)
                            continue;
                        normals.push_back(ofVec3f(ofToFloat(fields[1]), ofToFloat(fields[2]), ofToFloat(fields[3])));
   continue;
                    }
                    if(!line.substr(0,2).compare("vt")){
                        vector<string > fields = ofSplitString(line, " ");
                        if(fields.size()<3)
                            continue;
                        textures.push_back(ofVec2f(ofToFloat(fields[1]), ofToFloat(fields[2])));
 continue;
                    }
                    if(!line.substr(0,2).compare("f ")){
                        vector<string > fields = ofSplitString(line, " ");
                        if(fields.size()<4)
                            continue;
                        v1 = ofSplitString(fields[1], "/");
                        v2 = ofSplitString(fields[2], "/");
                        v3 = ofSplitString(fields[3], "/");
                        if(v1.size()==3 && v2.size()==3 && v3.size()==3){
                            ofMeshFace f;
                            f.setVertex(0, ofVec3f(ofToFloat(v1[0]), ofToFloat(v2[0]),ofToFloat(v3[0])));
                            f.setTexCoord(0, ofVec3f(ofToFloat(v1[1]), ofToFloat(v2[1]),ofToFloat(v3[1])));
                            f.setNormal(0, ofVec3f(ofToFloat(v1[2]), ofToFloat(v2[2]),ofToFloat(v2[3])));
                        } else{
                            if(v1.size()==2 && v2.size()==2 && !v3.empty()){
                                mesh.addVertex(vertices[ofToInt(v1[0])-1]);
                                mesh.addTexCoord(textures[ofToInt(v1[1])-1]);
                                
                                mesh.addVertex(vertices[ofToInt(v2[0])-1]);
                                mesh.addTexCoord(textures[ofToInt(v2[1])-1]);
                                
                                mesh.addVertex(vertices[ofToInt(v3[0])-1]);
                                mesh.addTexCoord(textures[ofToInt(v3[1])-1]);
                            }
                        }
                                                continue;
                    }
                }
            }
        }
        texture.loadImage("/Users/Raph/eqko/Projects/Azais/muscle_texture_2048.jpg");
    }
    
    void setup(){
    }
    
    void draw(){
        shapeMutex.lock();
        ofTranslate(0, (HEIGHT2-HEIGHT)/2);
        ofTranslate(0,0,-app->parameterMap[draw_oz]);
        ofTranslate(-app->parameterMap[draw_ox], -app->parameterMap[draw_oy], 0);
        
        ofSetColor(255,255,255);
        
        ofBackground(0);
//        ofSetColor(ofColor::black);
//        ofRect(-100000,-100000, WIDTH, HEIGHT);
//        ofRect(-100000,-100000, WIDTH+100000, HEIGHT+100000);
//        
        ofTranslate(WIDTH/2, HEIGHT/2);
        rotY += app->parameterMap[draw_rotY];
        ofRotateY(rotY);
        //        ofRotateY(ofGetFrameNum()%6<3 ? -20:20);
        ofSetColor(ofColor::white);
        int connections[(int)DRAWNPTS] = {0};
        int closest[(int)DRAWNPTS] = {0};
        
        float p = app->parameterMap[draw_points];
        
        
       /* if(importedMesh)
        {
            mesh.setMode(OF_PRIMITIVE_TRIANGLES);
//            mesh.setMode(OF_PRIMITIVE_POINTS);
//            mesh.setMode(OF_PRIMITIVE_LINES);
            ofScale(ofGetMouseX(), ofGetMouseY());
            ofRotateY(ofGetFrameNum());
            
            ofBackground(0);
            ofSetColor(ofColor::white);
            
            ofNoFill();
            ofEnableBlendMode(OF_BLENDMODE_ALPHA);
            
                        ofDisableArbTex();
            ofEnableNormalizedTexCoords();
            glEnable(GL_DEPTH_TEST);
            
            texture.bind();
            mesh.draw();
            texture.unbind();
            
            glDisable(GL_DEPTH_TEST);
            ofDisableNormalizedTexCoords();
                        ofEnableArbTex();
            
//            mesh.drawWireframe();
            
            shapeMutex.unlock();
            return;
        }*/
        

        if(preAnalysis){
            if(importedMesh){
                ofScale(50, 50);

                drawnPts.setMode(OF_PRIMITIVE_TRIANGLES);
                ofRotateY(ofGetFrameNum());
                
                ofBackground(0);
                ofSetColor(ofColor::white);
                
                ofNoFill();
                ofEnableBlendMode(OF_BLENDMODE_ALPHA);
                
                ofDisableArbTex();
                ofEnableNormalizedTexCoords();
                glEnable(GL_DEPTH_TEST);
                
                texture.bind();
                drawnPts.draw();
                texture.unbind();
                
                glDisable(GL_DEPTH_TEST);
                ofDisableNormalizedTexCoords();
                ofEnableArbTex();
                
                //            mesh.drawWireframe();
                
                shapeMutex.unlock();
                return;
            }
            mesh.clear();
            float dm = app->parameterMap[draw_minConnectionLength];
            float dM = app->parameterMap[draw_maxConnectionLength];

            for(int i=0;i<linePts1.getNumVertices();i++){
                if(p>2){
                    ofEllipse(linePts1.getVertex(i).x, linePts1.getVertex(i).y, linePts1.getVertex(i).z, p, p);
                }
                mesh.addVertex(linePts1.getVertex(i));
                mesh.addVertex(linePts2.getVertex(i));
                if(embedScene!=0 && app->parameterMap[embedCinema]==1){ // add z
                    mesh.addTexCoord((ofVec2f(linePts1.getVertex(i).x, linePts1.getVertex(i).y))+ofVec2f(WIDTH/2, HEIGHT/2));
                    mesh.addTexCoord((ofVec2f(linePts2.getVertex(i).x, linePts2.getVertex(i).y))+ofVec2f(WIDTH/2, HEIGHT/2));
                    if( app->parameterMap[draw_lines]==2){
                    ofVec3f v3 = (linePts1.getVertex(i)+linePts2.getVertex(i)) / 2 + ofVec3f(ofRandom(-1,1),ofRandom(-1,1),ofRandom(-1,1))*50;
                    mesh.addVertex(v3);
                    mesh.addTexCoord(ofVec2f(v3.x, v3.y)+ofVec2f(WIDTH/2, HEIGHT/2));
                    }
                
                }else{
                    float dbd = app->parameterMap[drawBrightDist];
                    if(dbd){
                    ofColor c1 = linePts1.getColor(i);
                    ofColor c2 = linePts2.getColor(i);
                        float dx;
                        float dd = (linePts1.getVertex(i).distance(linePts2.getVertex(i))-dm) / (dM-dm);
                        if(dbd==1)
                            dx =  255 - 200 * pow(dd,dbd);
                        else
                            dx =  55 + 200 * pow(dd,abs(dbd)+1);
                    c1.setBrightness(dx);
                    c2.setBrightness(dx);
                    mesh.addColor(c1);
                    mesh.addColor(c2);
                    }else{
                    mesh.addColor(linePts1.getColor(i));
                    mesh.addColor(linePts2.getColor(i));
                    }
                    
                                    }
            }
        }
        
//        for(int i=0;i<mesh.getNumVertices();i++){
//            if(abs(linePts1.getVertex(i).x / (float)ofGetWidth() + 0.5)<1
//               && abs(linePts1.getVertex(i).y / (float)ofGetHeight() + 0.5)<1
//               && abs(linePts2.getVertex(i).x / (float)ofGetWidth() + 0.5)<1
//               && abs(linePts2.getVertex(i).y / (float)ofGetHeight() + 0.5)<1){
//                app->ildaFrame.getLastPoly().lineTo(linePts1.getVertex(i).x / (float)ofGetWidth() + 0.5, linePts1.getVertex(i).y / (float)ofGetHeight() + 0.5);
//                app->ildaFrame.getLastPoly().lineTo(linePts2.getVertex(i).x / (float)ofGetWidth() + 0.5, linePts2.getVertex(i).y / (float)ofGetHeight() + 0.5);
//                cout << "v1 " << (linePts1.getVertex(i).x / (float)ofGetWidth() + 0.5)
//                << " " << (linePts1.getVertex(i).y / (float)ofGetHeight() + 0.5) << endl;
//            }
//        }
        
//        int i=-1;
//        for(int i=0;i<mesh.getNumVertices() && i<25;i++){
////            if(++i % 2 == 0)
////                app->ildaFrame.addPoly();
//            if(abs(mesh.getVertex(i).x / (float)ofGetWidth() + 0.5)<1
//               && abs(mesh.getVertex(i).y / (float)ofGetHeight() + 0.5)<1
//               ){
//                app->ildaFrame.getLastPoly().lineTo(mesh.getVertex(i).x / (float)ofGetWidth() + 0.5, mesh.getVertex(i).y / (float)ofGetHeight() + 0.5);
//            }
//        }

        
        if(app->parameterMap[draw_lines]){
            switch((int) app->parameterMap[draw_lines]){
                case 1: mesh.setMode(OF_PRIMITIVE_LINES);break;
                case 2: mesh.setMode(OF_PRIMITIVE_TRIANGLES);break;
            }
            ///test
            if(embedScene!=0 &&  embedScene->getTexture()!=0){
//            mesh.setMode(OF_PRIMITIVE_TRIANGLES);
            ofDisableArbTex();
//            ofEnableNormalizedTexCoords();
            glEnable(GL_DEPTH_TEST);
//            texture.bind();
            embedScene->getTexture()->bind();
            }
            
            mesh.draw();
            
            ///test
            if(embedScene!=0 &&  embedScene->getTexture()!=0){
//            texture.unbind();
            embedScene->getTexture()->unbind();
            glDisable(GL_DEPTH_TEST);
//            ofDisableNormalizedTexCoords();
            ofEnableArbTex();
            }
        }
        if(p==1){
            mesh.setMode(OF_PRIMITIVE_POINTS);
            mesh.draw();
            
        }
        
        shapeMutex.unlock();
        
        ofSetColor(ofColor::white);//cursor
        ofEllipse(cursorX-WIDTH/2 + app->deltaMap[draw_ox], cursorY-HEIGHT/2 + app->deltaMap[draw_oy],app->parameterMap[draw_oz],5,5);
        ofEllipse(leapX, leapY, app->parameterMap[draw_oz], 5, 5);
        ofSetColor(ofColor::white);
    }
    
    void mask(){}
    void capture(ofFbo* fbo){ }
    
    void update(){
        if(embedScene!=0)
           embedScene->update();
        
        if(app->parameterMap[draw_fresk] &&
           (ofDist(ofGetMouseX() - WIDTH/2, ofGetMouseY() - HEIGHT/2, 0, 0) > 2*HEIGHT/5)){
            float ea = app->parameterMap[draw_freskSpeed]*0.01;
            app->deltaMap[draw_ox] += (ofGetMouseX() - WIDTH/2)*ea;
            app->deltaMap[draw_oy] += (ofGetMouseY() - HEIGHT2/2)*ea;
        }
        
        app->deltaMap[draw_oz] -= app->parameterMap[draw_zSpeed];
        
        shapeMutex.lock();
        
        if(app->parameterMap[draw_destroy]){
            if(dest!=0 && (*dest).size()){
                switch((int) app->parameterMap[draw_destroyMode]){
                    case MATCH:{
                        int nb = max(5.0, (*dest).size()/10.0);
                        for(int i=0;i<nb;i++){
                            (*dest).erase(recPts.begin());
                            colors.erase(colors.begin());
                            if((*dest).size()==0)
                                break;
                        }
                        currentDrawNb = dest->size();
                    }break;
                    case EXPLODE:{
                        float amp = 5;
                        float f = 0.001;
                        for(int i=0;i<currentDrawNb && i<(*dest).size();i++){
                            (*dest)[i].scale((*dest)[i].length()*1.1);
                            //                            (*dest)[i].x += amp*(ofNoise(f*(*dest)[i].x, f*(*dest)[i].y)*2-1);
                            //                            (*dest)[i].y += amp*(ofNoise(f*(*dest)[i].y, f*(*dest)[i].x)*2-1);
                        }
                    }break;
                    case IMPLODE:{
                        float amp = 5;
                        float f = 0.001;
                        for(int i=0;i<currentDrawNb && i<(*dest).size();i++){
                            (*dest)[i].scale((*dest)[i].length()*0.9);
                        }
                    }break;
                }
                
                setAnalyzeLines();
            }else{
                app->deltaMap[draw_destroy] = 0; //end of destruction routine
                currentDrawNb = 0;
            }
        }
        
        if(dest!=0){
            vector<ofColor>::iterator i2 = colors.begin();
            for(iter it=dest->begin();it<dest->end();it++){
                i2++;
                if(it->distance(ofVec3f(app->parameterMap[draw_ox], app->parameterMap[draw_oy], app->deltaMap[draw_oz])) > HEIGHT*3/2){
                    *dest->erase(it);
                    colors.erase(i2);
                    setAnalyzeLines();
                }
            }
        }
        
        
        if(autoMode)
            app->parameterMap[draw_pace] = ofGetMouseY() / (float)(HEIGHT*10);
        
        if (app->parameterMap[draw_pace]<0) app->parameterMap[draw_pace] = 0;
        
        
        for(int i=drawnPts.getNumVertices();i<currentDrawNb;i++)//dynamic
            drawnPts.addVertex(ofVec3f(0,0,0));
        for(int i=perturbations.size();i<currentDrawNb;i++)//dynamic
            perturbations.push_back(ofVec3f(0,0));
        if(importedMesh){
            for(int i=perturbations.size();i<mesh.getNumVertices();i++)//dynamic
                perturbations.push_back(ofVec3f(0,0));
        }
        
        if(app->parameterMap[draw_random] && ofGetFrameNum() % (int)app->parameterMap[draw_randomRate] == 0){
            float rf = app->parameterMap[draw_random]*25;
            int n = currentDrawNb;
            if(importedMesh)
                n = perturbations.size();
            for(int i=0;i<n;i++){
                perturbations[i].x = ofRandom(-rf, rf);
                perturbations[i].y = ofRandom(-rf, rf);
                perturbations[i].z = ofRandom(-rf, rf);
            }
        }
        
        
        //interpolations between source network and dest network
        
        if(interpolate != -1){
            for(int i=0;i<currentDrawNb;i++){
                if(interpolateSteps==2){
                    drawnPts.setVertex(i, ofVec3f(ofLerp((*src)[indexMap[i]].x, (*dest)[i].x, interpolate), ofLerp((*src)[indexMap[i]].y, (*dest)[i].y, interpolate)));
                    
                }else{
                    if(interpolateSteps==3){
                        if(interpolate<0.5){
                            drawnPts.setVertex(i, ofVec3f(ofLerp((*src)[indexMap[i]].x, (*mid)[i].x, interpolate*2),
                                                      ofLerp((*src)[indexMap[i]].y, (*mid)[i].y, interpolate*2)));
                        }else{
                            drawnPts.setVertex(i, ofVec3f(ofLerp((*mid)[indexMap[i]].x, (*dest)[i].x, (interpolate-0.5)*2), ofLerp((*mid)[indexMap[i]].y, (*dest)[i].y, (interpolate-0.5)*2)));
                        }
                    }
                }
            }
            
            interpolate += app->parameterMap[draw_pace];
            
            if(interpolate>=1.0)
                interpolate = 1;
            
            if(autoMode && interpolate==1){
                interpolate = 0;
                loadNextPoints();
            }
            setAnalyzeLines();
        }else{ //TODO opt : dest == drawnPts
            if(importedMesh){
                drawnPts.clear();
                float f =ofGetFrameNum()%100/100.0 - 0.5;
                ofVec3f c = mesh.getCentroid();
                for(int i=0;i<mesh.getNumVertices();i++){
                    drawnPts.addVertex(mesh.getVertex(i)+ofRandom(0.1)); //TODO OPT modify vertex?
                    drawnPts.addTexCoord(mesh.getTexCoord(i)  + ofVec2f(f, f));
                }
            }else{
                for(int i=0;i<currentDrawNb && i<(*dest).size();i++){
                    drawnPts.setVertex(i, (*dest)[i]);
                }
            }
        }
        
        if((preAnalysis && analyzeLines && drawnPts.getNumVertices()>0) || 1==1){
            linePts1.clear();
            linePts2.clear();
            for(int i=0;i<currentDrawNb;i++){
                for(int j=i+1;j<currentDrawNb;j++){
                    float x2 = drawnPts.getVertex(i).x-drawnPts.getVertex(j).x;
                    float y2 = drawnPts.getVertex(i).y-drawnPts.getVertex(j).y;
                    float z2 = drawnPts.getVertex(i).z-drawnPts.getVertex(j).z;
                    float d = sqrt(x2*x2+y2*y2+z2*z2);
                    if(d<app->parameterMap[draw_maxConnectionLength] && d > app->parameterMap[draw_minConnectionLength] ){
                        linePts1.addVertex(drawnPts.getVertex(j) + perturbations[j]);
                        linePts2.addVertex(drawnPts.getVertex(i) + perturbations[i]);
                        linePts1.addColor(colors[j]);
                        linePts2.addColor(colors[i]);
//                        linePts1.addColor(ofColor::fromHsb(ofRandom(255), 255, 255));
//                        linePts2.addColor(ofColor::fromHsb(ofRandom(255), 255, 255));
                    }
                }
            }
            analyzeLines = false;
        }
        
        if(fractals){
            computeFractals();
        }
        
        shapeMutex.unlock();
    }
    
    void computeFractals(){
        vector<ofVec3f> add1, add2;
        for(int i=fractalIndex;i<linePts1.getNumVertices();i++){
            float d = linePts1.getVertex(i).distance(linePts2.getVertex(i));
            if(d<=1){ //fractal limit
                add1.push_back(linePts1.getVertex(i));
                add2.push_back(linePts2.getVertex(i));
                continue;
            }
            ofVec3f middle = (linePts1.getVertex(i)+linePts2.getVertex(i))/2.0;
            float amp;
            
            /** TRAIS **/
            switch((int) app->parameterMap[fractalMode]){
                case FRACTAL_LINE:{
                    amp = 0.6;
                    add1.push_back(middle);
                    add2.push_back(middle + (linePts2.getVertex(i)-linePts1.getVertex(i)).getRotated(90, ofVec3f(0,0,1))*amp);
                    add1.push_back(middle);
                    add2.push_back(middle + (linePts2.getVertex(i)-linePts1.getVertex(i)).getRotated(-90, ofVec3f(0,0,1))*amp);
                }break;
                case FRACTAL_TRIANGLE:{
                    amp = 0.4;
                    add1.push_back(linePts1.getVertex(i));
                    add2.push_back(middle + (linePts2.getVertex(i)-linePts1.getVertex(i)).getRotated(90, ofVec3f(0,0,1))*amp);
                    add1.push_back(linePts2.getVertex(i));
                    add2.push_back(middle + (linePts2.getVertex(i)-linePts1.getVertex(i)).getRotated(90, ofVec3f(0,0,1))*amp);
                    add1.push_back(linePts1.getVertex(i));
                    add2.push_back(middle + (linePts2.getVertex(i)-linePts1.getVertex(i)).getRotated(-90, ofVec3f(0,0,1))*amp);
                    add1.push_back(linePts2.getVertex(i));
                    add2.push_back(middle + (linePts2.getVertex(i)-linePts1.getVertex(i)).getRotated(-90, ofVec3f(0,0,1))*amp);
                }break;
                case FRACTAL_VON_KOCH:{
                    amp = 0.3333;
                    add1.push_back(linePts1.getVertex(i));
                    add2.push_back(linePts1.getVertex(i) + (linePts2.getVertex(i)-linePts1.getVertex(i))*1/3);
                    add1.push_back(linePts1.getVertex(i) + (linePts2.getVertex(i)-linePts1.getVertex(i))*1/3);
                    add2.push_back(middle + (linePts2.getVertex(i)-linePts1.getVertex(i)).getRotated(90, ofVec3f(0,0,1))*amp);
                    add1.push_back(middle + (linePts2.getVertex(i)-linePts1.getVertex(i)).getRotated(90, ofVec3f(0,0,1))*amp);
                    add2.push_back(linePts1.getVertex(i) + (linePts2.getVertex(i)-linePts1.getVertex(i))*2/3);
                    add1.push_back(linePts1.getVertex(i) + (linePts2.getVertex(i)-linePts1.getVertex(i))*2/3);
                    add2.push_back(linePts2.getVertex(i));
                    
                }break;
                case FRACTAL_SQUARE:{
                    amp = 0.3333;
                    add1.push_back(linePts1.getVertex(i));
                    add2.push_back(linePts1.getVertex(i) + (linePts2.getVertex(i)-linePts1.getVertex(i))/3);
                    add1.push_back(linePts1.getVertex(i) + (linePts2.getVertex(i)-linePts1.getVertex(i))/3);
                    add2.push_back(linePts1.getVertex(i)+(linePts2.getVertex(i)-linePts1.getVertex(i))/3.0 + (linePts2.getVertex(i)-linePts1.getVertex(i)).getRotated(90, ofVec3f(0,0,1))*amp);
                    add1.push_back(linePts1.getVertex(i)+(linePts2.getVertex(i)-linePts1.getVertex(i))/3.0 + (linePts2.getVertex(i)-linePts1.getVertex(i)).getRotated(90, ofVec3f(0,0,1))*amp);
                    add2.push_back(linePts1.getVertex(i)+(linePts2.getVertex(i)-linePts1.getVertex(i))*2/3.0 + (linePts2.getVertex(i)-linePts1.getVertex(i)).getRotated(90, ofVec3f(0,0,1))*amp);
                    add1.push_back(linePts1.getVertex(i)+(linePts2.getVertex(i)-linePts1.getVertex(i))*2/3.0 + (linePts2.getVertex(i)-linePts1.getVertex(i)).getRotated(90, ofVec3f(0,0,1))*amp);
                    add2.push_back(linePts1.getVertex(i) + (linePts2.getVertex(i)-linePts1.getVertex(i))*2/3);
                    add1.push_back(linePts1.getVertex(i) + (linePts2.getVertex(i)-linePts1.getVertex(i))*2/3);
                    add2.push_back(linePts2.getVertex(i));
                }break;
            }
            //1 mesh par fractales
        }
        
        fractalIndex = linePts1.getNumVertices();
        if(app->parameterMap[fractalMode]==FRACTAL_VON_KOCH || app->parameterMap[fractalMode]==FRACTAL_SQUARE){
            linePts1.clear();
            linePts2.clear();
            fractalIndex = 0;
        }
        for(int i=0;i<add1.size();i++){
            linePts1.addVertex(add1[i]);
            linePts2.addVertex(add2[i]);
        }
        fractalLevel++;
        fractals--;
    }
    
    void setAnalyzeLines(){
        analyzeLines = true;
        fractalIndex = 0;
        fractalLevel = 0;
        fractals = 0;
    }
    
    //matrice de similarité de voxels
    void computeDistances(){
        for(int a=0;a<allpts.size();a++){
            for(int b=a+1;b<allpts.size();b++){
                double distance = 0;
                vector<ofVec3f> v1;
                vector<ofVec3f> v2;
                for(int i=0;i<DRAWNPTS;i++){
                    v1.push_back(allpts[a][i]);
                    v2.push_back(allpts[b][i]);
                }
                
                int i = 0;
                // tempPts > destPts
                while(i<v2.size()){
                    float minDist = 10000;
                    int minI = 0;
                    for(int j=0;j<v1.size();j++){
                        float d = ofDist(v1[j].x, v1[j].y, v2[i].x, v2[i].y);
                        if(d<minDist && d>5){
                            minDist = d;
                            minI = j;
                        }
                    }
                    distance += minDist;
                    v1.erase(v1.begin() + minI);
                    v2.erase(v2.begin() + i);
                    i = 0;
                }
                distances[a][b] = distance/DRAWNPTS;
                distances[b][a] = distance/DRAWNPTS;
            }
        }
        cout << "Distances" << endl;
        for(int a=0;a<allpts.size();a++){
            for(int b=0;b<allpts.size();b++)
                cout << distances[a][b]  << " | ";
            cout << endl;
        }
    }
    
    //load indexed points db
    void preparePoints(int index){
        if(allpts.size()<=index)
            return;
        
        interpolate = 0;
        interpolateSteps = 2;
        
        app->parameterMap[draw_recording] = 0;
        app->parameterMap[draw_destruction] = 0;
        
        if(index>allpts.size())
            index = allpts.size()-1;
        lastIndexes.push_back(shapeIndex); //history
        while(lastIndexes.size()>lastIndexesSize)
            lastIndexes.erase(lastIndexes.begin());
        
        
        if(dest==0 || (*dest).size()==0){
            //            src = &(allpts[shapeIndex]); //must do a copy
            tempsrcPts.clear();
            for(int i=0;i<allpts[shapeIndex].size();i++)
                tempsrcPts.push_back(allpts[shapeIndex][i]);
            src = &tempsrcPts;
        }else{
            destToSrc();
        }
        
        tempdestPts.clear();
        dest = &(allpts[index]);
        
        if(src==0 || src->size()==0 || dest==0 || dest->size()==0)
            return;
        
        
        //if src has less pts than dest : pb
        for(int j=(*src).size();j<(*dest).size();j++)
            src->push_back((*src)[j%src->size()]);
        
        //transition by indexMap
        vector<bool> found((*src).size(), true);
        indexMap.clear();
        switch(transType){
            case ANGLE:{
                for(int j=0;j<(*src).size();j++)
                    indexMap.push_back(j);
            }
                break;
            case DISTANCE:{
                for(int j=0;j<(*dest).size();j++){
                    float minDist = 10000;
                    int minI = 0;
                    for(int i=0;i<(*src).size();i++){ //associate i to...
                        if(!found[i]) //1 to 1 match
                            continue;
                        float d = ofDist((*dest)[i].x, (*dest)[i].y, (*src)[j].x, (*src)[j].y);
                        if(d<minDist){
                            minDist = d;
                            minI = i;
                        }
                    }
                    
                    found[minI] = false;
                    indexMap.push_back(minI);
                }
            }
                break;
        }
        
        float sum = 0;
        float maxD = 0;
        
        shapeIndex = index;
        interpolate = 0;
        
        currentDrawNb = dest->size();
    }
    
    inline float distance(ofVec3f* a, ofVec3f* b){
        float d = ofDist(a->x, a->y, b->x, b->y) / (float)WIDTH;
        //float d = abs( ofDist(a->x, a->y, 0,0) / (float)WIDTH - ofDist(0,0, b->x, b->y) / (float)WIDTH);
        
        float angle = fmod(4*PI + atan2 (a->y,a->x) - atan2(b->y,b->x), 2*PI) / (2*PI);
        //return d*d + 10*angle;
        return angle;
    }
    
    //Used in auto mode only
    void loadNextPoints(){
        int futureIndex = 0;
        if(1==0){ //distance dependent
            int iMin = 0; //find closest shape
            double dMin = 100000;
            for(int i=0;i<50;i++){
                if(shapeIndex!=i && distances[shapeIndex][i]<dMin
                   && distances[shapeIndex][i]>0){
                    bool ok = true;//don't use previously used shapes
                    for(int j=0;j<lastIndexes.size();j++){
                        if(lastIndexes[j] == i){
                            ok = false;
                            break;
                        }
                    }
                    if(ok){
                        futureIndex = i;
                        dMin = distances[shapeIndex][i];
                    }
                }
            }
        }else{
            futureIndex = (shapeIndex + 1) % allpts.size();
        }
        preparePoints(futureIndex);
    }
    
    /** TRANSISTIONS **/
    
    void vMirror(){
        interpolate = 0;
        interpolateSteps = 2;
        
        //no history
        
        if(dest==0 || (*dest).size()==0){
            src = &(allpts[shapeIndex]);
        }else{
            destToSrc();
        }
        
        indexMap.clear();
        tempdestPts.clear();
        
        for(int i=0;i<DRAWNPTS;i++){
            tempdestPts.push_back(ofVec3f(-(*src)[i].x, (*src)[i].y));
            indexMap.push_back(i);
        }
        dest = &tempdestPts;
    }
    
    void doMidLine(){ //integrate to preparePoints
        interpolate = 0;
        interpolateSteps = 3;
        
        //no history
        
        if(dest==0 || (*dest).size()==0){
            src = &(allpts[shapeIndex]);
        }else{
            destToSrc();
        }
        
        indexMap.clear();
        tempdestPts.clear();
        
        for(int i=0;i<DRAWNPTS;i++){
            midLine.push_back(ofVec3f(0, (i-DRAWNPTS/2)*(DRAWNPTS/(float)(HEIGHT-50))));
            indexMap.push_back(i);
        }
        mid = &midLine;
        dest = &tempdestPts;
    }
    
    void destToSrc(){
        tempsrcPts.clear();
        for(int i=0;i<dest->size();i++)
            tempsrcPts.push_back((*dest)[i]);
        src = &tempsrcPts;
    }
    
    void load(){
        ofDirectory dir("draw/");
        dir.listDir();
        vector<string> dirs;
        for(int i=0;i<dir.numFiles();i++){
            string d = dir.getPath(i);
            if(d.find(".jpg")!=string::npos || d.find(".png")!=string::npos || d.find(".gif")!=string::npos||d.find(".JPG")!=string::npos || d.find(".PNG")!=string::npos || d.find(".GIF")!=string::npos){
                ofImage* i = new ofImage;
                i->allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR_ALPHA);
                i->loadImage(d);
                cout << "analyzing " << d << endl;
                if(i->width > i->height)
                    i->resize(WIDTH, i->height*WIDTH/i->width);
                else
                    i->resize(i->width*HEIGHT/i->height, HEIGHT);
                allImgs.push_back(i);
                
                vector<ofVec3f> p, ptemp, redp, p_polar;
                int count = 0;
                int x=0, y=0;
                //random until 300
                
                for(int y=0;y<i->height;y+=granularity){
                    for(int x=0;x<i->width;x+=granularity){
                        int xx = x;
                        int yy = y;
                        if(i->getColor(xx, yy).getSaturation()>200){
                            redp.push_back(ofVec3f(xx-i->width/2, yy-i->height/2));
                        }else{
                            if(i->getColor(xx, yy).getBrightness()<50){
                                // ptemp.push_back(ofVec3f(xx-i->width/2, yy-i->height/2));
                                ptemp.push_back(ofVec3f(xx-i->width/2, yy-i->height/2, ofDist(xx-i->width/2,yy-i->height/2,0,0))); //3D
                            }
                        }
                    }
                }
                
                int N = DRAWNPTS;
                if(redp.size()>N){
                    vector<ofVec3f> downsampled;
                    for(int i=0;i<N;i+=redp.size()/N)
                        downsampled.push_back(redp[i*redp.size()/N]);
                    redp = downsampled;
                }
                
                
                float xc = 0, yc = 0;
                for(int i=0;i<redp.size();i++){
                    p.push_back(redp[i]);
                    if(normalizeTraces){
                        xc += p[p.size()-1].x;
                        yc += p[p.size()-1].y;
                    }
                }
                for(int i=0;i<N-redp.size();i++){
                    p.push_back(ptemp[i*(ptemp.size()-redp.size())/DRAWNPTS]);
                    if(normalizeTraces){
                        xc += p[p.size()-1].x;
                        yc += p[p.size()-1].y;
                    }
                }
                if(normalizeTraces){
                    xc /= p.size();
                    yc /= p.size();
                    for(int i=0;i<p.size();i++){ //normalization
                        p[i].x -= xc;
                        p[i].y -= yc;
                    }
                }
                
                sort(p.begin(), p.end(), dist);
                
                for(int xx=0;xx<p.size();xx++)
                    p_polar.push_back(ofVec3f(ofDist(p[xx].x, p[xx].y,0,0), atan2(p[xx].x, p[xx].y), p[xx].z));
                
                allpts.push_back(p);
                allpts_polar.push_back(p_polar);
                cout << "recalled " << p.size() << " points (" << redp.size() << " red)" << endl;
            }else{
                if(d.find(".csv")!=string::npos){
                    vector<ofVec3f> p;
                    
                    string t = ofBufferFromFile(d).getText();
                    if(t.compare("")){
                        vector<string> lines = ofSplitString(t, "\n");
                        for(int x=0;x<lines.size();x++){
                            vector<string> ptstr = ofSplitString(lines[x], ",");
                            if(ptstr.size()==3)
                                p.push_back(ofVec3f(atoi(ptstr[0].c_str()),atoi(ptstr[1].c_str()),atoi(ptstr[2].c_str()) ));
                        }
                    }
                    
                    sort(p.begin(), p.end(), dist);
                    allpts.push_back(p);
                    //allpts_polar.push_back(p_polar);
                }
            }
        }
    }
    
    /** INTERACTION **/
    
    void mouseDragged(int x, int y, int button){
        recordPoint(x,y,0, ofColor::white);
        cursorX = ofGetMouseX();
        cursorY = ofGetMouseY();
    }
    
    void mouseMoved(int x, int y){
        cursorX = ofGetMouseX();
        cursorY = ofGetMouseY();
    }
    
    void touchMoved(ofTouchEventArgs &touch){
        if(touch.pressure==12345)
            destruction(MATCH);
        else{
            leapX = HEIGHT/2+ touch.x*HEIGHT/2;
            leapY = HEIGHT-touch.y*HEIGHT/2;
            recordPoint(HEIGHT/2+ touch.x*HEIGHT/2,HEIGHT-touch.y*HEIGHT/2,touch.pressure*HEIGHT/2,
                        ofColor::cyan);
        }
        cursorX = ofGetMouseX();
        cursorY = ofGetMouseY();
    }
    
    void moveCursor(int x, int y){
        cursorX = x;
        cursorY = y;
    }
    
    void recordPoint(int x, int y, int z, ofColor color){
        interpolate = -1;
        app->deltaMap[draw_destroy] = 0;
        
        ofVec3f v = ofVec3f(x-WIDTH/2 + app->deltaMap[draw_ox] , y-HEIGHT2/2 + app->deltaMap[draw_oy], z + app->deltaMap[draw_oz]);
        
        float len = recPts.size()==0 ? 100000 : recPts[recPts.size()-1].distance(v);
        
        if(len > app->parameterMap[draw_recInterval] || len < 0 || isnan(len)){
            if(app->parameterMap[draw_recording]==0){ //new recording
                app->parameterMap[draw_currentDrawing] ++;
                recPts.clear();
                colors.clear();
                dest = &recPts;
                app->deltaMap[draw_recording] = 1;
            }
            recPts.push_back(v);
            colors.push_back(color);
            setAnalyzeLines();
            stringstream str;
            str << "draw/testDB/" << app->parameterMap[draw_currentDrawing]  << ".csv";
            savePoints(str.str(), &recPts);
        }
        currentDrawNb = recPts.size();
    }
    
    void savePoints(string path, vector<ofVec3f>* pts){
        vector<ofVec3f> vec;
        ofFile file(path, ofFile::WriteOnly);
        
        if(!file.exists())
            file.create();
        ofBuffer b;
        stringstream str;
        
        
        for(ofVec3f p: (*pts)){
            str << p.x << "," << p.y << "," << p.z << "\n";
            vec.push_back(ofVec3f(p.x,p.y,p.z));
        }
        
        b.set(str.str());
        file.writeFromBuffer(b); //read only ??
        file.close();
        
        allDrawings.push_back(vec);
    }
    
    void loadPoints(string path, vector<ofVec3f>* pts){
        pts->clear();
        string t = ofBufferFromFile(path).getText();
        if(t.compare("")){
            vector<string > lines = ofSplitString(t, "\n");
            for(int l=0;l<lines.size();l++){
                float x, y, z;
                if(sscanf(lines[l].c_str(), "%f,%f,%f", &x, &y, &z)==3)
                    pts->push_back(ofVec3f(x,y,z));
            }
        }
    }
    
    //destroys and save
    void destruction(int mode){
        app->deltaMap[draw_destroy] = 1;
        app->deltaMap[draw_destroyMode]  = mode;
        app->deltaMap[draw_recording] = 0;
        interpolate = -1;
        sort(recPts.begin(), recPts.end(), dist);
        allpts.push_back(recPts);
        sort(allpts[allpts.size()-1].begin(), allpts[allpts.size()-1].end(), dist);
    }
    
    /** CONTROLS **/
    
    void keyPressed(int key){
        cout << key << endl;
        switch(key){
            case 'e':{
                destruction(EXPLODE);
            }break;
            case '\"':{
                destruction(MATCH);
            }break;
            case '\'':{
                destruction(IMPLODE);
            }break;
            case 'a':
                transType = ANGLE;
                break;
            case 'd':
                transType = DISTANCE;
                break;
            case 'g':
                preparePoints(int(ofRandom(allpts.size())));
                break;
            case 'r':
                app->parameterMap[draw_recording] = 0;
                break;
            case 'c':
                fractals = true;
                break;
            case 356:
                preparePoints((shapeIndex + allpts.size() - 1) % allpts.size());
                break;
            case 358:
                loadNextPoints();
                break;
            case 359:
                vMirror();
                break;
            case 357:
                app->parameterMap[draw_oz] -= 15;
                //                doMidLine();
                break;
        }
    }
    
    void midiEvent(ofxMidiMessage& eventArgs){
        float value = eventArgs.value;
        switch(eventArgs.channel){
            case 4:{
                switch(eventArgs.status){
                    case MIDI_CONTROL_CHANGE:{
                        switch(eventArgs.control){
                                app->deltaMap[draw_maxConnectionLength] = 50; //should be in pts
                                app->deltaMap[draw_pace]  = 0.05;
                                app->deltaMap[draw_divergence]  = 0;
                                
                                app->deltaMap[draw_destroy]  = 0;
                                app->deltaMap[draw_destroyMode]  = 0;
                                app->deltaMap[draw_recInterval]  = 7; //should be in pts
                                app->deltaMap[draw_currentDrawing]  = 0;
                                app->deltaMap[draw_random]  = 0.1;
                                app->deltaMap[draw_randomRate]  = 5;
                                app->deltaMap[draw_lines] = 1;
                                app->deltaMap[draw_points] = 0;
                                
                            case 1:
                                app->deltaMap[draw_maxConnectionLength] = 100*value/127.0 ;
                                analyzeLines = true;
                                break;
//                            case 2:
//                                app->parameterMap[draw_lines] = value<64 ;
//                                break;
//                            case 4:
//                                app->parameterMap[draw_random] = 3*value/127.0 ;
//                                break;
                                
                            case 5:
                                app->deltaMap[draw_zSpeed] = (value<60 || value>68) ? 50*(value-64)/127.0 : 0;
                                break;
                                
                            case 19:
                                transType = value<64 ? ANGLE : DISTANCE;
                                break;
                            case 18:
                                app->deltaMap[draw_pace] = (1+value)/127.0 * 0.25;
                                break;
                                
                        }
                    }break;
                    case MIDI_NOTE_ON:{
                        int pitch = eventArgs.pitch;
                        switch(pitch){
                            default:
                                if(pitch>=36 && pitch <=51){
                                    preparePoints(pitch - 36);
                                };
                        }
                    }break;
                    default:;
                }
            }break;
        }
    }
    void oscEvent(std::string header, std::string arg){}
    void oscEvent(std::string header, std::vector<float> args){}
    
    void saveMacro(stringstream *xml){
        (*xml) << "<draw>" << endl;
        (*xml) << "</draw>" << endl;
    }
    void loadMacro(ofXml *xml){
        
        
    }
    
    std::string getInfo(){
        return "";
    }
    
    void exit(){};
    
    Cinema* embedScene = 0;

    
protected:
    
    vector<ofVec3f>* src=0, *dest=0, *mid=0; //vectors
    
    vector<int> indexMap;
    
    ofMesh drawnPts;
    vector<ofVec3f> drawnPts_polar;
    vector<ofVec3f> perturbations;
    vector<ofVec3f> tempdestPts;
    vector<ofVec3f> tempsrcPts;
    float distances[50][50] = {-1};
    vector<int> lastIndexes; //graph memory
    int lastIndexesSize = 3; //size of graph memory
    
    vector<vector<ofVec3f> > allpts;
    vector<vector<ofVec3f> > allpts_polar;
    vector<ofImage* > allImgs;
    
    vector<vector<ofVec3f> > allDrawings;
    
    vector<ofVec3f> recPts;
    vector<ofColor> colors;
    
    //special vectors
    vector<ofVec3f> midLine;
    
    ofImage trait;
    
    int lastRecDate = -1;
    
    
    float interpolate = -1; //B4 zero , test
    
    /** ANALYSIS **/
    int granularity = 2;
    bool normalizeTraces = false;
    
    int shapeIndex = 0;
    
    int currentDrawNb = 0;
    
    int interpolateSteps = 2;
    
    
    TRANSFORMATIONTYPE transType = ANGLE;
    
    bool autoMode = false;
    
    ofMutex shapeMutex;
    ofMesh mesh;//using a mesh is waaay faster
    
    bool preAnalysis = true;
    bool analyzeLines = false;
    ofMesh linePts1;
    ofMesh linePts2;
    int fractals = 0; //number of fractal levels to be computed
    int fractalIndex = 0;
    int fractalLevel = 0;
    
    float rotY = 0;
    
    bool importedMesh = false;
    ofImage texture;
    
    float leapX=-1000,leapY=-10000;
    
    int cursorX = 0, cursorY=0;
    
    
};

#endif
