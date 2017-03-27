#ifndef eqkoscope_ofApp_LEAP_h
#define eqkoscope_ofApp_LEAP_h

#include "eqkoscope.h"

void eqkoscope::updateLeap(){
#ifdef LEAPMOTION
    fingersFound.clear();
    long s = ofGetElapsedTimeMicros();
    if(parameterMap[leapAutoReco] && !leap.isConnected()){
        cout << "reco leap" << endl;
        leap.open();
    }
    if(leap.isFrameNew()){
        simpleHands = leap.getSimpleHands();
        bool isLeft = false, isRight = false;
        float meanX = 0;
        float meanY = 0;
        float meanZ = 0;
        float meanRoll = 0;
        float meanDX = 0;
        float meanDY = 0;
        float meanDZ = 0;
        
        if(simpleHands.size()){
            float leapEasing = 0.6;
            
            leap.setMappingX(-200, 200, 0, 1);
            leap.setMappingY(150, 490, 0, 1);
            leap.setMappingZ(-200, 200, 0, 1);
            
            for(int i = 0; i < simpleHands.size(); i++){
                if(leap.getLeapHands()[0].grabStrength()==1) //closed fist
                    continue;
                
                meanX += simpleHands[i].handPos.x;
                meanY += simpleHands[i].handPos.y;
                meanZ += simpleHands[i].handPos.z;
                meanRoll += abs(simpleHands[i].handNormal.x);
                meanDX += simpleHands[i].handVelocity.x/300.0;
                meanDY += simpleHands[i].handVelocity.y/300.0;
                meanDZ += simpleHands[i].handVelocity.z/300.0;
                
                //adaptative easing
                leapX[i] += (meanX-leapX[i])*(ofMap(abs(meanX-leapX[i]), 0.25, 0, 1, leapEasing, true));
                leapY[i] += (meanY-leapY[i])*(ofMap(abs(meanY-leapY[i]), 0.25, 0, 1, leapEasing, true));
                leapZ[i] += (meanZ-leapZ[i])*(ofMap(abs(meanZ-leapZ[i]), 0.25, 0, 1, leapEasing, true));
                leapDX[i] += (meanDX-leapDX[i])*(ofMap(abs(meanDX-leapDX[i]), 0.25, 0, 1, leapEasing, true));
                leapDY[i] += (meanDY-leapDY[i])*(ofMap(abs(meanDY-leapDY[i]), 0.25, 0, 1, leapEasing, true));
                leapDZ[i] += (meanDZ-leapDZ[i])*(ofMap(abs(meanDZ-leapDZ[i]), 0.25, 0, 1, leapEasing, true));
                leapRoll[i] += (meanRoll-leapRoll[i])*(ofMap(abs(meanRoll-leapRoll[i]), 0.25, 0, 1, leapEasing, true));
                
                //precision (too jittery instead)
                float precision = 200;
                //        leapX = int(leapX*precision)/precision;
                //        leapY = int(leapY*precision)/precision;
                //        leapZ = int(leapZ*precision)/precision;
                //        leapDX = int(leapDX*precision)/precision;
                //        leapDY = int(leapDY*precision)/precision;
                //        leapDZ = int(leapDZ*precision)/precision;
                //        leapRoll = int(leapRoll*precision)/precision;
                
                ofTouchEventArgs args;
                args.set(meanX, meanY); //todo
                args.pressure = meanZ;
                
                if(simpleHands[0].handNormal.y > 0.9)
                    args.pressure = 12345;
                if(simpleHands[0].fingers.size()>0 || simpleHands[1].fingers.size()>0){
                    if(scenes[parameterMap[currentScene]]==drawscene)
                        scenes[parameterMap[currentScene]]->touchMoved(args);
                }
                
                if(leap.getLeapHands()[i].isLeft()){
                    isLeft = true;
                    for(Auto *a: leapAutos){
                        if(!a->leapDimension.compare("xl"))
                            a->update(meanX);
                        if(!a->leapDimension.compare("yl"))
                            a->update(meanY);
                        if(!a->leapDimension.compare("zl"))
                            a->update(meanZ);
                    }
                }
                if(leap.getLeapHands()[i].isRight()){
                    isRight = true;
                    for(Auto *a: leapAutos){
                        if(!a->leapDimension.compare("xr"))
                            a->update(meanX);
                        if(!a->leapDimension.compare("yr"))
                            a->update(meanY);
                        if(!a->leapDimension.compare("zr"))
                            a->update(meanZ);
                    }
                }
            }
        
            for(Auto *a: leapAutos){
                if(!a->leapDimension.compare("x"))
                    a->update(meanX);
                if(!a->leapDimension.compare("y"))
                    a->update(meanY);
                if(!a->leapDimension.compare("z"))
                    a->update(meanZ);
                if(!isLeft){
                    if(!a->leapDimension.compare("xl"))
                        a->update(0);
                    if(!a->leapDimension.compare("yl"))
                        a->update(0);
                    if(!a->leapDimension.compare("zl"))
                        a->update(0);
                }
                if(!isRight){
                    if(!a->leapDimension.compare("xr"))
                        a->update(0);
                    if(!a->leapDimension.compare("yr"))
                        a->update(0);
                    if(!a->leapDimension.compare("zr"))
                        a->update(0);
                }
            }
        }
        

    }
#endif
}

#endif
