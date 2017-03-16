#ifndef eqkoscope_ofApp_OSC_h
#define eqkoscope_ofApp_OSC_h

#include "eqkoscope.h"

void eqkoscope::initOSC(){
    receiver.setup(5555);
//    sender.setup("127.0.0.1", 6666);
}

bool eqkoscope::manageOSCParam(ofxOscMessage* m){
    bool ret = false;
    vector<string> elts = ofSplitString(m->getAddress(), ",");
    for(int i=0;i<elts.size();i++){
    if(parameterNameMap.count(elts[i])){
        int id = parameterNameMap[elts[i]];
        deltaMap[id] = parameterMap[id] = m->getArgAsFloat(i);
        ret = true;
    }
    }
    return ret;
}

bool recordTablet = true;
void eqkoscope::manageOSC(){
    while(receiver.hasWaitingMessages()){
        ofxOscMessage m;
        receiver.getNextMessage(&m);
        
        if(strStartsWith(m.getAddress(), "/Ping")){
            sender.setup(m.getRemoteIp(), 6666);
            oscout("/Ping", 1);
            oscout("/Macros/Names", macroNames, 8*16);
            continue;
        }
        
        if(!m.getAddress().compare("/Macro") && m.getNumArgs()==2){
            loadMacro(getMacroFromMIDI(m.getArgAsFloat(0)*16 + m.getArgAsFloat(1)));
            continue;
        }
        
        if(m.getNumArgs()==1 && strStartsWith(m.getAddress(), "/toggle")){//hack
            recordTablet = m.getArgAsFloat(0);
            continue;
        }
        
        if(m.getNumArgs()==2 && strStartsWith(m.getAddress(), "/Draw")){
            if(recordTablet)
                drawscene->recordPoint(m.getArgAsFloat(0)*WIDTH, m.getArgAsFloat(1)*HEIGHT, 0, ofColor::white);
                drawscene->moveCursor(m.getArgAsFloat(0)*WIDTH, m.getArgAsFloat(1)*HEIGHT);
            continue;
        }
        
        if(strStartsWith(m.getAddress(), "/Draw/erase")){
            drawscene->destruction(0);
            continue;
        }
        
//        if(!m.getAddress().compare("/Gpad/L")){
//            cout << m.getArgAsFloat(0) << endl;
//        }
        
        bool found = false;
        for(Auto* a: oscAutos)
            if(!a->oscPrefix.compare(m.getAddress())){
                a->update(m.getArgAsFloat(0));
                bool found = true;
            }
        if(found)
            continue;
        
        /** KINECT **/
        if(parameterMap[kinect] && m.getAddress()=="/Kinect"){
            kinectMesh.clear();
            
            int N = m.getNumArgs()/3;
            //                for(int x=0;x<640;x++)
            //                    for(int y=0;y<480;y++)
            //                        kinectGrid[x][y] = -10000;
            for(int i=0;i<N;i++){
                float x = 640-m.getArgAsFloat(i*3);
                float y = m.getArgAsFloat(i*3+1);
                float z = m.getArgAsFloat(i*3+2);
                ofVec3f v(x, y, z);
                kinectMesh.addVertex(v);
                int zMult = 400;
                //                    kinectMesh.addColor(ofColor::fromHsb(0, 0, 50+z*zMult, 50+z*zMult));
                //                    kinectMesh.addColor(ofColor::fromHsb(0, 0, 100+z*zMult));
                kinectMesh.addColor(ofColor::fromHsb(0, 0, ofRandom(255)));
            }
            
            continue;
        }
        
        long s = ofGetElapsedTimeMicros();
        if(manageOSCParam(&m)){ //test
            cout << ofGetElapsedTimeMicros() - s << "(elapsed osc)" << endl;
            continue;
        }
        cout << ofGetElapsedTimeMicros() - s << "(elapsed osc)" << endl;
      }
}


void eqkoscope::oscout(std::string head, float value){
    ofxOscMessage msg;
    msg.setAddress(head);
    msg.addFloatArg(value);
    sender.sendMessage(msg);
}

void eqkoscope::oscout(std::string head, vector<float> values){
    ofxOscMessage msg;
    msg.setAddress(head);
    for(float value : values)
    msg.addFloatArg(value);
    sender.sendMessage(msg);
}

void eqkoscope::oscout(std::string head, string values[], int length){
    ofxOscMessage msg;
    msg.setAddress(head);
    for(int i=0;i<length;i++)
        msg.addStringArg(values[i]);
    sender.sendMessage(msg);
}

#endif
