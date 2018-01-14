//
//  three.h
//  eqkoscope
//
//  Created by Raphaël on 02/12/2015.
//
//

#ifndef eqkoscope_three_h
#define eqkoscope_three_h

#include "abstractapp.h"
#include "scene.h"
#include <ofMesh.h>
#include <ofTypes.h>

using namespace std;

class Three: public Scene
{
public:
    Three(AbstractApp* app)
    :Scene(app, "three"){
        sceneID = "three";
        cam = ofEasyCam();
        
        for(int i=0;i<1000;i++){
            points.push_back(ofVec3f(ofRandom(-1,1),ofRandom(-1,1),ofRandom(-1,1)));
            lines.push_back(ofVec3f(ofRandom(-1,1),ofRandom(-1,1),ofRandom(-1,1)));
            lines.push_back(ofVec3f(ofRandom(-1,1),ofRandom(-1,1),ofRandom(-1,1)));
        }
        
        sphere = ofMesh::icosphere(5);
        
        light.setAmbientColor(ofColor::red);
        light.setPointLight();
        light.setDiffuseColor(ofColor::cyan);
        light.setAttenuation(0,0,0);
        light.setup();
        
        material.setShininess(120);
        
        material.setSpecularColor(ofColor(255, 255, 255, 255));
        material.setEmissiveColor(ofColor(0, 0, 0, 255));
        material.setDiffuseColor(ofColor(255, 255, 255, 255));
        material.setAmbientColor(ofColor(255, 255, 255, 255));
        
//        app->featuredParameter = user1;

        
        app->addCommand("NOTEON,user2,0.1/2,0.8,2,36/52 random", true); //bass
        app->addCommand("NOTEON,pert,0/2,0.2,1,38 random,transcient", true); //batt
        app->addCommand("NOTEON,warp,0/0.4,1,1,38 random", true); //batt
        app->addCommand("NOTEON,user3,0/1,0.5,1,44 transcient", true); //batt
    

    
    }
    
    void setup(){
        app->deltaMap[pertEvoAuto] = 0.01;
        app->deltaMap[pertFreq] = 3;
        app->deltaMap[sobel] = 0.5;
        app->deltaMap[user1] = 0.4;
        
//        loadMesh();
        
       mesh = mesh.icosphere(1);
        ofVec3f o(-0.3, -0.3, 0.2);
        for(int i=0;i<mesh.getVertices().size();i++){
            float v = (mesh.getVertex(i) - o).length();
            mesh.addColor(ofColor(0*v, 155*v, 55*v));
        }
    }
    void draw(){
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        ofSetColor(0,0,0);
        ofRect(0,0,WIDTH,HEIGHT);
        
        ofTranslate(WIDTH/2, HEIGHT/2);
        
        drawAmoeba();
    }
    
    void drawAmoeba(){
        cam.setPosition(ofPoint(0,0,-2));
        light.setPosition(0,0,2);
        ofTranslate(0,0,0);

        ofScale(HEIGHT/4, HEIGHT/4, HEIGHT/4);
        
        cam.setOrientation((ofVec3f((ofGetMouseY()-HEIGHT/2)/3.0, -(ofGetMouseX()-WIDTH/2)/3.0, 0)));
        
        ofSetColor(ofColor(255,255,255,255));
        mesh.setMode(OF_PRIMITIVE_TRIANGLES);
        
//        mesh.drawWireframe();
//        ofEnableDepthTest();
//            glEnable(GL_DEPTH_TEST);
//        ofEnableLighting();
//        glDisable(GL_COLOR_MATERIAL);
//        material.begin();
//        light.enable();
//        cam.begin();
        
//        glEnable(GL_CULL_FACE); // Cull back facing polygons
        glCullFace(GL_FRONT_AND_BACK);
//        glCullFace(GL_FRONT);
        
        ofRotateY(ofGetFrameNum() + app->parameterMap[user3]*35);
        ofRotateX(ofGetFrameNum()/2);
        
        mesh.draw();
//        mesh.drawWireframe();
        
        glDisable(GL_CULL_FACE);
        
//         ofSphere(0,0,0, 1);
//        cam.end();

//        light.disable();
//        material.end();
//        ofDisableLighting();
//
//        light.draw();
        
    }
    
    void drawInfiniMeshes(){

        ofScale(HEIGHT/4, HEIGHT/4, HEIGHT/4);
        
        cam.setPosition(ofPoint(0,0,0));
        cam.setOrientation((ofVec3f((1)/3.0, 0, 0)));
        cam.begin();
        
        ofSetColor(ofColor(255,255,255,255));
        mesh.setMode(OF_PRIMITIVE_LINES);
        
        int index = ofRandom(mesh.getNumVertices());
        mesh.setVertex(index,
                       mesh.getVertex(index)+0.1*ofVec3f(ofRandom(1), ofRandom(1), ofRandom(1)));
        
        int N = 6;
        int S = 4;
        ofTranslate(0,0,-N/2*S);
        for(float i=-N/2+0.5;i<N/2;i++)
            for(float j=-N/2+0.5;j<N/2;j++){
                ofPushMatrix();
                ofTranslate(i*S,0,j*S + zoom - S*int(zoom/(S)));
                mesh.drawWireframe();
                ofPopMatrix();
            }
        
        cam.end();
    }
    
    void loadMesh(){
        _max = 0;

        string t = ofBufferFromFile("/Users/Raph/Desktop/3D Models/cocoTree/coconut.obj").getText();
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
                        f.setNormal(0, ofVec3f(ofToFloat(v1[2]), ofToFloat(v2[2]),ofToFloat(v2[2])));
                        mesh.addVertex(f.getVertex(0));
                        mesh.addTexCoord(f.getTexCoord(0));
                        mesh.addNormal(f.getNormal(0));
                        
                        _max = max(f.getVertex(0)[0], max(f.getVertex(0)[1], max(f.getVertex(0)[2], _max)));
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
    
    void mask(){}
    void capture(ofFbo* fbo){}
    void update(){
        zoom += 0.5;
        
        mesh = ofMesh::sphere(1, 100, OF_PRIMITIVE_TRIANGLES);
        
        ofVec3f o(-0.3, -0.3, 0.2);
        for(int i=0;i<mesh.getVertices().size();i++){
            float v = (mesh.getVertex(i) - o).length();
            mesh.addColor(ofColor(155*v, 155*v, 155*v));
        }
        
        float a1 = app->parameterMap[user1];
        float a2 = app->parameterMap[user2];
        float a3 = app->parameterMap[user3];
        
        
        float off = ofGetFrameNum()/5.0;
        if(a1 || 1==1){
            for(int i=0;i<mesh.getVertices().size();i++){
                ofVec3f v = mesh.getVertex(i);
                v.y = ofMap(a1, 0, 1, v.y, v.y/abs(v.y) * abs(sin(off+v.x*3.14*a2)), true);
//                 v.z = ofMap(a1,0, 1, v.z, v.z/abs(v.z) * abs(sin(v.x*3.14*a2)), true);
//                v.z = ofLerp(v.z, sin(v.z), a1);
                v.x *= (1+a2)/4.0;
                mesh.setVertex(i, v);
                float vv = abs((v - o).length());
                mesh.setColor(i, ofColor(255*vv, 255*vv, 255*vv));
            }
        }
        
//        if(a2){
//            for(int i=0;i<mesh.getVertices().size();i++){
//                ofVec3f v = mesh.getVertex(i);
//                v.x = ofLerp(v.x, sqrt(tan(v.x)), a2);
//                v.z = ofLerp(v.z, sqrt(tan(v.z)), a2);
//                mesh.setVertex(i, v);
//            }
//        }
        
    }
    
    void setImage(ofImage*){}
    ofImage* getImage(){return 0;}
    
    void mousePressed(int x, int y, int button){}
  void mouseDragged(int x, int y, int button){
       // zoom += (y-HEIGHT/2) / 10.0;
    }
    void mouseMoved(int x, int y){}

    void touchMoved(ofTouchEventArgs &touch){
        
    }
    
     void keyPressed(int key){
         switch(key){
     case 357:
         zoom += 10;
         break;
     case 359:
         zoom -= 10;
         break;
         }
     }
    
     void midiEvent(ofxMidiMessage& eventArgs){}
     void oscEvent(std::string header, std::string arg){}
     void oscEvent(std::string header, std::vector<float> args){}
    
     void saveMacro(stringstream *xml){}
     void loadMacro(ofXml *xml){}
    void exit(){ }

    std::string getInfo(){
        return "";
    }
    
private:
    vector<ofVec3f> points;
    vector<ofVec3f> lines;
    ofMesh sphere;
    ofEasyCam cam;
    float zoom = 0;
    
    ofMesh mesh;
    
    ofLight light;
    ofMaterial material;
    
    float _max;
};

#endif
