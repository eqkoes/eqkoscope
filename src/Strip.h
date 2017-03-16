//
//  Strip.h
//  eqkoscope
//
//  Created by Raphaël on 20/02/2017.
//
//

#ifndef eqkoscope_Strip_h
#define eqkoscope_Strip_h
#include "utils.h"

class Strip{
    
public:
    
    Strip(int nLeds, ofSerial* serial){
        this->nLeds = nLeds;
        this->serial = serial;
    }
    
    void setColor(int index, ofColor c){
        
    }
    
    void update(){
//        i.resize(NUM_LEDS, 1);
//        for(int c=0;c<NUM_LEDS;c++){
//            serial.writeByte(buf[3*c]);
//            serial.writeByte(buf[3*c+1]);
//            serial.writeByte(buf[3*c+2]);
//        }
//        serial.writeByte('\n');
//        serial.flush();
    }
    
private:
    int nLeds;
    ofSerial *serial;

};


#endif
