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
 dur
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
        this->parameterID = pid;
    }
    
    void update(float value){
        
        if(saw && value > 0){
            app->parameterEasingMap[parameterID] = 1;
        }else{
         app->parameterEasingMap[parameterID] = smoothing; //DAT HACK !!
        }
            
        if(parameterID == ledEvent ){
            app->deltaMap[ledEvent]++;
            app->parameterMap[ledEvent]++;
            return;
        }
        
        float nValues = values.size();
        
        if(bin)
            value = (value>=0.0001) ? 1 : 0;
        float v;
        bool timedRandom = false;
        switch(type){
            case TIMED: case BPM:{
                float date = app->localFrameNum*1000/app->currentFrameRate;

                if(ulastDate!=-1){
                    float tau = (date - ulastDate)*sens;
#ifdef REAL_TIME_FX
                    tau /= app->maxFrameRate/ofGetFrameRate();
#endif
                    uelapsed += tau;
                    
                }
                if(uelapsed<0){
                    sens = 1;
                    uelapsed = 0;
                }
                
                ulastDate = (int) date;

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
                    v =  ! app->deltaMap[parameterID];
                }else{
                    if(random==0){
                        v = getValue(value);
                    }else{
                        if(value>0){
                            do{
                                v = ofRandom(1);
                            }while(abs(lastRandom-v) <= 1/5.0);
                        //variance de 1/5
                            lastRandom = v;
                            v = getValue(v);
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
                if(bin)
                    v = v>0 ? 1 : 0;
                if(is_int)
                    v = ceil(v);
                app->deltaMap[parameterID] = app->parameterMap[parameterID] = v;
            }
        }else{
            if(bin)
                v = v>0 ? 1 : 0;
            if(is_int)
                v = ceil(v);
            
            if(transcient){
                app->parameterMap[parameterID] = v;
            }else{
                app->deltaMap[parameterID] = v;
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

        if(app->parameterIDMap[parameterID].compare("")){
        
        switch(type){
            case AUDIO:
                str = "AUDIO," + getCoreStr() + "," + ofToString(channel);
                break;
            case OSC:
                str = ("OSC,") + getCoreStr() + "," + oscPrefix;
                break;
            case MIDION:{
                str = ("NOTEON,") + getCoreStr() + "," + ofToString(channel) + ",";
                if(minId==maxId)
                    str += ofToString(minId);
                else
                    str += ofToString(minId) + "/" + ofToString(maxId);

                    }break;
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
        str += app->parameterIDMap[parameterID] + ",";
        for(int i=0;i<values.size();i++)
            str += ofToString(values[i]) + ((i<values.size()-1) ? "/" : "");
        if(type!=BPM && type!=TIMED)
        str += "," + ofToString(smoothing);
        return str;
    }
    
    
    AbstractApp* app;
    int parameterID = 0;
    AutoType type;
    vector<float> values;
    float smoothing = 1;
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
    float lastRandom = 0;
    float targetValue = 0;
    
    /*** OPTIONS **/
    bool bin = false;
    bool is_int = false;
    bool transcient = false;
    bool saw = false;
    
    /** MISC/UTILS */
    bool midiLearnt = false;
    
private:
    int ulastDate = -1;
};
#endif
