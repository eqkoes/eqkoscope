#ifndef eqkoscope_LedStrip_h
#define eqkoscope_LedStrip_h

#include <ofBaseTypes.h>
#include <string>
#include "scene.h"
#include "utils.h"
#include "ofSerial.h"
#include "strip.h"


class LedStrip: public Scene{
    
public:
    
    LedStrip(AbstractApp* app): Scene(app, "ledstrip"){
    }
    
    void initParameters(){
    }
    void setup(){
        serial.listDevices();
        serial.setup(0, 115200);
        sleep(1);

        for(int i=0;i<NB_STRIPS;i++)
            strips.push_back(new Strip(1, &serial));
    };
    
    void exit(){};
    void draw(){
        unsigned char mode = app->parameterMap[ledMode];
        unsigned char sync = app->parameterMap[ledSync];
        unsigned char bright = min(0.95f, (float)app->parameterMap[ledBrightness]*app->parameterMap[tintBrightness])*254;
        unsigned char info = app->parameterMap[ledInfo];
        unsigned char period = app->parameterMap[ledPeriod];
        if(period ==10) period = 11; //why ???
        unsigned char tint = app->parameterMap[ledTint];
        
        if(app->parameterMap[skewAmp] > BOOL_PARAM_EPSILON){
            mode = 0;
            sync = 1;
        }
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
        
        if(app->parameterMap[_reTint]>0.8){
            if(app->parameterMap[tintHue]<0.1 || app->parameterMap[tintHue]>0.95)
                tint = 0;
                if(app->parameterMap[tintHue]>0.55 || app->parameterMap[tintHue]<0.65)
                    tint = 1;
        }
        
        buf[0] =  mode;
        buf[1] = sync;
        buf[2] = bright;
        buf[3] = ledInfo;
        buf[4] = period;
        //        if(app->parameterMap[ledEvent]>= 0.001)
        //            cout << "strip evnt" << endl;
        bool event = app->parameterMap[ledEvent] >= 0.001;// || ofGetFrameNum()%4==0;
        if(event)
            eventId ++;
        buf[5] = eventId;
        buf[6] = tint;
        
        buf[10] = (unsigned char) (min(1.0f,app->parameterMap[user1])*127);
        buf[10+1] = (unsigned char)(min(1.0f,app->parameterMap[user2])*127);
        buf[10+2] = (unsigned char) (min(1.0f,app->parameterMap[user3])*127);
        buf[10+3] = (unsigned char)(min(1.0f,app->parameterMap[user4])*127);
        buf[10+4] = (unsigned char)(min(1.0f,app->parameterMap[user5])*127);
        
        buf[15] = '\n';
        
        //sanity check
      for(int i=0;i<15;i++)
          if(buf[i]=='\n')
              buf[i] = 11;
        //            buf[i] += 48;
        
        cout << "send " << (int)buf[0] << " " << (int)buf[1] << " "
        << (int)buf[2]  << " " << (int)buf[3]<< " " << (int)buf[5] << endl
        << (int)buf[10] << "|"  << (int)buf[11] << "|"  << (int)buf[12] << "|"  << (int)buf[13] << "|" << endl;
        
        serial.writeBytes(buf, 16);
        serial.flush();
        
        app->parameterMap[ledEvent] = 0;
        app->deltaMap[ledEvent] = 0;
}
    
    void mask(){};
    void capture(ofFbo* fbo){};
    
    void update(){
        if(ofGetFrameNum()%100==0)
            serial.setup(0, 115200);
        
//        for(Strip* strip: strips)
//            strip->update();
    
    };
    
   /* void replicate(){
        if(ofGetFrameNum()<5)
            return;
        
//        app->i.resize(NUM_LEDS, NB_STRIPS);
        for(int s=0;s<NB_STRIPS;s++){
//            serial.writeByte(s);
            buf[0] = s;
            char cc = 127*ofGetMouseX()/1280.0;
            for(int c=0;c<NUM_LEDS;c++){
//                ofColor col = app->i.getColor(c,s);
//                buf[3*c+1] = col.r;
//                buf[3*c+2] = col.g;
//                buf[3*c+3] = col.b;
                buf[3*c+1-1] = cc;
                buf[3*c+2-1] = cc;
                buf[3*c+3-1] = cc;
//                serial.writeByte(buf[3*c+1]);
//                serial.writeByte(buf[3*c+2]);
//                serial.writeByte(127*ofGetMouseX()/1280.0);
                serial.writeByte(( char)127*ofGetMouseX()/1280.0);
                serial.writeByte(( char)127*ofGetMouseX()/1280.0);
//                serial.writeByte(127*ofGetMouseX()/1280.0);
//              serial.writeByte((unsigned char) (127*ofGetMouseX()/1280));
                
                serial.writeByte(( char)0);
//                for(int ccc=0;ccc<3;ccc++)
//                serial.writeByte(0);
            }
            serial.writeByte('\n');
            buf[3*NUM_LEDS+2 - 2] = '\n';
//            serial.writeBytes(buf, 3*NUM_LEDS+2);
            serial.flush();
        }
        cout << "color " << abs(127*ofGetMouseX()/1280.0) << endl;
    }*/
    
    void rain(){
        
    }
    
    void mouseDragged(int x, int y, int button){};
    void touchMoved(ofTouchEventArgs &touch){};
    void keyPressed(int key){};
    
    void midiEvent(ofxMidiMessage& eventArgs){};
    void oscEvent(std::string header, std::string arg){};
    void oscEvent(std::string header, std::vector<float> args){};
    
    void saveMacro(stringstream *xml){};
    void loadMacro(ofXml *xml){};
    
    std::string getInfo(){};
    
    bool isBackground(){return back;}
    
    int mode = 0;
    
private:
    ofSerial serial;
    int NB_STRIPS = 1;
    std::vector<Strip*> strips;
    unsigned char buf[25];
    
    unsigned char eventId = 0;
};


#endif
