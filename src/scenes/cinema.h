#ifndef emptyExample_Cinema_h
#define emptyExample_Cinema_h

#include "ofxMidi.h"
#include "scene.h"
#include "utils.h"
#include <ofBaseTypes.h>
#include <ofPixels.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <ofUtils.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>

using namespace std;


#define ALPHABLACK 0
#define SCREEN 1
#define TRUED 2
#define MULTIPLY 3
#define ADD 4
#define SUBTRACT 5
#define INCRUSTATION 6
#define PROLLAGE 7

class Cinema : public Scene
{
    
public:
    Cinema(AbstractApp* app) : Scene(app, "cinema"){

        sceneID = "cinema";
        
        fbo.allocate(WIDTH, HEIGHT, GL_RGB);
        remanentFbo.allocate(WIDTH, HEIGHT2, GL_RGB);
        copyFbo.allocate(WIDTH,HEIGHT, GL_RGB);
        circleMask.loadImage("assets/maskCircle1280x720.png");
        circleMask.resize(WIDTH,HEIGHT);
        
        load();
        
        brightnessContrastShader.load("../shaders/brightnessContrast");
        circleShader.load("../shaders/circle");
        alphaShader.load("../shaders/alphaBlack");
        alphaWhiteShader.load("../shaders/alphawhite");
        alphaScreenShader.load("../shaders/alphascreen");
        contourShader.load("../shaders/contour");
        
        for(int i=0;i<4;i++){
            players.push_back(new ofVideoPlayer());
            loadLaterMoviePosition.push_back(0);
            setMixer(i,0);
        }
        
        loadMovie(0, 0, 0, true);
        setMixer(0,1); //need to load
        
        for(int i=0;i<players.size();i++)
            if(players[i]!=0)
                players[i]->stop();
        
        initParameters();
        
//        vlt.stzartThread();
    }
    
    void setup(){
        triggerPlayLater = true;
                
        if(dbIndex>=0 && dbIndex < strdb.size())
            app->blackNWhiteMedia = strdb[dbIndex][0].find("_b&w_")!=string::npos;
    }
    
    void exit(){
        if(app->parameterMap[embedCinema]==0){
        unloadLater = true;
        for(int i=0;i<players.size();i++)
            if(players[i]!=0)
                players[i]->stop();
        }
    }
    
    void draw(){
        
        ofTranslate(0,HEIGHT);
        ofScale(1,-1);
        
        videoMutex.lock();
        
        ofSetColor(ofColor::black);
        ofRect(0,0,WIDTH,HEIGHT2);
        ofSetColor(ofColor::white);
        //adjustBrightness();
        if(app->parameterMap[adjust]){
            brightnessContrastShader.begin();
            brightnessContrastShader.setUniform1f("brightness", app->parameterMap[brightness]);
            brightnessContrastShader.setUniform1f("contrast", app->parameterMap[contrast]);
        }
        
        ofTranslate(WIDTH/2, HEIGHT2/2 + (HEIGHT2 - HEIGHT)/2);
        ofTranslate(app->parameterMap[mediaX]*WIDTH, -app->parameterMap[mediaY]*HEIGHT, app->parameterMap[mediaZ]*HEIGHT);
        ofRotateX(app->parameterMap[mediaRotX]);
        ofRotateY(app->parameterMap[mediaRotY]);
        ofRotateZ(app->parameterMap[mediaRotZ]);
        ofTranslate(-WIDTH/2, -HEIGHT2/2);

        bool first = true;
        int pred = 0;
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        fbo.begin();
        
        if(!app->parameterMap[feedback]){
            ofSetColor(ofColor::black);
            ofRect(0,0,WIDTH,HEIGHT);
            ofSetColor(ofColor::white);
        }
        
        for(int i=0;i<playerIntensities.size();i++){
            
            if(playerIntensities[i]>0){
                if(app->parameterMap[blendType]==SCREEN && !first){
                    alphaScreenShader.begin();
                    players[pred]->update();
                    alphaScreenShader.setUniformTexture("tex1", players[pred]->getTextureReference(), 1);
                    alphaScreenShader.setUniform1f("thresh", 1 - playerIntensities[i]);
                    alphaScreenShader.setUniform1f("WIDTH", WIDTH);
                    alphaScreenShader.setUniform1f("xRatio", players[i]->getWidth()/(float)players[pred]->getWidth());
                    alphaScreenShader.setUniform1f("yRatio", players[i]->getHeight()/(float)players[pred]->getHeight());
                }
                
                if(app->parameterMap[blendType]==ALPHABLACK && playerIntensities[i]<1){
                    alphaShader.begin();
                    alphaShader.setUniform1f("thresh", 1 - playerIntensities[i]);
                }
                
                if(!first && app->parameterMap[blendType]==TRUED){
                    //                    alphaWhiteShader.load("shaders/alphawhite");
                    alphaWhiteShader.begin();
                    players[pred]->update();
                    alphaWhiteShader.setUniformTexture("tex1", players[pred]->getTextureReference(), 1);
                    //alphaWhiteShader.setUniformTexture("tex1", fbo.getTextureReference(), 1);
                    alphaWhiteShader.setUniform1f("thresh", playerIntensities[i]);
                    alphaWhiteShader.setUniform1f("thresh_pred", playerIntensities[pred]);
                    alphaWhiteShader.setUniform1f("WIDTH", WIDTH);
                    alphaWhiteShader.setUniform1f("xRatio", players[i]->getWidth()/(float)players[pred]->getWidth());
                    alphaWhiteShader.setUniform1f("yRatio", players[i]->getHeight()/(float)players[pred]->getHeight());
                }
                
                
                if(first && app->parameterMap[blendType]!=ALPHABLACK){
                    ofSetColor(playerIntensities[i]*255);
                }
                players[i]->draw(app->parameterMap[hMirror]?WIDTH:0,app->parameterMap[vMirror]?HEIGHT:0, app->parameterMap[hMirror]?-WIDTH:WIDTH, app->parameterMap[vMirror]?-HEIGHT:HEIGHT);
                
                if(first && app->parameterMap[blendType]!=ALPHABLACK){
                    ofSetColor(ofColor::white);
                }
                
                if(app->parameterMap[blendType]==ALPHABLACK && playerIntensities[i]<1)
                    alphaShader.end();
                if(!first && app->parameterMap[blendType]==TRUED)
                    alphaWhiteShader.end();
                if(app->parameterMap[blendType]==SCREEN && !first)
                    alphaScreenShader.end();
                
                
                first = false;
                pred = i;
            }
        }
        fbo.end();
        
        
        copyFbo.begin();
        fbo.draw(0,0);
        copyFbo.end();
        pastFbos.push_back(copyFbo);
        if(pastFbos.size()>nbPastFbos)
            pastFbos.erase(pastFbos.begin());
        
        
        /** POST TRAITEMENT **/
        ofPushMatrix();
        ofTranslate(WIDTH/2, HEIGHT/2);
        float max = 50;
        
        if(app->parameterMap[circle]){
            vector<float> pos;
            int zMax = 15;
            for(float s=max;s>=1;s--){
                float sc = fmod(-(s-2) + ofGetFrameNum()*app->parameterMap[circleSpeed], zMax);
                pos.push_back(sc);
            }
            std::sort(pos.begin(), pos.end());
            for(int i=0;i<max;i++){
                ofPushMatrix();
                ofTranslate(0,0,(pos[i]*250 - 3250)*app->parameterMap[circleDist]);
                ofRotate(-app->parameterMap[circleRotation]*(ofGetFrameNum()+pos[i]/(float)zMax*360));
                
                circleShader.begin();
                circleShader.setUniform1f("WIDTH", WIDTH);
                circleShader.setUniform1f("HEIGHT", HEIGHT);
                float alphaRatio = 20;
                circleShader.setUniform1f("alpha", ofMap(pos[i], 0, zMax/alphaRatio, 0, 1, true)*ofMap(pos[i], zMax, zMax-zMax/alphaRatio, 0, 1, true));
                if(pastFbos.size()>i)
                    pastFbos[pastFbos.size()-i-1].draw(-WIDTH/2,-HEIGHT/2);
                else
                    fbo.draw(-WIDTH/2,-HEIGHT/2);
                circleShader.end();
                //ofEnableBlendMode(OF_BLENDMODE_SCREEN);
                circleMask.draw(-WIDTH/2,-HEIGHT/2, WIDTH, HEIGHT);
                //ofEnableBlendMode(OF_BLENDMODE_ALPHA);
                ofPopMatrix();
            }
        }else{
            fbo.draw(-WIDTH/2,-HEIGHT/2);
        }
        
        first = false;
        ofPopMatrix();
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        
        /** POOP **/
        if(app->parameterMap[supZoom]!=1 || app->parameterMap[supRot]!=0){
            ofTranslate(WIDTH/2, HEIGHT2/2);
            int i = 1;
            ofPushMatrix();
            ofScale(app->parameterMap[supZoom]/i, app->parameterMap[supZoom]/i);
            ofRotateZ(app->parameterMap[supRot]/i);
            //            vidPlayer->draw(-WIDTH/2,-HEIGHT/2, WIDTH, HEIGHT);
            ofPopMatrix();
        }
        
        
        if(app->parameterMap[adjust])
            brightnessContrastShader.end();
        videoMutex.unlock();
    }
    
    void adjustBrightness(){
        //        unsigned char * pixels = vidPlayer->getPixels();
        //        unsigned char * newPixels;
        
        float b;
        //        float n = vidPlayer->width*vidPlayer->height;
        //        for (int x = 0; x<vidPlayer->width; x++){
        //            for (int y = 0; y<vidPlayer->height; y++){
        //                int loc = x + y*vidPlayer->width;
        //                b += pixels[loc] / n;
        //
        //            }
        //        }
        app->parameterMap[brightness] = (75 - b)/150.0;
        app->parameterMap[contrast] = 1+(10 - b/10.0)/10.0;
        app->parameterMap[contrast] = 1+app->parameterMap[brightness]*2;
    }
    
    void randomJump(){
        videoMutex.lock();
        players[app->parameterMap[selectedPlayer]]->setFrame(ofRandom(players[app->parameterMap[selectedPlayer]]->getTotalNumFrames()));
        videoMutex.unlock();
    }
    
    void capture(ofFbo* f){
        remanentFbo.begin();
        f->draw(0,0);
        remanentFbo.end();
    }
    
    void mask(){ //post traitement
        ofSetColor(ofColor::white);
        remanentFbo.draw(0, HEIGHT2, WIDTH, -HEIGHT2); //flip the image
        
        if (app->parameterMap[backMask])
            circleMask.draw(0,0, WIDTH, HEIGHT);
    }
    
    void update(){
        if(triggerPlayLater){
            for(int i=0;i<players.size();i++)
                if(players[i]!=0 && !players[i]->isPlaying() && playerIntensities[i]!=0)
                    players[i]->play();
            triggerPlayLater= false;
        }
        
        app->parameterMap[circleRotation] = app->parameterMap[circleRotation] + (app->deltaMap[circleRotation]-app->parameterMap[circleRotation])*0.6;
        app->parameterMap[circleDist] = app->parameterMap[circleDist] + (app->deltaMap[circleDist]-app->parameterMap[circleDist])*0.6;
        
        if(secondLoad==0) //interlaced load for dual movies
            loadMovie(1, secondLoadDBIndex, secondLoadIndex, false);
        if(secondLoad>=0)
            secondLoad--;
        
        //        if(syncToTempo){
        //            int index = 0;
        //            int sceneIndex = 0;
        //            double posToFrame = players[0]->getTotalNumFrames();
        //            loopStart = playerScenes[strdb[index][sceneIndex]] * posToFrame;
        //            loopLength = (playerScenes[index][sceneIndex+1]-playerScenes[index][sceneIndex]) * posToFrame;
        //            if(ofGetFrameNum()>3)
        //                players[0]->setSpeed(app->bpm*60*ofGetFrameRate()/(float)(4*loopLength));
        //        }
        
        videoMutex.lock();
        
        if(app->parameterMap[loopLength]>0){
            int p = players[app->parameterMap[selectedPlayer]]->getCurrentFrame()-app->parameterMap[loopStart];
            if(p>app->parameterMap[loopLength]-1 || p<-app->parameterMap[loopLength]){
                if(app->parameterMap[loopMode]==1){
                    int sens = p>app->parameterMap[loopLength]-1 ? -1 : 1;
                    if(app->parameterMap[movieSpeed] != abs(app->parameterMap[movieSpeed])*sens){
                        app->parameterMap[movieSpeed] = abs(app->parameterMap[movieSpeed])*sens;
                        cout << "set speed " << app->parameterMap[movieSpeed]*sens << endl;
                    }
                }else{
                    int frame = max(0.0f, app->parameterMap[loopStart]);
                    players[app->parameterMap[selectedPlayer]]->setFrame(frame);
                }
                
            }
        }
        
        for(int i=0;i<playerIntensities.size();i++)
            if(playerIntensities[i]>0){
                if(app->parameterMap[movieSpeed]!=players[i]->getSpeed())
                    players[i]->setSpeed(app->parameterMap[movieSpeed]);
                players[i]->update();
            }
        
        
        
        videoMutex.unlock();
    }
    
    void pause(bool b){
        if(b){
            for(int i=0;i<players.size();i++){
                if(players[i]!=0){
                    if(b){
                    players[i]->setPaused(true);
                    }else{
                    players[i]->play();
                    }
                }
            }
        }
    }
    
    void setMixer(int i, float f){ //put in update
        if(i >= 2)
            return;
        videoMutex.lock();
        if(i>=0 && i<players.size()){
            if(f>0 && !players[i]->isPlaying()){
                players[i]->play();
                players[i]->setSpeed(1);
                players[i]->update();
            }
            playerIntensities[i] = f;
            if(f==0 && players[i]->isPlaying()){
                //                        players[i]->closeMovie();
                players[i]->stop(); //??
            }
            app->parameterMap[selectedPlayer] = i;
        }
        videoMutex.unlock();
    }
    
    void loadMovie(int player, int dbIndex, int i, bool randomPos){
        app->parameterMap[loopLength] = 0;
        app->programChangeFlag = 1;
        
        if(ofGetFrameNum()-lastloadDate < 1)
            return;
        
        lastloadDate = ofGetFrameNum();
        
        if(player >= 2)
            return;
        if(dbIndex>=0 && dbIndex<strdb.size() && i>=0 && i<strdb[dbIndex].size()){
            vidIndex = i;
            this->dbIndex = dbIndex;
            
            cout << "loading " << strdb[dbIndex][i]  << " to slot " << player << endl;
            
            app->blackNWhiteMedia = strdb[dbIndex][0].find("_b&w_")!=string::npos;
            
            // do async
            players[player]->stop();
            ofSleepMillis(3); //so that there's no mutex bug
            
#ifdef USE_PRELOADED_MOVIES
            ofVideoPlayer* inDB = 0;
            for(ofVideoPlayer* p : preloadedPlayers)
                if(p == players[player]){
                    inDB = p;
                    break;
                }
            if(!inDB){
#endif
                players[player]->closeMovie();
                delete players[player];
     #ifdef USE_PRELOADED_MOVIES
            }
#endif
            
            #ifdef USE_PRELOADED_MOVIES
            ofFile f(strdb[dbIndex][i]);
            ofVideoPlayer* found = 0;
            for(ofVideoPlayer* p : preloadedPlayers){
                ofFile fp(p->getMoviePath());
                if(!f.getFileName().compare(fp.getFileName())){
                    found = p;
                    break;
                }
            }
            if(found){
                players[player] = found;
            }else{
#endif
                players[player] = new ofVideoPlayer;
                try{
                    string name = strdb[dbIndex][i];
                    if(name.compare(""))
                players[player]->loadMovie(name);
                } 
                catch ( const std::exception & e )
                {
                    cout << e.what() << endl;
                }
            #ifdef USE_PRELOADED_MOVIES
            }
#endif
            
            
            
//            vlt.load(strdb[dbIndex][i]);
            if(app->parameterMap[randomSpeed]>0){
                bool up = ofRandom(1)>0.5;
                players[player]->setSpeed(up ? ofRandom(1, app->parameterMap[randomSpeed]*4) : ofRandom(1-app->parameterMap[randomSpeed], 1));
            }
            if(!players[player]->isPlaying())
                players[player]->play();
            if(strdb[dbIndex][i].find("sound")!=string::npos){
                players[player]->setVolume(1);
            }else{
                players[player]->setVolume(0);
            }
            if(randomPos){
                float pos;
                if(playerScenes[strdb[dbIndex][i]].size()>0)
                    pos = playerScenes[strdb[dbIndex][i]][(int)ofRandom(playerScenes[strdb[dbIndex][i]].size()-0.001)];
                else
                    pos = ofRandom(1);
                players[player]->setPosition(pos);
            }else{
                players[player]->setPosition(loadLaterMoviePosition[player]);
            }
        }
    }
    
    void mouseDragged(int x, int y, int button){
    }
    
    void touchMoved(ofTouchEventArgs &touch){
        
    }
    
    void setSleep(int s){
        sleep = s;
        wasSleep = s;
    }
    
    ofTexture* getTexture(){
        return &(players[0]->getTextureReference());
//        return players[0]->getTexture();
    }
    
    
    
    void parseMidi(ofxMidiMessage eventArgs){
        float value = eventArgs.value;
        switch(eventArgs.channel){
            case 4: case 3:{
            switch(eventArgs.status){
            case MIDI_CONTROL_CHANGE:{
                if(eventArgs.channel!=4)
                    return;
                switch(eventArgs.control){
                case 1:
                    app->deltaMap[circleRotation] = (value-64)/127 * 2;
                    break;
                case 28:
                    app->deltaMap[circleRotation] = 0;
                    break;
                case 2:
                    app->deltaMap[circleDist] = value/127*2;
                    break;
                case 29:
                    app->deltaMap[circleDist] = 1;
                    break;
                case 4:
                    app->deltaMap[circleSpeed] = (value)/127 * 1;
                    break;
                        
                    case 6:{
                        int s = (value-64)/64.0 * 4;
                        if(s==0) s++;
                        app->deltaMap[movieSpeed] = app->parameterMap[movieSpeed] = s;
                    }break;
                        
                case 30:
                    app->parameterMap[circleSpeed] = 1;
                    break;
//                case 6:
//                    setMixer(0, value/127.0);
//                    break;
//                case 7:
//                    setMixer(1, value/127.0);
//                    break;
                    /* case 7:{
                                float speed=1;
                                if(value < 1)
                                    speed = 0.25;
                                else if(value/127.0 < 0.25)
                                    speed = 0.5;
                                else if(value/127.0 < 0.75)
                                    speed = 1;
                                else if(value < 127)
                                    speed = 2;
                                else
                                    speed = 4;

                                if(selectedPlayer==0 && players[0]->getSpeed()<0 && laterPosition>-1){
                                    players[0]->setFrame(laterPosition);
                                    laterPosition = -1;
                                }
                                
                                if(players[selectedPlayer]!=0 && players[selectedPlayer]->getSpeed()!=speed)
                                    players[selectedPlayer]->setSpeed(speed);
                            }
                                break;
                            case 8:{
                                float speed=1;
                                if(value < 1)
                                    speed = 0.25;
                                else if(value/127.0 < 0.25)
                                    speed = 0.5;
                                else if(value/127.0 < 0.75)
                                    speed = 1;
                                else if(value < 127)
                                    speed = 2;
                                else
                                    speed = 4;
                                speed = -speed;
                                if(selectedPlayer == 0)
                                    laterPosition = players[0]->getCurrentFrame();
                                if(players[selectedPlayer]!=0 && players[selectedPlayer]->getSpeed()!=speed)
                                    players[selectedPlayer]->setSpeed(speed);
                            }
                                break;*/
                case 12: app->parameterMap[blendType] = value/127.0<0.33 ? ALPHABLACK : value/127.0<0.66 ? TRUED : SCREEN; break;
                case 16 : app->parameterMap[circle] = 3*value/128.0;break;
                }
                switch(eventArgs.control){
                case 28:
                    app->parameterMap[selectedPlayer] = 0;
                    break;
                case 29:
                    app->parameterMap[selectedPlayer] = 1;
                    break;

                case 41:
                    if(!app->safeMode){
                        if(players[app->parameterMap[selectedPlayer]]!=0 && players[app->parameterMap[selectedPlayer]]->getSpeed()!=1)
                            players[app->parameterMap[selectedPlayer]]->setSpeed(1);
                    }
                    break;
                case 46:
                    if(value==127)
                        app->parameterMap[vMirror] = !app->parameterMap[vMirror];
                    break;
                case 47:
                    if(value==127)
                        app->parameterMap[hMirror] = !app->parameterMap[hMirror];
                    break;
                }
            }
                break;
            case MIDI_NOTE_ON:{
                switch(eventArgs.pitch){
                    case 48:case 49: randomPosition(ofRandom(0,1));break;
                    case 45: randomScene("U");break;
                    case 40: randomScene("L");break;
                    case 41: randomScene("S");break;
                    case 42: randomScene("R");break;
                    case 37: randomScene("D");break;
                default:
                        if(eventArgs.pitch>=52 && eventArgs.pitch<67){ //laod scene or pos
                            randomPosition(ofRandom((eventArgs.pitch-52)/15.0, (1+eventArgs.pitch-52)/15.0));
                        }
                }
            }
                break;
            case MIDI_NOTE_OFF:{
                if(app->parameterMap[loopLength]>0){
                    app->parameterMap[loopLength] = 0;
                }
            }break;
            default:;
            }
        }break;
        default:;
        }
    }
    
    
    void randomPosition(float r){
        string path = players[app->parameterMap[selectedPlayer]]->getMoviePath();
        float pos, pos2;
        if(playerScenes[path].size()>0){
            int s = (int) (r * playerScenes[path].size());
            pos = playerScenes[path][s];
            pos2 = s<playerScenes[path].size()-1 ? playerScenes[path][s+1] : players[app->parameterMap[selectedPlayer]]->getTotalNumFrames()-1;
            players[app->parameterMap[selectedPlayer]]->setFrame(pos);
            app->parameterMap[loopLength] = pos2-pos;
            app->parameterMap[loopStart] = pos;
        }else{
            players[app->parameterMap[selectedPlayer]]->setPosition(r);
        }
    }
    
    void randomScene(string tag){
        string path = players[app->parameterMap[selectedPlayer]]->getMoviePath();
        float pos, pos2;
        if(playerScenes[path].size()>0){
            vector<int> indexes;
            for(int i=0;i<playerScenesMovements[path].size();i++)
                if(!playerScenesMovements[path][i].compare(tag))
                    indexes.push_back(playerScenes[path][i]);
            if(indexes.size()==0){
                randomPosition(ofRandom(1));
                return;
            }
            
            int sx = (int) (ofRandom(1) * indexes.size());
            pos = indexes[sx];
            for(int i=0;i<playerScenes[path].size();i++)
                if(pos==playerScenes[path][i])
                        pos2 = i<playerScenes[path].size()-1 ? playerScenes[path][i+1] : players[app->parameterMap[selectedPlayer]]->getTotalNumFrames()-1;
            players[app->parameterMap[selectedPlayer]]->setFrame(pos);
            app->parameterMap[loopLength] = pos2-pos;
            app->parameterMap[loopStart] = pos;
        }else{
            players[app->parameterMap[selectedPlayer]]->setPosition(ofRandom(1));
        }
    }
    
    void midiEvent(ofxMidiMessage& eventArgs){
        parseMidi(eventArgs);
    }
    
    void keyPressed(int key){
        switch(key){
        case ' ':{
            dbIndex = ofRandom(strdb.size());
            loadLaterIndex = ofRandom(strdb[dbIndex].size());
            loadMovie(app->parameterMap[selectedPlayer], dbIndex, loadLaterIndex, true);
        }break;
        case '<':
            app->parameterMap[adjust] = !app->parameterMap[adjust];
            break;
        case '&':
            app->parameterMap[selectedPlayer] = 0;break;
        case 233:
            app->parameterMap[selectedPlayer] = 1;break;
                case 'n':
                randomPosition(ofRandom(0,1));
                break;
        default:;

        }
    };
    void oscEvent(std::string header, std::string file){}
    
    void oscEvent(std::string header, std::vector<float> args){}
    
    void loadDirectMovie(string vidPath){
        bool found = false;
        for(int dbIndex=0;dbIndex<strdb.size();dbIndex++){
            for(int mIndex=0;mIndex<strdb[dbIndex].size();mIndex++){
                ofFile f(strdb[dbIndex][mIndex]);
                string name = f.getFileName();
                if(!name.compare(vidPath)){
                    loadMovie(currentVidIndex, dbIndex, mIndex, true);
                    found = true;
                    break;
                }
            }
            if(found)
                break;
        }
    }
    
    void load(){
        ofDirectory dir("cinema/");
        dir.listDir();
        vector<string> dirs;
        for(int i=0;i<dir.numFiles();i++){
            string d = dir.getPath(i);
            if(d.find("DB")!=string::npos){
                vector<string> vids;
                vector<ofVideoPlayer*> ps;
                ofDirectory dir2(d);
                dir2.listDir();
                for(int j=0;j<dir2.numFiles();j++){
                    vector<double> scene;
                    vector<string> movs;
                    string vidpath = dir2.getPath(j);
                    string origVidPath = vidpath;
                    if(vidpath.find(".mov")!=string::npos || vidpath.find(".mp4")!=string::npos ||
                            vidpath.find(".avi")!=string::npos ||
                            vidpath.find(".3gp")!=string::npos
                       || vidpath.find(".webm")!=string::npos){
                        vids.push_back(vidpath);
                        playerPaths.push_back(vidpath);
                        ofVideoPlayer* p = new ofVideoPlayer;
                        playerIntensities.push_back(0);
                        ps.push_back(p);
                        
                        ofStringReplace(vidpath, ".mov", ".txt");
                        ofStringReplace(vidpath, ".mp4", ".txt");
                        string t = ofBufferFromFile(vidpath).getText();
                        if(t.compare("")){
                            vector<string > splitted = ofSplitString(t, "\n");
                            for(int x=0;x<splitted.size();x++){
                                vector<string> line = ofSplitString(splitted[x]," ");
                                scene.push_back(ofToDouble(line[0]));
                                if(line.size()>1)
                                    movs.push_back((line[1]));
                                else
                                    movs.push_back(" ");
                            }
                        }
                    }
                    playerScenes[origVidPath] = scene;
                    playerScenesMovements[origVidPath] = movs;
                }
                strdb.push_back(vids);
            }
        }
    }
    
    void saveMacro(stringstream *xml){
        (*xml) << "<cinema>" << endl;
    

        stringstream ss;
        for(int i=0;i<3;i++){
            ss.str("");
            ss << "playerIntensities" << i ;
            saveParameterXML(xml, ss.str(), playerIntensities[i]);
            ss.str("");
            ss << "vidPath" << i ;
            saveParameterXML(xml, ss.str(), players[i]->getMoviePath());
            ss.str("");
            ss << "vidPosition" << i ;
            saveParameterXML(xml, ss.str(), players[i]->getPosition());
        }
        
        (*xml) << "</cinema>" << endl;
    }
    
    void loadMacro(ofXml *xml){
        videoMutex.lock();
        
        stringstream ss;
        for(int i=0;i<3;i++){
            ss.str("");
            
            ss << "playerIntensities" << i ;
            float intensity = xml->getFloatValue(ss.str());
            cout << "intensity : " << i << " " << intensity << endl;
            
            ss.str("");
            ss << "vidPath" << i ;
            string vidPath = xml->getValue(ss.str());
            
            ss.str("");
            ss << "vidPosition" << i ;
            float position = xml->getFloatValue(ss.str());
            
            
            playerIntensities[i] = intensity;
            if(intensity>0){
                cout << "loading cinema macro path "  << vidPath << endl;
                ofFile f1(vidPath);
                ofFile f2(players[i]->getMoviePath());
                if(f1.getFileName().compare(f2.getFileName())){ //check current vid
                    bool found = false;
                    for(int dbIndex=0;dbIndex<strdb.size();dbIndex++){
                        for(int mIndex=0;mIndex<strdb[dbIndex].size();mIndex++){
                            f2 = ofFile(strdb[dbIndex][mIndex]);
                            if(!f2.getFileName().compare(f1.getFileName())){
                                if(i==0){
                                    loadLaterMoviePosition[i] = position;
                                    loadMovie(i, dbIndex, mIndex, false);
                                }else{
                                    secondLoad = 5;
                                    secondLoadDBIndex = dbIndex;
                                    secondLoadIndex = mIndex;
                                }
                                found = true;
                                break;
                            }
                        }
                        if(found)
                            break;
                        else
                            loadLaterMoviePosition[i] = position;
                    }
                }else{
                    players[i]->setPosition(position);
                }
            }
            app->parameterMap[selectedPlayer] = 0;
        }
        
        videoMutex.unlock();
    }
    
    void setPosition(float f){
        players[app->parameterMap[selectedPlayer]]->setPosition(f);
    }
    
    std::string getInfo(){
        stringstream str;
        str << "Media position : " << players[0]->getPosition() << "(f. " << players[0]->getCurrentFrame() << ")";
        return str.str();;
    }
    
    void preloadVideos(vector<string> vids){
        #ifdef USE_PRELOADED_MOVIES
        for(string path: vids){
            ofFile f(path);
            bool found = false;
            for(int i=0;i<preloadedPlayers.size();i++){
                ofVideoPlayer* p = preloadedPlayers[i];
                ofFile fp(p->getMoviePath());
                if(!f.getFileName().compare(fp.getFileName())){
                    found = true;
                    break;
                }
            }
            if(!found){
                ofVideoPlayer* p = new ofVideoPlayer();
                p->loadMovie(path);
                preloadedPlayers.push_back(p);
                cout << "PRELOADED " << path << endl;
            }
        }
#endif
    }
    
    ofFbo remanentFbo;
    ofImage circleMask;
    
    //VIDEO
    std::vector<std::vector<std::string> > strdb;
    
    std::vector<ofVideoPlayer*> players;
    std::vector<string> playerPaths;
    std::vector<float> playerIntensities;
    std::map<std::string, std::vector<double> > playerScenes;
    std::map<std::string, std::vector<string> > playerScenesMovements;
    int currentVidIndex = 0;
    int dbIndex = 0;
    int vidIndex = 0;
    
    ofShader brightnessContrastShader, circleShader;

    ofMutex videoMutex;
    
    bool unloadLater = false;
    float laterPosition = -1;
    vector<float> loadLaterMoviePosition;
    int loadLaterIndex = -1;
    
    int secondLoad = -1;
    int secondLoadDBIndex = -1;
    int secondLoadIndex = -1;
    
    ofShader alphaShader, contourShader, alphaWhiteShader, alphaScreenShader;
    ofFbo fbo;
    vector<ofFbo> pastFbos;
    int nbPastFbos = 50;
    
    bool triggerPlayLater = false;
    
    ofFbo copyFbo;
    
    int lastloadDate = -10;
    
    float updateMixerValue = 0;
    
    vector<ofVideoPlayer*> preloadedPlayers;
    
    int sleep = 0;
    bool wasSleep = false;
    
//    VideoLoadingThread vlt;
};

#endif
