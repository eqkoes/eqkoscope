#ifndef eqkoscope_ofApp_OSC_h
#define eqkoscope_ofApp_OSC_h

#include "eqkoscope.h"
#include <sys/types.h>
#ifndef WIN32
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

void eqkoscope::initOSC(){
    receiver.setup(OSC_INPUT);
    string myIP = "";

#ifndef WIN32
    /// Get my IP address
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;
    getifaddrs(&ifAddrStruct);
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr)
            continue;
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            //            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
            string ip = addressBuffer;
            if(strStartsWith(ip, "192.")){
                myIP = ip;
                break;
            }
        }
        //        else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
        //            // is a valid IP6 Address
        //            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
        //            char addressBuffer[INET6_ADDRSTRLEN];
        //            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
        //            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
        //        }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
#endif

    if(myIP.compare("")){
        string mask = myIP;
        int i = mask.find_last_not_of(".");
        mask.replace(i, mask.length()-i, "255");
        if(i>0){
            sender.setup(mask, 6666); //broadcast
            oscout("/Ping", myIP);
        }
    }else{ //if no remote interface, use local network
        myIP = "127.0.0.1";
        sender.setup(myIP, 6666); //broadcast
        oscout("/Ping", myIP);
    }
}

bool eqkoscope::manageOSCParam(ofxOscMessage* m){
    bool ret = false;
    vector<string> elts = ofSplitString(m->getAddress(), ",");
    for(int i=0;i<elts.size();i++){
        int pid = getParameterFromName(elts[i]);
        if(pid>=0){
            deltaMap[pid] = parameterMap[pid] = m->getArgAsFloat(i);
            ret = true;
        }
    }
    return ret;
}

bool recordTablet = true;
void eqkoscope::manageOSC(){
    if( receiver.getPort() != deltaMap[oscIn]){
        receiver.setup(deltaMap[oscIn]);
    }
    
    int count = 0;
    int maxCount = 100;
    while(receiver.hasWaitingMessages() && (++count<maxCount)){
        oscHasReceivedMessages = true;
        
        ofxOscMessage m;
        #ifdef OF_10
        receiver.getNextMessage(m); //update TEST
#else
        receiver.getNextMessage(&m); //update TEST
#endif
        
        if(strStartsWith(m.getAddress(), "/Ping")){
            #ifdef OF_10
            sender.setup(m.getRemoteHost(), 55);
#else
            sender.setup(m.getRemoteIp(), 55);
#endif
            oscout("/Ping", 1);
            oscout("/Macros/Names", macroNames, 8*16);
            continue;
        }
        
        if(!m.getAddress().compare("/MIDI/NoteOn") && m.getNumArgs()==3){
            ofxMidiMessage evt;
            evt.portNum = MIDI_MAX_NB_PORTS-1;
            evt.status = m.getArgAsInt32(1)>0 ? MIDI_NOTE_ON : MIDI_NOTE_OFF;
            evt.pitch = m.getArgAsInt32(0);
            evt.velocity = m.getArgAsInt32(1);
            evt.channel = m.getArgAsInt32(2);
            #ifdef VERBOSE
            cout << "NOTE ON " << ofGetFrameNum() << " " << evt.channel <<  "" << evt.pitch  <<  endl;
#endif
            newMidiMessage(evt);
            continue;
        }
        if(!m.getAddress().compare("/MIDI/CC") && m.getNumArgs()==3){
            ofxMidiMessage evt;
            evt.portNum = MIDI_MAX_NB_PORTS-1;
            evt.status = MIDI_CONTROL_CHANGE;
            evt.control = m.getArgAsInt32(0);
            evt.value = m.getArgAsInt32(1);
            evt.channel = m.getArgAsInt32(2);
            newMidiMessage(evt);
            continue;
        }
        if(!m.getAddress().compare("/MIDI/Audio") && m.getNumArgs()==2){
            //            cout << "audio msg count " << count << endl;
            audioOverOSC = true;
//            ofxMidiMessage evt;
            float gain = pow(10, (parameterMap[audioGain])/20.0);
            if(m.getArgAsInt32(1)<10){
                currentRms[int(m.getArgAsInt32(1))] = m.getArgAsFloat(0)*gain;
            }
            continue;
        }
        
        if(strStartsWith(m.getAddress(), "/p/") && m.getNumArgs()==1){
            int pid = getParameterFromName(m.getAddress().substr(3, m.getAddress().size()-3));
                deltaMap[pid] = m.getArgAsFloat(0);
            continue;
        }
        
#ifdef JOSE
        if(strStartsWith(m.getAddress(),"/30-BOULES_vite")){
            int off = 15;
            int on = 4;
            float x = m.getArgAsFloat(0+off*on);
            float y = m.getArgAsFloat(1+off*on);
            float z = m.getArgAsFloat(2+off*on);
            //deltaMap[upRot] = x;
            //            deltaMap[glow] = abs(y)*0.5;
            deltaMap[mediaX] = (x);
            deltaMap[mediaY] = (y);
            deltaMap[mediaZ] = 1 + (z);
            continue;
        }
        
        if(strStartsWith(m.getAddress(),"/30-BOULES")){
            int off = 15;
            int on = 3; //number of values per object
            float x = m.getArgAsFloat(0+off*on);
            float y = m.getArgAsFloat(1+off*on);
            float z = m.getArgAsFloat(2+off*on);
            //deltaMap[upRot] = x;
//            deltaMap[glow] = abs(y)*0.5;
//            deltaMap[mediaX] = (x)/10.0;
//            deltaMap[mediaY] = (y)/10.0;
//            deltaMap[mediaZ] = 1 + (z);
            continue;
        }
#endif

        
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

void eqkoscope::oscout(std::string head, std::string value){
    ofxOscMessage msg;
    msg.setAddress(head);
    msg.addStringArg(value);
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
