#ifndef eqkoscope_lines_h
#define eqkoscope_lines_h

#include "scene.h"

class Lines: public Scene{
    
public:
    
    Lines(AbstractApp* app)
    :Scene(app, "lines"){
        sceneID = "lines";

        
        for(int i=0;i<50;i++){
            attractors.push_back(ofVec4f(ofRandom(WIDTH),
                                         ofRandom(HEIGHT+app->parameterMap[updateLen]*2)-app->parameterMap[updateLen],
                                         ofRandom(-1,1), ofRandom(0,1)));
        }
            }
    void setup(){
        
    }
    void exit(){
        
    }
    
    void draw(){
        ofTranslate(WIDTH/2,HEIGHT/2-(HEIGHT2-HEIGHT)/2);
        
        ofSetColor(ofColor::black);
        ofRect(-WIDTH,-HEIGHT,WIDTH*2,HEIGHT*2);
        ofSetColor(ofColor::white);
        
        ofTranslate(app->parameterMap[mediaX]*WIDTH, -app->parameterMap[mediaY]*HEIGHT, app->parameterMap[mediaZ]*HEIGHT);
        ofRotateX(app->parameterMap[mediaRotX]);
        ofRotateY(app->parameterMap[mediaRotY]);
        ofRotateZ(app->parameterMap[mediaRotZ]);
        

        float angle = 0;
        int mm = 1+app->parameterMap[lines_ySquare];
        mm = mm==0 ? 1 : mm;
        int yres = int((1+app->parameterMap[lines_yres]*HEIGHT2/10)/mm)*mm;
        int xres = max(1,int(20/mm)*mm);
        
        int hh = HEIGHT*(1-app->parameterMap[user1]);
        for(int y=yres/2-hh+HEIGHT/2;y<hh+HEIGHT/2;y+=yres){
            ofPolyline b;
            ofMesh mesh;
            mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
            for(int x=-xres*2;x<WIDTH+xres*2;x+=xres){
                float ax=0,ay=0, az=0;
                float dMax = 30000;
                for(int a=0;a<attractors.size();a++){
                    ofVec4f at = attractors[a];
                    float d = ofDistSquared(x, y, at.x, at.y);
                    if(d<dMax){
                        ay += at.w*(y-at.y) * ofMap(d, 1, dMax, 1, 0, true)*app->parameterMap[lines_yWeight];
                        az += at.w*(at.z) * ofMap(d, 1, dMax, 1, 0, true)*app->parameterMap[lines_zWeight];
                    }
                }
                mm = mm==0 ? 1 : mm;
                float xx = int((x) / mm )* mm;
                float yy = y + ay;
                mm = 1+app->parameterMap[lines_zSquare]*HEIGHT2;
                float zz = -int((az*HEIGHT2) / mm) * mm;
                if(app->parameterMap[lines_mesh])
                mesh.addVertex(ofVec3f(xx-WIDTH/2,yy-HEIGHT/2,zz));
                else
                                    b.lineTo(xx-WIDTH/2, yy-HEIGHT/2, zz);
            }
            
            int count = 1 + (y%7)*app->parameterMap[shapeWeight] / 7.0;
            
            ofSetColor(ofColor::white);
            for(int i=0;i<count;i++){
                ofPushMatrix();
                ofTranslate(0,i);
                if(app->parameterMap[lines_mesh])
                mesh.draw();
                else
                    b.draw();
                ofPopMatrix();
            }
        }
    }
    
    void mask(){
        
    }
    void capture(ofFbo* fbo){
        
    }
    void update(){
        float n = ofGetFrameNum()/10000000.0;
        for(int a=0;a<attractors.size();a++){
            if(attractors[a].y>HEIGHT+app->parameterMap[updateLen])
                initAttractor(a, ofRandom(WIDTH), -app->parameterMap[updateLen]);
            if(attractors[a].y<-app->parameterMap[updateLen]-1)
                initAttractor(a, ofRandom(WIDTH), HEIGHT+app->parameterMap[updateLen]);
            attractors[a].w = ofMap(1, -1, 1, -1, 1, true);
            attractors[a].y += app->parameterMap[speed];
        }
    }
    
    void initAttractor(int index, int x, int y){
        attractors[index] = ofVec4f(x, y, ofRandom(-1,1), ofRandom(1));
    }
    
    void mousePressed(int x, int y, int button){}
        void mouseDragged(int x, int y, int button){
        
    }
    void mouseMoved(int x, int y){}

    void touchMoved(ofTouchEventArgs &touch){
        
    }
    
    void keyPressed(int key){
//        app->parameterMap[lines_yWeight] = ofRandom(0,1);
//        app->parameterMap[lines_zWeight] = ofRandom(0,1);
//        app->parameterMap[lines_zSquare] = ofRandom(0,1);
//        app->parameterMap[lines_ySquare] = ofRandom(0,1);
//        app->parameterMap[lines_yres] = ofRandom(0,1);
    }
    
    void midiEvent(ofxMidiMessage& eventArgs){
        if(eventArgs.channel==1 || eventArgs.channel==3)
            return;
        
        float value = eventArgs.value;
        switch(eventArgs.status){
            case MIDI_CONTROL_CHANGE:{
                switch(eventArgs.control){
                    case 2:
                        app->deltaMap[lines_yWeight] = (value)/127.0;
                        break;
                    case 4:
                        app->deltaMap[lines_zWeight] = (value)/127.0;
                        break;
                    case 6:
                        app->deltaMap[speed] = (value-64)/127.0*100;
                        break;
                    case 75 :
                        app->deltaMap[speed] = 0;
                        break;
//                    case 6:
//                        app->deltaMap[lines_ySquare] = (value)/127.0;
//                        break;
                    case 7:
                        app->deltaMap[lines_zSquare] = (value)/127.0;
                        break;
                    case 8:
                        app->deltaMap[lines_yres] = (15+value)/127.0;
                        break;
                    case 9:
                        app->deltaMap[shapeWeight] = value/127.0*20;
                        break;
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
        (*xml) << "<lines>" << endl;
        (*xml) << "</lines>" << endl;
    }
    
    void loadMacro(ofXml *xml){
    }
    
    std::string getInfo(){
        return "";
    }
    
    bool isBackground(){return back;}
    
    void setResolution(int r){
        //reset the attractors
        attractors.clear();
        for(int i=0;i<50;i++){
            attractors.push_back(ofVec4f(ofRandom(WIDTH),
                                         ofRandom(HEIGHT+app->parameterMap[updateLen]*2)-app->parameterMap[updateLen],
                                         ofRandom(-1,1), ofRandom(0,1)));
        }
    }
    
    protected :
    vector<ofVec4f> attractors;
    
};


#endif
