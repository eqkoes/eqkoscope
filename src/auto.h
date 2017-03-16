//
//  auto.h
//  eqkoscope
//
//  Created by Raphaël on 06/11/2016.
//
// A ranger dans des structures de données

/**
 
 USE CASES:
 
 MIDI ON/OFF : sets the parameter on ON, resets on off (negative value)
 
 AUDIO: maps audio signal on effect (with smoothing)
 
 OSC : maps controller on effect (with smoothing)
 
 **/


#ifndef eqkoscope_auto_h
#define eqkoscope_auto_h

#include "AbstractApp.h"

using namespace std;

enum AutoType{AUDIO, OSC, MIDION, MIDICC, LEAP, TIMED, BPM};

class Auto{
    
public:
    Auto(AbstractApp* app, int pid){
        this->app = app;
        this->parameterName = pid;
    }
    
    void update(float value){
        if(parameterName==_invert)
            cout << "woot" << endl;
        
        float nValues = values.size();
        
        if(bin)
            value = value ? 1 : 0;
        float v;
        bool timedRandom = false;
        switch(type){
            case TIMED: case BPM:{
                if(ulastDate!=-1){
                    uelapsed += (ofGetElapsedTimeMillis() - ulastDate)*sens;
                }
                if(uelapsed<0)
                    sens = 1;
                
                ulastDate = (int) ofGetElapsedTimeMillis();
                if(BPMDivision==0){
                    v = getValue(ofMap(uelapsed, 0, uDuration, 0, 1, true));
                    if(uelapsed>uDuration || uelapsed <0)
                        timedRandom = true;
                    switch(loopMode){
                        case 1:
                            uelapsed = uelapsed % uDuration;
                            break;
                        case 2:
                            if(uelapsed>uDuration){
                                sens = -1;
                            }break;
                        default:;
                    }
                }else{
                    long uTau = 4*60000*BPMDivision/app->parameterMap[bpm];
                    v = getValue(ofMap(uelapsed, 0, uTau, 0, 1, true));
                    if(uelapsed>uTau || uelapsed <0)
                        timedRandom = true;
                    switch(loopMode){
                        case 1:
                            uelapsed %= uTau;
                            break;
                        case 2:
                            if(uelapsed>uTau)
                                sens = -1;
                            break;
                        default:;
                    }
                }
            }break;
            default:{
                if(invert==1){
                    v =  ! app->deltaMap[parameterName];
                }else{
                    if(random==0){
                        v = getValue(value);
                    }else{
                        if(value>0){
                            v = lastValue;
                            do{
                            v = getValue(ofRandom(1));
//                                cout << "while " << abs(lastValue-v) << " <= " <<abs(1/(5*(values[values.size()-1]-values[0]))) << endl;
                            }while(abs(lastValue-v)>=abs(1/(5*(values[values.size()-1]-values[0]))));
                        //variance de 1/5
                        }else{
                            return;
                        }
                    }
                }
            }
        }
        
        if((type==TIMED || type==BPM) && random>0){
            if(timedRandom){
                v = getValue(ofRandom(1));
                app->deltaMap[parameterName] = app->parameterMap[parameterName] = v;
            }
        }else{
            if(transcient){
                cout << " value " << v  << ofGetElapsedTimeMillis() << endl;
                app->parameterMap[parameterName] = v;
            }else{
                app->deltaMap[parameterName] = v;
            }
        }
        lastValue = v;
    }
    
    void addValue(float v){
        values.push_back(v);
    }
    
    float getValue(float v){
        if(values.size()==1)
            return values[0];
        float nValues = values.size()-1;
        int vIndex1 = v*nValues;
        int vIndex2 = MIN(vIndex1+1, values.size()-1);
        float v1 = vIndex1/(nValues);
        float v2 = vIndex2/(nValues);
        return ofMap(v, v1, v2, values[vIndex1], values[vIndex2], true);
    }
    
    std::string toString(){
        string str;

        if(app->parameterIDMap[parameterName].compare("")){
        
        switch(type){
            case AUDIO:
                str = "AUDIO," + getCoreStr() + "," + ofToString(channel);
                break;
            case OSC:
                str = ("OSC,") + getCoreStr() + "," + oscPrefix;
                break;
            case MIDION:
                str = ("NOTEON,") + getCoreStr() + "," + ofToString(channel) + "," + ofToString(minId);
                break;
            case MIDICC:
                str = ("CC,") + getCoreStr() + "," + ofToString(channel) + "," + ofToString(minId);;
                break;
            case LEAP:
                str = ("LEAP,") + getCoreStr() + "," + leapDimension;
                break;
            case TIMED:
                str = ("DUR,") + getCoreStr() + "," + ofToString(uDuration);
                break;
            case BPM:
                str = ("BPM,") + getCoreStr() + "," + ofToString(BPMDivision);
                break;
        }
            str += " " + args;
        }
        
        return str;
    }
    
    std::string getCoreStr(){
        string str;
        str += app->parameterIDMap[parameterName] + ",";
        for(int i=0;i<values.size();i++)
            str += ofToString(values[i]) + ((i<values.size()-1) ? "/" : "");
        if(type!=BPM && type!=TIMED)
        str += "," + ofToString(smoothing);
        return str;
    }
    
    
    AbstractApp* app;
    int parameterName = 0;
    AutoType type;
    vector<float> values;
    float smoothing = 0;
    string args;
    
    float inputMinValue = 0;
    float inputMaxValue = 1;
    
    
    string oscPrefix = "";
    string leapDimension = "x";
    int channel = 0;
    int minId = 0, maxId = 0;
    float random = 0;
    bool invert = false;
    
    //MIDI SPECIFIC
    bool isMIDICC = true;
    bool hold = false; //stops on midi off
    
    //TIME SPECIFIC
    float BPMDivision = false;
    int uelapsed = 0;
    int uDuration = 1000;
    int loopMode = 1; //1 : loop, 2 FWD/BCKWD
    int sens = 1;
    
    float lastValue = 0;
    float targetValue = 0;
    
    bool bin = false;
    bool transcient = false;
    
//    bool noNoteRamp = false;
    
private:
    int ulastDate = -1;
};
#endif
