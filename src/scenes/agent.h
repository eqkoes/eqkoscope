#ifndef agent_h
#define agent_h

#define AGENT_MAX_Z -25

//tip : all fractals should change as one
// TODO : agentRandomness : indicates random of position, angle etc (regarding the general h a r m o n y

#include "figure.h"
#include "AbstractApp.h"

class Agent{
    
public:
    Agent(AbstractApp* app,float nShapes){
        this->app = app;
        init(nShapes);
    }
    
    void init(int nShapes){
        figs.clear();
        int n = app->parameterMap[agentShapeNb]+ofRandom(-app->parameterMap[agentShapeNbRand], app->parameterMap[agentShapeNbRand]);
        for(int i=0;i<nShapes;i++){
            Figure f(app, n, i/nShapes);
            figs.push_back(f);
        }
        
//        if(ofRandom(1)<0.1){ //recursion
//            a = new Agent(nShapes*2);
//            a->scale = 0.5;
//            a->pos.z = 0;
//            a->pos *= 1.5;
//        }
        
        float a = 2*PI *app->parameterMap[agentAngleFreq] * ofGetElapsedTimeMillis()/1000.0;
        float d = 1;
        if(app->parameterMap[agentRandDist])
            d = ofRandom(0.25, 1);
        pos = ofVec3f(d*sin(a), d*cos(a), AGENT_MAX_Z);
    }
    
    void draw(){
        ofPushMatrix();
        ofTranslate(pos);
        ofScale(scale,scale);
        ofRotate(360*dRot*ofGetElapsedTimeMillis()/1000.0);
        ofSetColor(ofMap(pos.z, 0, AGENT_MAX_Z, 255, 0, true));

        for(int i=0;i<figs.size();i++)
            figs[i].draw();
        
        if(a!=0)
            a->draw();
        
        ofPopMatrix();
        
        
    }
    
    void update(){
        for(int i=0;i<figs.size();i++)
            figs[i].update();
        
        if(a!=0)
            a->update();
    }
    
    vector<Figure> figs;
    ofVec3f pos;
    float scale = 1;
    float dRot = 0.05;
    float dRotBirth = 1;
    Agent *a = 0;
    AbstractApp *app;
    
    
};


#endif /* agent_h */
