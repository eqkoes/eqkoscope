//
//  point.h
//  BORING_ANGEL
//
//  Created by Raphaël on 01/03/2015.
//
//

#ifndef BORING_ANGEL_point_h
#define BORING_ANGEL_point_h

#include <ofBaseTypes.h>

class EPoint: public ofVec3f{
public:
    EPoint(ofVec3f position, float size){
        this->x = position.x;
        this->y = position.y;
        this->z = position.z;
        intensity = 0.01;
        this->size = size;
    }
    
    void update(){
        if(sens)
            intensity += decay;
        else
            intensity -= decay;
        if(intensity>=1)
            sens = false;
        
    }
    
    float size;
    float intensity;
    float decay = 0.2;
    bool sens = true;
};

#endif
