//
//  thoughts.h
//  eqkoscope
//
//  Created by Raphaël on 29/12/2017.
//
//

#ifndef eqkoscope_thoughts_h
#define eqkoscope_thoughts_h

#include "ofTypes.h"
#include <stdlib.h>

using namespace std;

struct thought{
    string text;
    ofSoundPlayer sound;
};

class Thoughts{
    
public:
    
    Thoughts(AbstractApp* app){
        this->app = app;
        
        loadData("thoughts/hight.csv",
             &(sentences["high"]));
        loadData("thoughts/low.csv",
                 &(sentences["low"]));
        loadData("thoughts/com.csv",
                 &(sentences["com"]));
        
    }
    
    void loadData(string path, vector<thought> *data){
        dataFile.open(path);
        vector <string> dataTxt = ofSplitString(dataFile.readToBuffer().getText(), "\n");
        for(string line : dataTxt){
            vector <string> csvTxt = ofSplitString(line, "/");
            thought t;
            t.text = csvTxt[0];
            if(csvTxt.size()>1){
                t.sound.loadSound(csvTxt[1]);
            }else{
                t.sound.loadSound("thoughts/default.wav");
            }
            t.sound.setMultiPlay(true);
            data->push_back(t);
        }
    }
    
    void update(){
        if(ofRandom(1)<0.01){
            index = ofRandom(1);
            app->deltaMap[user1] = index;
            app->deltaMap[user2] = 0.1f;
            if(index<0.3){
                sentence = sentences["high"][ofRandom(sentences["high"].size())];
            }else{
                if(index<0.7){
                    sentence = sentences["com"][ofRandom(sentences["com"].size())];
                }else{
                    sentence = sentences["low"][ofRandom(sentences["low"].size())];
                }
            }
            if(sentence.sound.isLoaded() && !sentence.sound.getIsPlaying())
                sentence.sound.play();
            cout << "CURRENT THOUGHT : " << sentence.text << endl;
        }
    }
    
private:
    ofFile dataFile;
    AbstractApp* app;
    float index = 0;
    map<string, vector<thought> > sentences;
    thought sentence;
//    std::vector<float> high, low, com;
    
};


#endif
