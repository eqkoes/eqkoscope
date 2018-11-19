#ifndef eqkoscope_LedStrip_h
#define eqkoscope_LedStrip_h

#include <ofBaseTypes.h>
#include <string>
#include "scene.h"
#include "utils.h"
#include "ofSerial.h"


class LedStrip: public Scene{
    
public:
    
    LedStrip(AbstractApp* app): Scene(app, "ledstrip"){
    }
    
    void initParameters(){
    }
    
    void setup(){
        vector <ofSerialDeviceInfo> info = serial.getDeviceList();
#ifdef VERBOSE
        serial.listDevices();
#endif
        serial.setup(0, 115200);
        ofSleepMillis(1);
    };
    
    void exit(){};

    void draw(){

        if(!serial.isInitialized())
            return;
        
        
        unsigned char mode = app->parameterMap[ledMode];
        unsigned char sync = app->parameterMap[ledSync];
        unsigned char bright = max(0.0f, min(0.95f, (float)app->parameterMap[ledBrightness]))*255;
        unsigned char info = app->parameterMap[ledInfo];
        unsigned char period = app->parameterMap[ledPeriod];
        if(period ==10) period = 11; //why ???
        unsigned char tint = app->parameterMap[ledTint]*255;
        unsigned char saturation =  app->parameterMap[ledSaturation]*255;
        unsigned char strobeMode = app->parameterMap[ledStrobe];
        
        if(app->parameterMap[strobe]>BOOL_PARAM_EPSILON){
            mode = 4;
            sync = 1;
            period = 25;
            if(app->parameterMap[randHHide]>BOOL_PARAM_EPSILON){
                sync = 0;
            }
        }else{
            if(app->parameterMap[flash]>BOOL_PARAM_EPSILON){
                mode = 1;
                sync = 0;
            }
            if(app->parameterMap[randHHide]>BOOL_PARAM_EPSILON){
                mode = 1;
                sync = 0;
            }
        }
        
        if(app->parameterMap[ledTint]==-2 && app->parameterMap[_reTint]>0.5 ){
            tint = app->parameterMap[tintHue]*255/2.0;
            saturation = app->parameterMap[_reTint]*255;
        }
        
        bright *= app->maxLedBrightness;
        
#ifdef NO_STROBE
        if(mode==4)
            mode=3;
#endif
        
        buf[0] = mode;
        buf[1] = sync;
        buf[2] = bright;
        buf[3] = app->deltaMap[ledInfo];
        buf[4] = period;
        bool event = app->deltaMap[ledEvent] >= 0.001;// || ofGetFrameNum()%4==0;
        if(event)
            ++eventId;
        buf[5] = eventId;
        //        buf[5] = event;
        buf[6] = tint;
        buf[7] = saturation;
        buf[8] = strobeMode;
        buf[9] = app->parameterMap[uvLight]*255;

        buf[24] = '\n';
        
        //sanity check
        for(int i=0;i<24;i++)
            if(buf[i]=='\n')
                buf[i] = 11;
        
        serial.writeBytes(buf, 25);
        serial.flush();
        
        if(app->parameterMap[ledAuto]==0){
            app->parameterMap[ledEvent] = app->deltaMap[ledEvent] = 0;
        }
    }
    
    void mask(){};
    void capture(ofFbo* fbo){};
    
    void update(){
        if(ofGetFrameNum()%100==0) //auto reload
            serial.setup(0, 115200);
    };
    
    void mousePressed(int x, int y, int button){}
    void mouseDragged(int x, int y, int button){}
    void mouseMoved(int x, int y){}
    void touchMoved(ofTouchEventArgs &touch){}
    void keyPressed(int key){}
    
    void midiEvent(ofxMidiMessage& eventArgs){}
    void oscEvent(std::string header, std::string arg){}
    void oscEvent(std::string header, std::vector<float> args){}
    
    void saveMacro(stringstream *xml){};
    void loadMacro(TiXmlHandle *xml){};
    
    std::string getInfo(){return "";};
    
    bool isBackground(){return back;}
    
    void closeSerial(){serial.close();}
    
private:
    ofSerial serial;
    unsigned char buf[25];
    
    unsigned char eventId = 0;
    unsigned char user[5];
    
    unsigned char mode,sync,tint,strobeMode,info,event,bright,period;
};


#endif
