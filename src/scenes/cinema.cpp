

#include "cinema.h"

Cinema::Cinema(AbstractApp* app) : Scene(app, "cinema"){
        
        sceneID = "cinema";
        
        fbo.allocate(WIDTH, HEIGHT, GL_RGB);
        //        remanentFbo.allocate(WIDTH, HEIGHT2, GL_RGB);
        copyFbo.allocate(WIDTH,HEIGHT, GL_RGB);
        //        circleMask.loadImage("assets/maskCircle1280x720.png");
        //        circleMask.resize(WIDTH,HEIGHT);
    
    stillFbo = new ofFbo();
    stillFbo->allocate(WIDTH,HEIGHT, GL_RGB);
        
        load();
        
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
        
        //        vlt.stzartThread();
        
        
    }
    

void Cinema::draw(){
        
                ofTranslate(0,HEIGHT);
               ofScale(1,-1);
        
        videoMutex.lock();
        
        ofSetColor(ofColor::black);
        ofDrawRectangle(0,0,WIDTH,HEIGHT2);
        ofSetColor(ofColor::white);
        
        ofTranslate(WIDTH/2, HEIGHT2/2 + (HEIGHT2 - HEIGHT)/2);
        if(!sleep){ //not background
            ofScale(1+app->parameterMap[mediaZ], 1+app->parameterMap[mediaZ]);
            ofTranslate(app->parameterMap[mediaX]*WIDTH, -app->parameterMap[mediaY]*HEIGHT);
            ofRotateX(app->parameterMap[mediaRotX]);
            ofRotateY(app->parameterMap[mediaRotY]);
            ofRotateZ(app->parameterMap[mediaRotZ]);
        }
        
        
        ofTranslate(-WIDTH/2, -HEIGHT2/2);
        
        ofScale(app->parameterMap[mediaScaleX], app->parameterMap[mediaScaleY]);
    
    
        bool first = true;
        int pred = 0;
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        fbo.begin();
        
        ofSetColor(ofColor::black); //180308
        ofDrawRectangle(0,0,WIDTH,HEIGHT);
        ofSetColor(ofColor::white);
        
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
                if(app->parameterMap[resize]){
                    players[i]->draw(app->parameterMap[hMirror]?WIDTH:0,app->parameterMap[vMirror]?HEIGHT:0, app->parameterMap[hMirror]?-WIDTH:WIDTH, app->parameterMap[vMirror]?-HEIGHT:HEIGHT);
                }else{
                    ofPushMatrix();
                    ofTranslate(WIDTH/2, HEIGHT/2);
                    int w = players[i]->getWidth();
                    int h = players[i]->getHeight();
                    players[i]->draw(app->parameterMap[hMirror] ? w/2 : -w/2,
                                     app->parameterMap[vMirror] ? h/2 : -h/2,
                                     app->parameterMap[hMirror] ? -w : w,
                                     app->parameterMap[vMirror] ? -h : h);
                    ofPopMatrix();
                }
                
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
    
//    if(app->parameterMap[user5]==1){
//        stillFbo->begin();
//        fbo.draw(0,0);
//        stillFbo->end();
//    }
    
        if(app->parameterMap[circle]>0){
            copyFbo.begin();
            fbo.draw(0,0);
            copyFbo.end();
            pastFbos.push_back(copyFbo);
            if(pastFbos.size()>nbPastFbos)
                pastFbos.erase(pastFbos.begin());
        }
        
        
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
                //                circleMask.draw(-WIDTH/2,-HEIGHT/2, WIDTH, HEIGHT);
                ofPopMatrix();
            }
        }else{
            float  s = (ofGetFrameNum() % 500) / 500.0;
            float ss = s*10;
            ofPushMatrix();
            
            ofBackground(0);

            
            if(app->parameterMap[user5]>0){
                ofEnableBlendMode(OF_BLENDMODE_SCREEN);
                ofSetColor(255*app->parameterMap[user5]);
                stillFbo->draw(-WIDTH/2,-HEIGHT/2);
                app->deltaMap[user5] -= 1 / 25.0;
                
                ofSetColor(255 * std::min(1.0, (1.5-app->parameterMap[user5])));
                fbo.draw(-WIDTH/2,-HEIGHT/2);
                ofEnableBlendMode(OF_BLENDMODE_ALPHA);

            }else{
                fbo.draw(-WIDTH/2,-HEIGHT/2);
            }
            
            ofPopMatrix();
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
        
        videoMutex.unlock();
    }
    
void Cinema::randomJump(){
        videoMutex.lock();
        players[app->parameterMap[selectedPlayer]]->setFrame(ofRandom(players[app->parameterMap[selectedPlayer]]->getTotalNumFrames()));
        videoMutex.unlock();
    }

    
void Cinema::update(){
        if (app->deltaMap[switchImg]) {
            app->deltaMap[switchImg] = app->parameterMap[switchImg] = 0;
            randomPosition(ofRandom(0,1));
        }
        
        if(triggerPlayLater){
            for(int i=0;i<players.size();i++)
                if(players[i]!=0 && !players[i]->isPlaying() && playerIntensities[i]!=0)
                    players[i]->play();
            triggerPlayLater= false;
        }
        
        app->parameterMap[circleRotation] = app->parameterMap[circleRotation] + (app->deltaMap[circleRotation]-app->parameterMap[circleRotation])*0.6;
        app->parameterMap[circleDist] = app->parameterMap[circleDist] + (app->deltaMap[circleDist]-app->parameterMap[circleDist])*0.6;
        
        
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
        
        if(app->deltaMap[loopLength]>0){
            int p = players[app->parameterMap[selectedPlayer]]->getCurrentFrame()-app->parameterMap[loopStart];
            if(p>app->deltaMap[loopLength]-1 || p<-app->deltaMap[loopLength]){
                if(app->deltaMap[loopMode]==1){
                    int sens = p>app->parameterMap[loopLength]-1 ? -1 : 1;
                    if(app->parameterMap[movieSpeed] != abs(app->parameterMap[movieSpeed])*sens){
                        app->parameterMap[movieSpeed] = abs(app->parameterMap[movieSpeed])*sens;
                    }
                }else{
                    int frame = max(0.0f, app->deltaMap[loopStart]);
                    players[app->parameterMap[selectedPlayer]]->setFrame(frame);
                }
                
            }
        }
        
        for(int i=0;i<playerIntensities.size();i++)
            if(playerIntensities[i]>0){
                if(app->parameterMap[movieSpeed]!=players[i]->getSpeed())
                    players[i]->setSpeed(app->parameterMap[movieSpeed]);
                if(app->savingGif){ // frame by frame
                    players[i]->setPaused(true);
                    if(players[i]->getPosition() < 1)
                        players[i]->nextFrame();
                    else
                        players[i]->setPosition(0);
                    players[i]->update();
                }else{
                    players[i]->setPaused(false);
                    players[i]->update();
                }
            }
        
        
        
        videoMutex.unlock();
    }
    
void Cinema::pause(bool b){
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
    
void Cinema::setMixer(int i, float f){ //put in update
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
    
void Cinema::loadMovie(int player, int dbIndex, int i, bool randomPos){
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
            
#ifdef VERBOSE
            cout << "loading " << strdb[dbIndex][i]  << " to slot " << player << endl;
#endif
            
            // do async
            players[player]->stop();
            ofSleepMillis(3); //so that there's no mutex bug
            
            
            players[player]->closeMovie();
            delete players[player];
            
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

    
void Cinema::parseMidi(ofxMidiMessage eventArgs){
}
    
    
void Cinema::randomPosition(float r){
#ifndef NO_SCENE_CUTS
        string path = players[app->parameterMap[selectedPlayer]]->getMoviePath();
        float pos, pos2;
        if(playerScenes.count(path) && playerScenes[path].size()>0){
            int s = (int) (r * (playerScenes[path].size()+1));
            pos = playerScenes[path][s];
            while(playerScenes[path].size()>1 && pos==lastSceneIndex){ // for mandatory random scene jump, not fitted for live cinema !
                s = (int) (ofRandom(1) * (playerScenes[path].size()+1));
                pos = playerScenes[path][s];
            }
            
            pos2 = s<playerScenes[path].size()-1 ? playerScenes[path][s+1] : players[app->parameterMap[selectedPlayer]]->getTotalNumFrames()-1;
            players[app->parameterMap[selectedPlayer]]->setFrame(pos);
//            app->deltaMap[loopLength] = 0;
            app->deltaMap[loopLength] = pos2-pos;
            app->deltaMap[loopStart] = pos;
            lastSceneIndex = pos;
        }else
#endif
        {
//            int frame = r*players[app->parameterMap[selectedPlayer]]->getTotalNumFrames();
//            players[app->parameterMap[selectedPlayer]]->setFrame(frame);
           players[app->parameterMap[selectedPlayer]]->setPosition(r);
        }
        
    }
    
void Cinema::randomScene(string tag){
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

    
void Cinema::keyPressed(int key){
        switch(key){
            case ' ':{
//                dbIndex = ofRandom(strdb.size());
                loadLaterIndex = ofRandom(strdb[dbIndex].size());
                loadMovie(app->parameterMap[selectedPlayer], dbIndex, loadLaterIndex, true);
            }break;
            case '&':
                app->parameterMap[selectedPlayer] = 0;break;
            case 233:
                app->parameterMap[selectedPlayer] = 1;break;
            case 'n':
                {
                    randomPosition(ofRandom(0,1));
                    app->deltaMap[user5] = 1;
                        stillFbo->begin(); //capture transition fbo
                        fbo.draw(0,0);
                        stillFbo->end();
                }
                break;
            case 4352: randomPosition(ofRandom(0,1));
                break;
            default:;
                
        }
    };

void Cinema::loadDirectMovie(string vidPath){
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
        
        if(!found){
            players[0]->closeMovie();
            delete players[0];
            
            players[0] = new ofVideoPlayer;
            try{
                players[0]->loadMovie(vidPath);
            }catch ( const std::exception & e ) {
                cout << e.what() << endl;
            }
            
            if(vidPath.find("sound")!=string::npos){
                players[0]->setVolume(1);
            }else{
                players[0]->setVolume(0);
            }
            
        }
    }
    
void Cinema::load(){
        ofDirectory dir("cinema/");
        dir.listDir();
        vector<string> dirs;
        for(int i=0;i<dir.size();i++){
            string d = dir.getPath(i);
            if(d.find("DB")!=string::npos){
                vector<string> vids;
                vector<ofVideoPlayer*> ps;
                ofDirectory dir2(d);
                dir2.listDir();
                for(int j=0;j<dir2.size();j++){
                    vector<double> scene;
                    vector<string> movs;
                    string vidpath = dir2.getPath(j);
                    string origVidPath = vidpath;
                    if(dir2.getName(j)[0] != '.' &&
                            (vidpath.find(".mov")!=string::npos || vidpath.find(".mp4")!=string::npos ||
                       vidpath.find(".avi")!=string::npos ||
                       vidpath.find(".3gp")!=string::npos
                       || vidpath.find(".gif")!=string::npos
                       || vidpath.find(".webm")!=string::npos
                       || vidpath.find(".mkv")!=string::npos)){
                        vids.push_back(vidpath);
                        playerPaths.push_back(vidpath);
                        ofVideoPlayer* p = new ofVideoPlayer;
                        playerIntensities.push_back(0);
                        ps.push_back(p);
                        
                        string txtPath = vidpath;
                        ofStringReplace(txtPath, ".mov", ".txt");
                        ofStringReplace(txtPath, ".mp4", ".txt");
                        ofStringReplace(txtPath, ".mkv", ".txt");
                        ofStringReplace(txtPath, ".webm", ".txt");
                        ofStringReplace(txtPath, ".avi", ".txt");
                        ofStringReplace(txtPath, ".gif", ".txt");
                        string t = ofBufferFromFile(txtPath).getText();
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
                            ofFile vf(vidpath) ;
                            string absoluteVidPath = vf.getAbsolutePath();
                            playerScenes[absoluteVidPath] = scene;
                            playerScenesMovements[origVidPath] = movs;
                        }
                    }
                   
                }
                strdb.push_back(vids);
            }
        }
    }
    
void Cinema::saveMacro(stringstream *xml){
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
            if(app->saveMacroTC)
                saveParameterXML(xml, ss.str(), players[i]->getPosition());
            else
                saveParameterXML(xml, ss.str(), app->currentTC);
        }
        
        (*xml) << "</cinema>" << endl;
    }
    
void Cinema::loadMacro(TiXmlHandle *xml){
        videoMutex.lock();
        
        stringstream ss;
        for(int i=0;i<3;i++){
            playerIntensities[i] = i==0;
            
            
            ss.str("");
            
            ss << "playerIntensities" << i ;
            
            float intensity = ofToFloat(xml->FirstChild(ss.str()).ToElement()->GetText());
            cout << "intensity : " << i << " " << intensity << endl;
            
            ss.str("");
            ss << "vidPath" << i ;
            auto c = xml->FirstChild(ss.str()).ToElement()->GetText();
            if(c == 0)
                continue;
            string vidPath = c;
            
            ss.str("");
            ss << "vidPosition" << i ;
            float position = ofToFloat(xml->FirstChild(ss.str()).ToElement()->GetText());
            if(i==0)
                app->currentTC = position;
            
            
            playerIntensities[i] = intensity;
            if(intensity>0){
#ifdef VERBOSE
                cout << "loading cinema macro path "  << vidPath << endl;
#endif
                ofFile f1(vidPath);
                ofFile f2(players[i]->getMoviePath());
                if(f1.getFileName().compare(f2.getFileName())){ //check current vid
                    bool found = false;
                    for(int dbIndex=0;dbIndex<strdb.size();dbIndex++){
                        for(int mIndex=0;mIndex<strdb[dbIndex].size();mIndex++){
                            f2 = ofFile(strdb[dbIndex][mIndex]);
                            if(!f2.getFileName().compare(f1.getFileName())){
                                loadLaterMoviePosition[i] = position;
                                loadMovie(i, dbIndex, mIndex, false);
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
    
void Cinema::setPosition(float f){
        players[app->parameterMap[selectedPlayer]]->setPosition(f);
    }
    
std::string Cinema::getInfo(){
        stringstream str;
        str << "Media " << endl
        << "name :     " << strdb[dbIndex][vidIndex] << endl
        << "position : " << players[0]->getPosition() << "(f. " << players[0]->getCurrentFrame() << ")";
        return str.str();;
    }
    
    
void Cinema::setResolution(){
        fbo.allocate(WIDTH, HEIGHT, GL_RGB);
        copyFbo.allocate(WIDTH,HEIGHT, GL_RGB);
    }
    
