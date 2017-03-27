//
//  freezer.h
//  emptyExample
//
//  Created by Raphaël on 12/12/2014.
//
//

#ifndef emptyExample_freezer_h
#define emptyExample_freezer_h

#include <vector>
#include "utils.h"
#include "AbstractApp.h"

//add glitches to the image using display()
//todo use fbo pool

class Freezer {
    
public:
    
    Freezer(AbstractApp* app) {
        this->app = app;
    }
    
    // add a black glitch
    void block() {
        int w = int(ofRandom(40, WIDTH));
        int h = 10*int(ofRandom(20));
        int x = WIDTH/2-w/2, y=HEIGHT/2-h/2;
        if(!centered){
            x = int(ofRandom(WIDTH+w))-w;
        }
        y = 10*int(ofRandom(HEIGHT/10));
        ofFbo* img = new ofFbo();
        img->begin();
        ofSetColor(0);
        ofRect(0,0,w,h);
        img->end();
        img->allocate(w, h);
        imgs.push_back(img);
        coords.push_back(new ofVec2f(x, y));
    }
    
    // add a colored glitch
    void ablock() {
        int w = int(ofRandom(40, WIDTH));
        int h = 10*int(ofRandom(20));
        int x = int(ofRandom(WIDTH+w))-w;
        int y = 10*int(ofRandom(HEIGHT/10));
        ofFbo* img = new ofFbo();
        img->allocate(w, h);
        ofColor c;
        if(app->parameterMap[tintSaturation]==0)
            c = ofColor(ofRandom(255), ofRandom(255), ofRandom(255), ofRandom(100));
        else{
            c = ofColor::fromHsb(app->parameterMap[tintHue]*100, app->parameterMap[tintSaturation]*100, ofRandom(100), ofRandom(100));
        }
        img->begin();
        ofSetColor(c);
        ofRect(0,0,w,h);
        img->end();
        imgs.push_back(img);
        coords.push_back(new ofVec2f(x, y));
    }
    
    //add a shift glitch
    void shift(ofFbo* src) {
        if(ofRandom(1)>proba)
            return;
        int w = int(ofRandom(40, WIDTH));
        int h = 10*int(ofRandom(20));
        ofFbo* img = new ofFbo();
        img->allocate(w, h); //memory leak
        int sx = int(ofRandom(0, WIDTH - w)); //shift values
        int sy = int(ofRandom(0, HEIGHT - h));
        int x=WIDTH/2-w/2, y=HEIGHT/2-h/2;
        if(!centered)
            x = int(ofRandom(WIDTH-w));
        y = int(ofRandom(HEIGHT-h));
        img->begin();
        src->draw(-sx,-sy);
        img->end();
        shiftimgs.push_back(img);
        shiftcoords.push_back(new ofVec2f(x, y));
    }
    
    void display() {
        if (imgs.size()==0 && shiftimgs.size()==0)
            return;
        update();
        for (int i=0; i<imgs.size (); i++)
            imgs[i]->draw(coords[i]->x, coords[i]->y +  (HEIGHT2-HEIGHT)/2);
        for (int i=0; i<shiftimgs.size (); i++)
            shiftimgs[i]->draw(shiftcoords[i]->x, shiftcoords[i]->y + (HEIGHT2-HEIGHT)/2);
    }
    
    void update() {
        while (imgs.size()> min(app->parameterMap[nBlocks], MAX_IMGS)) {
            int ri = int(ofRandom(imgs.size()));
            delete imgs[ri];
            delete coords[ri];
            imgs.erase(imgs.begin() + (ri)); //bourrin
            coords.erase(coords.begin() + (ri));
        }
        while (shiftimgs.size ()>min(app->parameterMap[nFreeze], MAX_SHIFT)) {
            int ri = int(ofRandom(shiftimgs.size()));
            delete shiftimgs[ri];
            delete shiftcoords[ri];
            shiftimgs.erase(shiftimgs.begin() + ri); //bourrin
            shiftcoords.erase(shiftcoords.begin() + ri);
        }
    }
    
    bool isActive(){
        return shiftimgs.size()>0;
    }
    
    void clear() {
        imgs.clear();
        coords.clear();
        shiftimgs.clear();
        shiftcoords.clear();
    }
    
    bool centered = false;
    
private:
    AbstractApp* app;
    
    vector<ofVec2f*> coords;
    vector<ofFbo*> imgs;
    vector<ofFbo*> shiftimgs;
    vector<ofVec2f*> shiftcoords;
    
    float proba = 0.4;
    
    float MAX_IMGS = 15;
    float MAX_SHIFT = 15;
};



#endif
