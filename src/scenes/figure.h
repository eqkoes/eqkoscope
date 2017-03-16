#ifndef figure_h
#define figure_h

#include "ofMesh.h"
#include "AbstractApp.h"

//TODO shapes inside of shapes with transfos or not
//colors
//shape morph tri->square
//evolution parameter (determines everything !)


class Figure{

public:
    
    Figure(AbstractApp* app, float n, float index){
        this->app = app;
    this->index = index;
    this->nbSides = n;
    for(int i=0;i<n;i++)
        mesh.addVertex(ofVec3f(sin(2*PI*i/n), cos(2*PI*i/n), 0));
    pos = ofVec3f(sin(2*PI*index), cos(2*PI*index), ofRandom(-0.5,0.5));

}

void draw(){
//    if(app->parameterMap[agentFill]<1)
        mesh.setMode(OF_PRIMITIVE_LINE_LOOP);
//    else
//        mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    ofPushMatrix();
    ofTranslate(pos);
    ofRotate(360*dRot*ofGetElapsedTimeMillis()/1000.0);
    float s = scale * cos(2*PI*dScale*ofGetElapsedTimeMillis()/1000.0);
    ofScale(s,s);
    mesh.draw();
    
    if(app->parameterMap[agentFill]==1){
        mesh.setMode(OF_PRIMITIVE_TRIANGLES);
//    ofSetColor(app->parameterMap[agentFill]*255,app->parameterMap[agentFill]*255,app->parameterMap[agentFill]*255);
    mesh.draw();
//    ofSetColor(255);
    }

    ofPopMatrix();
}
    
    void update(){
        mesh.clear(); //TODO opt
        float n = nbSides + app->parameterMap[evolution]*5;
        for(int i=0;i<n;i++)
            mesh.addVertex(ofVec3f(sin(2*PI*i/n), cos(2*PI*i/n), 0));
        
        float a = index + app->parameterMap[evolution];
        pos = ofVec3f(sin(2*PI*a), cos(2*PI*a), pos.z);

    }


    ofMesh mesh;
    ofVec3f pos;
    float scale=1;
    float index = 0;
    int nbSides = 3;
    float dScale = 0.; //per second
    float dRot = 1;
    AbstractApp* app;

};

#endif /* figure_h */
