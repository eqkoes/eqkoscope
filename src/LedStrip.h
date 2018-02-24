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
        vector <ofSerialDeviceInfo> info = serial.getDeviceList();
        serial.listDevices();
        serial.setup(0, 115200);
        sleep(1);

        for(int i=0;i<NB_STRIPS;i++)
            strips.push_back(new Strip(1, &serial));
    };
    
    void exit(){};
    void draw(){
        
      /*  if(0==1){
            cout << serial.readByte() << endl;
            cout << serial.readByte() << endl;
            cout << serial.readByte() << endl;
            cout << serial.readByte() << endl;
        }else{
        
        int bytesRequired = 8;
        unsigned char bytes[bytesRequired];
        int bytesRemaining = bytesRequired;
        // loop until we've read everything
        while ( bytesRemaining > 0 )
        {
            // check for data
            if ( serial.available() > 0 )
            {
                // try to read - note offset into the bytes[] array, this is so
                // that we don't overwrite the bytes we already have
                int bytesArrayOffset = bytesRequired - bytesRemaining;
                int result = serial.readBytes( &bytes[bytesArrayOffset],
                                              bytesRemaining );
                
                // check for error code
                if ( result == OF_SERIAL_ERROR )
                {
                    // something bad happened
                    ofLog( OF_LOG_ERROR, "unrecoverable error reading from serial" );
                    // bail out
                    break;
                }
                else if ( result == OF_SERIAL_NO_DATA )
                {
                    // nothing was read, try again
                }
                else
                {
                    bytesRemaining -= result;
                }
            }
        }
        int a = int((unsigned char)(bytes[0]) << 24 |
                    (unsigned char)(bytes[1]) << 24 |
                    (unsigned char)(bytes[0]) << 24 |
                    (unsigned char)(bytes[0]) << 24 |
                    (unsigned char)(bytes[1]) << 16 |
                    (unsigned char)(bytes[2]) << 8 |
                    (unsigned char)(bytes[3]));
        
        cout << a << endl;
        }
        
        if(1==1)
            return;
        */
        
      /*  unsigned char tmpmode = app->parameterMap[ledMode];
        unsigned char tmpsync = app->parameterMap[ledSync];
        unsigned char tmpdelay = app->parameterMap[ledPeriod];
        unsigned char tmpTint = app->parameterMap[ledTint]<0 ? -1 : app->parameterMap[ledTint]/2;
        
        if(app->parameterMap[strobe]>BOOL_PARAM_EPSILON){
            tmpmode = 4;
            tmpsync = 1;
            tmpdelay = 25;
            if(app->parameterMap[randHHide]>BOOL_PARAM_EPSILON){
                tmpsync = 0;
            }
        }else{
            if(app->parameterMap[flash]>BOOL_PARAM_EPSILON){
                tmpmode = 1;
                tmpsync = 0;
            }
            if(app->parameterMap[randHHide]>BOOL_PARAM_EPSILON){
                tmpmode = 1;
                tmpsync = 0;
            }
        }
        
        if(app->parameterMap[ledTint]==-2 && app->parameterMap[_reTint]>0.5 ){
            tmpTint = app->parameterMap[tintHue]*255/2.0;
        }
        
//        if(mode!= tmpmode){
            mode = tmpmode;
            buf[0] = 109;//(unsigned char)'m';
            buf[1] = mode;
            serial.writeBytes(buf, 3);
            serial.flush();
//        }
        if(sync!= tmpsync){
            sync = tmpsync;
            buf[0] = 120;// 'y';
            buf[1] = sync;
            serial.writeBytes(buf, 3);
            serial.flush();
        }
        float b = max(0.0f, min(1.0f, (float)app->parameterMap[ledBrightness]))*255;
        b *= app->maxLedBrightness;
        if(bright!= b){
            bright = b;
            buf[0] = 'b';
            buf[1] = bright;
            serial.writeBytes(buf, 3);
            serial.flush();
        }
        if(period!= tmpdelay){
            period = tmpdelay;
            buf[0] = 100;//'d';
            buf[1] = period;
            serial.writeBytes(buf, 3);
            serial.flush();
        }
//        if(info!= app->parameterMap[ledInfo]){
//            info = app->parameterMap[ledInfo];
//            buf[0] = 'd';
//            buf[1] = info;
//            serial    .writeBytes(buf, 2);
//            serial.flush();
//        }
        if(tint!= tmpTint){
            tint = tmpTint;
            buf[0] = 't';
            buf[1] = tint;
            serial.writeBytes(buf, 3);
            serial.flush();
        }
        if(strobeMode!= app->parameterMap[ledStrobe]){
            strobeMode = app->parameterMap[ledStrobe];
            buf[0] = 15;//'s';
            buf[1] = strobeMode;
            serial.writeBytes(buf, 3);
            serial.flush();
        }
        
        for(int i=0;i<5;i++){
            unsigned char tmpuser = app->parameterMap[user1+i];
            if(user[i]!= tmpuser){
                user[i] = tmpuser;
                buf[0] = 117;
                buf[1] = i;
                buf[2] = user[i];
                serial.writeBytes(buf, 3);
                serial.flush();
            }
        }
     */
        
       
     unsigned char mode = app->parameterMap[ledMode];
     unsigned char sync = app->parameterMap[ledSync];
     unsigned char bright = max(0.0f, min(0.95f, (float)app->parameterMap[ledBrightness]))*255;
     unsigned char info = app->parameterMap[ledInfo];
     unsigned char period = app->parameterMap[ledPeriod];
     if(period ==10) period = 11; //why ???
        unsigned char tint = app->parameterMap[ledTint];
        unsigned char saturation =  app->parameterMap[ledSaturation];
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
        
        buf[10] = (unsigned char) (ofClamp(app->parameterMap[user1]*127,0,127));
        buf[10+1] = (unsigned char)(ofClamp(app->parameterMap[user2]*127,0,127));
        buf[10+2] = (unsigned char)(ofClamp(app->parameterMap[user3]*127,0,127));
        buf[10+3] = (unsigned char)(ofClamp(app->parameterMap[user4]*127,0,127));
        buf[10+4] = (unsigned char)(ofClamp(app->parameterMap[user5]*127,0,127));
        
        buf[24] = '\n';
        
        //sanity check
      for(int i=0;i<24;i++)
          if(buf[i]=='\n')
              buf[i] = 11;
        //            buf[i] += 48;
        
//        cout << "send " << (int)buf[0] << " " << (int)buf[1] << " "
//        << (int)buf[2]  << " " << (int)buf[3]<< " " << (int)buf[5] << endl
//        << (int)buf[10] << "|"  << (int)buf[11] << "|"  << (int)buf[12] << "|"  << (int)buf[13] << "|" << endl;
        
        serial.writeBytes(buf, 25);
        serial.flush();
        
        
        if(app->parameterMap[ledAuto]==0){
        app->parameterMap[ledEvent] = app->deltaMap[ledEvent] = 0;
        }
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
    
    void mousePressed(int x, int y, int button){}
    void mouseDragged(int x, int y, int button){}
    void mouseMoved(int x, int y){}
    void touchMoved(ofTouchEventArgs &touch){}
    void keyPressed(int key){}
    
    void midiEvent(ofxMidiMessage& eventArgs){}
    void oscEvent(std::string header, std::string arg){}
    void oscEvent(std::string header, std::vector<float> args){}
    
    void saveMacro(stringstream *xml){};
    void loadMacro(ofXml *xml){};
    
    std::string getInfo(){};
    
    bool isBackground(){return back;}

    void closeSerial(){serial.close();}
    
private:
    ofSerial serial;
    int NB_STRIPS = 1;
    std::vector<Strip*> strips;
    unsigned char buf[25];
    
    unsigned char eventId = 0;
    unsigned char user[5];
    
    unsigned char mode,sync,tint,strobeMode,info,event,bright,period;
};


#endif
