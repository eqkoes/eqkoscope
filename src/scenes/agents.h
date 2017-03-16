#ifndef agents_h
#define agents_h

#include "Scene.h"
#include "agent.h"

class Agents: public Scene{
    
public:

    Agents(AbstractApp* app)
        :Scene(app, "agents"){
            // add agent generation shape + shape randomness in float (does interpolations)
            
            sceneID = "agents";
            
            for(int i=0;i<app->parameterMap[agentNb];i++){
                initAgent();
            }
        }
    
    void initAgent(){
        Agent a(app, app->parameterMap[agentNbShapes]);
        a.scale = (1/20.0);
        a.pos.z = ofRandom(0,AGENT_MAX_Z);
        agents.push_back(a);
    }

     void setup(){}
     void exit(){}
     void draw(){
         ofBackground(0);
         ofTranslate(WIDTH/2,HEIGHT/2);
         ofScale(HEIGHT/2,HEIGHT/2, HEIGHT/2);
         ofRotate(360*dRot*ofGetElapsedTimeMillis()/1000.0);
         ofRotateX(app->parameterMap[mediaRotX]);
         ofRotateY(app->parameterMap[mediaRotY]);
         ofRotateZ(app->parameterMap[mediaRotZ]);
         for(int i=0;i<agents.size();i++){
             agents[i].draw();
         }
     }
    
     void mask(){}
     void capture(ofFbo* fbo){}
    
     void update(){
         app->deltaMap[evolution] = fmod(app->deltaMap[evolution] + 0.01, 1) ;
         
         if(agents.size() != app->parameterMap[agentNb]){
             while(app->parameterMap[agentNb]>agents.size())
                 initAgent();
             while(app->parameterMap[agentNb]<agents.size())
                 agents.erase(agents.end()-1);
         }
         
         for(int i=0;i<agents.size();i++){
             agents[i].pos.z += app->parameterMap[agentSpeed] ;
             if(agents[i].pos.z>2){
                 agents[i].init(app->parameterMap[agentNbShapes]);
                 agents[i].scale = (1/20.0);
             }
         }
         
         for(int i=0;i<agents.size();i++){
             agents[i].update();
         }
     }
    
     void mouseDragged(int x, int y, int button){}
     void touchMoved(ofTouchEventArgs &touch){}
     void keyPressed(int key){}
    
     void midiEvent(ofxMidiMessage& eventArgs){}
     void oscEvent(std::string header, std::string arg){}
     void oscEvent(std::string header, std::vector<float> args){}
    
     void saveMacro(stringstream *xml){
         (*xml) << "<agents>" << endl;
         
         (*xml) << "</agents>" << endl;
     }
     void loadMacro(ofXml *xml){}
    
     std::string getInfo(){return "";}
    
    bool isBackground(){return false;}

    private:
        vector<Agent> agents;
    float dRot = 0.1;

    
};

#endif /* agents_h */
