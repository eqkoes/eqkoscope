#ifndef __BORING_ANGEL__UX278__
#define __BORING_ANGEL__UX278__

#include <iostream>
#include <ofGraphics.h>
#include "eqkoscope.h"

class UX278: public ofApp{
    
public:
    UX278(): ofApp()
    {}
    
    void setup();
    void update();
    void draw();
    void analyzeAudio();
    
private:
    bool INSTALLATION = false;
    int presence = 0;
    float movementRatio = 0;
    
    float respirationRate = 1;
    float respiration = 0;
    float breaths = 0;
    float meanZ = -1, meanY=-1, meanX=-1, lastMeanX=0, lastMeanY=0;
    float xExtent = 0;
    float hiArms = 0;
    
    int MAX_PRESENCE = 400;
    int MIN_PRESENCE = 100;
    int MAX_MEANZ = 700;
    int MIN_MEANY = 10;
    int MIN_MEANX = 10;
};

#endif
