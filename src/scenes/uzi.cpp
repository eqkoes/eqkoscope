//
//  uzi.h
//  emptyExample
//
//  Created by Raphaël on 05/11/2014.
//
//

#include "uzi.h"

static bool isImgPath(string picpath){
    return  (picpath.find(".jpg")!=string::npos ||picpath.find(".jp2")!=string::npos || picpath.find(".png")!=string::npos || picpath.find(".bmp")!=string::npos || picpath.find(".gif")!=string::npos||picpath.find(".JPG")!=string::npos || picpath.find(".PNG")!=string::npos || picpath.find(".GIF")!=string::npos
    );
}

Uzi::Uzi(AbstractApp* app) : Scene(app, "uzi"){
    sceneID = "uzi";
    
    source = new ofFbo();
    source->allocate(WIDTH,HEIGHT, GL_RGBA);
    
    circleMask.loadImage("assets/maskCircle1280x720.png");
    circleMask.resize(WIDTH,HEIGHT);
    
    load();
    image = new ofImage;
    rawImage = new ofImage;
    image->allocate(WIDTH,HEIGHT, OF_IMAGE_COLOR);
    loadImage(strdb[0][0]);
    setDirty();
    
    swapTintShader.load("../shaders/swapTint");
    depthShader.load("../shaders/depthMap");
    blendShader.load("../shaders/blend");
}


void Uzi::draw(){
    if(image->getWidth()==0 || image->getHeight()==0)
        return;
    
    int w = (!app->parameterMap[resize] || image->getWidth() > image->getHeight()) ? WIDTH : image->getWidth()*HEIGHT/image->getHeight();
    int h = (!app->parameterMap[resize] || image->getWidth() < image->getHeight()) ? HEIGHT : image->getHeight()*WIDTH/image->getWidth();
    
    
    if(1==1){
        
        ofPushMatrix();
        ofTranslate(WIDTH/2, HEIGHT/2);
        
        if(app->parameterMap[randomUzi]>1){
            int p = (int)max(1.0f, app->parameterMap[uziPeriod]);
            ofTranslate(0,0, app->parameterMap[randomUzi]*(ofGetFrameNum()%p / float(p)));
        }
        
        if(app->parameterMap[vMirror])
            ofScale(1,-1);
        if(app->parameterMap[hMirror])
            ofScale(-1,1);
        
        ofBackground(0);
        
        if(currentMesh!=0){
            
        }else{
            if(app->parameterMap[resize])
            {
                ofPushMatrix();
                ofScale(1+app->parameterMap[mediaZ], 1+app->parameterMap[mediaZ]);
                ofTranslate(app->parameterMap[mediaX]*rawImage->getWidth(),
                            app->parameterMap[mediaY]*rawImage->getHeight()/2);
                ofRotateX(app->parameterMap[mediaRotX]);
                ofRotateY(app->parameterMap[mediaRotY]);
                ofRotateZ(app->parameterMap[mediaRotZ]);
                image->draw(-w/2, -h/2, w, h);
                ofPopMatrix();
            }else{
                ofPushMatrix();
                ofScale(1+app->parameterMap[mediaZ], 1+app->parameterMap[mediaZ]);
                ofTranslate(app->parameterMap[mediaX]*rawImage->getWidth(),
                            app->parameterMap[mediaY]*rawImage->getHeight()/2);
                ofRotateX(app->parameterMap[mediaRotX]);
                ofRotateY(app->parameterMap[mediaRotY]);
                ofRotateZ(app->parameterMap[mediaRotZ]);
                rawImage->draw(-rawImage->getWidth()/2,
                               -rawImage->getHeight()/2);
                ofPopMatrix();
            }
        }
        
        ofPopMatrix();
        
        return;
    }
    
    
    if(imgDirty || meshdb.size()>0){
        
        source->begin();
        if(! app->parameterMap[transluscentUzi] ||
           ofGetFrameNum() % int(app->parameterMap[transluscentUzi]) == 0)
            ofBackground(ofColor(0,0,0));
        
        if(app->parameterMap[ak47Mode] && !app->parameterMap[ak47Frame]){
        }else{
            
            app->deltaMap[ak47Frame] = app->parameterMap[ak47Frame] = false;
            
            ofPushMatrix();
            ofTranslate(WIDTH/2, HEIGHT/2);
            
            if(app->parameterMap[randomUzi]>1){
                int p = (int)max(1.0f, app->parameterMap[uziPeriod]);
                ofTranslate(0,0, app->parameterMap[randomUzi]*(ofGetFrameNum()%p / float(p)));
            }
            
            if(app->parameterMap[vMirror])
                ofScale(1,-1);
            if(app->parameterMap[hMirror])
                ofScale(-1,1);
            
            if(app->parameterMap[autoRot]>0)
                ofRotate(45*app->parameterMap[autoRot]*sin(app->parameterMap[autoRotFreq]*ofGetFrameNum()/15.0));
            
            /* if(app->parameterMap[user1]){
             //                        ofTranslate(ofRandom(-0.5,0.5)*HEIGHT, ofRandom(-0.5,0.5)*HEIGHT);
             //
             //                        float ssss = ofRandom(0.1,0.3);
             //                        ofScale(ssss, ssss);
             
             blendShader.begin();
             blendShader.setUniform1i("mode", 0);
             blendShader.setUniform1i("thresh", 0.5);
             
             int nb = 10;
             if(zs.size()!=nb){
             for(int x=0; x<10;x++){
             zs.push_back(ofVec4f(0,0, x/(float)10,0));
             }
             }
             
             for(int x=0; x<10;x++){
             zs[x].z += app->parameterMap[omg3D2Speed];
             zs[x].z = fmod(zs[x].z, 1);
             }
             
             for (int index=0; index<zs.size();index++){
             ofPushMatrix();
             
             float s = zs[index].z;
             ofScale(s,s);
             
             int dbb = ofRandom(0, strdb[dbIndex].size());
             dbb = index % strdb[dbIndex].size();
             tripImg.loadImage(strdb[dbIndex][dbb]);
             
             tripImg.draw(-w/2, -h/2, w, h);
             
             ofPopMatrix();
             }
             
             blendShader.end();
             }else{*/
            
            
            
            if(currentMesh!=0){
               /* currentMesh->setMode(OF_PRIMITIVE_TRIANGLES);
                ofScale(HEIGHT/40, HEIGHT/40);
                
                ofTranslate(app->parameterMap[mediaX]*WIDTH, -app->parameterMap[mediaY]*HEIGHT, app->parameterMap[mediaZ]*HEIGHT);
                ofRotateX(app->parameterMap[mediaRotX]);
                ofRotateY(app->parameterMap[mediaRotY]);
                ofRotateZ(app->parameterMap[mediaRotZ]);
                
                ofBackground(0);
                ofSetColor(ofColor::white);
                
                ofNoFill();
                ofEnableBlendMode(OF_BLENDMODE_ALPHA);
                
                ofDisableArbTex();
                ofEnableNormalizedTexCoords();
                glEnable(GL_DEPTH_TEST);
                
                if(currentTexture != NULL)
                    currentTexture->bind();
                currentMesh->draw();
                if(currentTexture != NULL)
                    currentTexture->unbind();
                
                //                       currentMesh->drawWireframe();
                
                glDisable(GL_DEPTH_TEST);
                ofDisableNormalizedTexCoords();
                ofEnableArbTex();*/
            }else{
                if(app->parameterMap[resize])
                {
                    image->draw(-w/2, -h/2, w, h);
                }else{
                    //image->draw(-image->width/2, -image->height/2);
                    ofPushMatrix();
                    ofScale(1+app->parameterMap[mediaZ], 1+app->parameterMap[mediaZ]);
                    ofTranslate(app->parameterMap[mediaX]*rawImage->getWidth(),
                                app->parameterMap[mediaY]*rawImage->getHeight()/2);
                    ofRotateX(app->parameterMap[mediaRotX]);
                    ofRotateY(app->parameterMap[mediaRotY]);
                    ofRotateZ(app->parameterMap[mediaRotZ]);
                    rawImage->draw(-rawImage->getWidth()/2,
                                   -rawImage->getHeight()/2);
                    ofPopMatrix();
                }
            }
            
            ofPopMatrix();
            
        }
        source->end();
        
        imgDirty = false;
    }
    
    ofTranslate(0, (HEIGHT2-HEIGHT)/2);
    
    imgMutex.unlock();
    
    ofTranslate(WIDTH/2, HEIGHT/2);
    
    if(currentMesh==0){
        if(app->parameterMap[resize]){
            ofTranslate(app->parameterMap[mediaX]*WIDTH, -app->parameterMap[mediaY]*HEIGHT, app->parameterMap[mediaZ]*HEIGHT);
            
            ofRotateX(app->parameterMap[mediaRotX]);
            ofRotateY(app->parameterMap[mediaRotY]);
            ofRotateZ(app->parameterMap[mediaRotZ]);
        }
    }
    
    ofScale(app->parameterMap[mediaScaleX], app->parameterMap[mediaScaleY]);
    
    ofBackground(0,0,0);
    
    if(depthMap.isAllocated() ){
        //            LOAD_ASYNC depthShader.load("../shaders/depthMap");
        depthShader.begin();
        depthShader.setUniformTexture("depthMap", depthMap.getTextureReference(), 1);
        depthShader.setUniform1f("intensity", fmod(app->deltaMap[pertEvo] , 1));
        depthShader.setUniform1f("mode", 2);
        depthShader.setUniform1i("WIDTH", WIDTH);
        depthShader.setUniform1i("HEIGHT", HEIGHT);
    }
    source->draw(-WIDTH/2,(HEIGHT2 - HEIGHT)/2 - HEIGHT/2);
    if(depthMap.isAllocated() ){
        depthShader.end();
    }
}

void Uzi::loadImage(string path){
    cout << "loading img " << path << "...";
    if(strEndsWith(path, ".obj")){
        currentMesh = &meshdb[path];
        currentTexture = &texturedb[currentMesh];
    }else{
        image->clear();
        image->loadImage(path);
        cout << "ok" << endl;
        
        rawImage->clear();
        rawImage->loadImage(path);
        currentMedia = path;
        
        if(preload && strStartsWith(path, "preload_")){
            preloadedImages.clear();
            ofDirectory dir(ofFile(path).getEnclosingDirectory());
            dir.listDir();
            for(int i=0;i<dir.size();i++){
                if(isImgPath(dir.getFile(i).getAbsolutePath())){
                    ofImage img(dir.getFile(i).getAbsolutePath());
                    preloadedImages[dir.getFile(i).getAbsolutePath()] = img;
                }
            }
        }
        
        string ext = path.substr(currentMedia.length()-4, 4);
        string mapStr = currentMedia.substr(0, currentMedia.length()-4).append("_map").append(ext);
        if(ofFile(mapStr).exists()){
            depthMap.loadImage(mapStr);
            depthMap.update();
        }else{
            if(depthMap.isAllocated())
                depthMap.clear();
        }
    }
    
//    app->parameterMap[mediaX] = app->parameterMap[mediaY]
//    =  app->deltaMap[mediaX] = app->deltaMap[mediaY] = 0;
    
    setDirty();
}

void Uzi::update(){
    
    /** AUTO LOAD MEDIA FROM CLIPBOARD **/
    /*string cr = ofSystem("pbpaste");
     if ( strEndsWith(cr, "\xff"))
     cr = cr.substr(0, cr.length()-1);
     if(clipboardRawContents.compare(cr)){
     clipboardRawContents = cr;
     
     string clipboardContents = ofSystem("find ./../../../data/uzi -name "+clipboardRawContents);
     
     if(clipboardContents.size()>10){
     if ( strEndsWith(clipboardContents, "\xff"))
     clipboardContents = clipboardContents.substr(0, clipboardContents.length()-1);
     int pos = clipboardContents.find("\n");
     if ( pos != string::npos )
     clipboardContents = clipboardContents.substr(0, pos);
     ofFile clipFile(clipboardContents);
     if(clipFile.exists()){
     loadImage(clipboardContents);
     }
     }
     }*/
    
//    if(app->parameterMap[jpgGlitch]){
//        string ext = ofToLower(ofFile(strdb[dbIndex][imgIndex]).getExtension());
//        string path = "./tmp/glitch." + ext;
//        glitchImage(path, strdb[dbIndex][imgIndex]);
//        loadImage(path);
//    }
    
    if(app->deltaMap[randomUzi]){
        
        if (ofGetFrameNum()%((int)max(1.0f, app->parameterMap[uziPeriod])) == 0){
            app->parameterMap[switchImg] =app->deltaMap[switchImg] = 1;
        }
    }
    
    if (dbIndex >= strdb.size()) //sanity check
        dbIndex = strdb.size() - 1;
    if (imgIndex >= strdb.at(dbIndex).size())
        imgIndex = strdb.at(dbIndex).size() - 1;
    
    
    if (app->deltaMap[switchImg]) {
        if(ofGetFrameNum() - lastSwitchFrame < lastSwitchLimit){
            app->parameterMap[switchImg] = app->deltaMap[switchImg] = 0;
            return;
        }
        
#ifdef VERBOSE
        cout << "load " << strdb[dbIndex][imgIndex] << endl;
#endif
        //            dbIndex = strdb.size()-1; //test
        if(strdb[dbIndex].size()>1){
            int lastIndex = imgIndex;
            do{
                imgIndex = (int)ofRandom(strdb[dbIndex].size());
            }while(lastIndex==imgIndex);
        }
        lastSwitchFrame = ofGetFrameNum();
        
        scheduleLoad = true;
        
        app->parameterMap[_mask] = strdb[dbIndex][imgIndex].find("_round_")!=string::npos;
        app->parameterMap[switchImg] = app->deltaMap[switchImg] = false;
    }
    
    if(scheduleLoad){
        //here
        if(preloadedImages.count(strdb[dbIndex][imgIndex])){
           loadFromPreloadedImgs();

        }else{
            loadImage(strdb[dbIndex][imgIndex]);
        }
        scheduleLoad = false;
    }
    
    if((app->parameterMap[autoRot]>0 || app->parameterMap[ak47Mode]))
        setDirty();
}

void Uzi::loadFromPreloadedImgs(){
    image->clear();
    rawImage->clear();
#ifdef OF_10
    image->setFromPixels(preloadedImages[strdb[dbIndex][imgIndex]].getPixels());
    image->setFromPixels(preloadedImages[strdb[dbIndex][imgIndex]].getPixels());
#else
    image->clone(preloadedImages[strdb[dbIndex][imgIndex]]);
    rawImage->clone(preloadedImages[strdb[dbIndex][imgIndex]]);
#endif
    currentMedia = strdb[dbIndex][imgIndex];
}

void Uzi::midiEvent(ofxMidiMessage& eventArgs){
    float value = eventArgs.value;
    switch(eventArgs.channel){
        case 1:{
            if(eventArgs.status==MIDI_CONTROL_CHANGE){
                switch(eventArgs.control){
                    case 105:{
                        if(eventArgs.value>0)
                            manualSwitch();
                    }break;
                        //            case 106:{
                        //                uziSwitch();
                        //            if(eventArgs.value==0)
                        //                app->deltaMap[randomUzi] = false;
                        //            }break;
                        //                        case 107:app->deltaMap[randomUzi] = true;break;
                }
            }break;
        }
        case 3: case 4:{
            switch(eventArgs.status){
                case MIDI_CONTROL_CHANGE:{
                    switch(eventArgs.control){
                        case 9:{
                            app->deltaMap[uziPeriod] = app->parameterMap[uziPeriod] = 1 + (int) (30*value/127);
                        }break;
                        case 16:{
                            app->parameterMap[autoRot] = (value/127.0);
                        }break;
                        case 17:{
                            app->parameterMap[autoRotFreq] = 5*(value/127.0);
                        }break;
                        case 47:
                            if(value==127)
                                app->parameterMap[ak47Mode] = !app->parameterMap[ak47Mode];
                            break;
                    }
                }break;
                    
                case MIDI_NOTE_ON:{
                    switch(eventArgs.pitch){
                        case 44:
                            app->deltaMap[randomUzi] = !app->deltaMap[randomUzi];
                            if(app->parameterMap[ak47Mode])
                                app->deltaMap[ak47Frame] = app->deltaMap[randomUzi];
                            break;
                        case 48:
                            uziSwitch(); break;
                        case 49:{
                            manualSwitch(); break;

                        default:;

                        }
                    }break;
                case MIDI_NOTE_OFF:{
                    switch(eventArgs.pitch){
                        case 48:
                            app->deltaMap[randomUzi] = false;
                            app->deltaMap[ak47Frame] = false;
                            break;
                        case 49:
                            app->deltaMap[randomUzi] = false;
                            app->deltaMap[ak47Frame] = false;
                            break;
                        default:
                            if (eventArgs.pitch >=36 && eventArgs.pitch <48) {
                                if(app->parameterMap[ak47Mode]){
                                    int nb = eventArgs.pitch - 36;
                                    if(nb > strdb.size()-1)
                                        nb = strdb.size()-1;
                                    if(app->parameterMap[lastAk47Index]==nb && !app->parameterMap[randomUzi])
                                        app->deltaMap[ak47Frame] = false;
                                };
                            }
                    }
                }break;
                default:;
                }
            }break;
        default:;
        }
    }
    
}

void Uzi::uziSwitch(){
    app->deltaMap[switchImg] = true;
    app->deltaMap[randomUzi] = true;
    if(app->parameterMap[ak47Mode])
        app->deltaMap[ak47Frame] = true;
}

void Uzi::manualSwitch(){
    int t = imgIndex;
    do {
        imgIndex = int(ofRandom(strdb.at(dbIndex).size()));
    }
    while (imgIndex == t && strdb.at(dbIndex).size()>1);
    
    if(preloadedImages.count(strdb[dbIndex][imgIndex])){
       loadFromPreloadedImgs();
        
    }else{
        loadImage(strdb[dbIndex][imgIndex]);
    }
}

void Uzi::manualNext(){
    imgIndex = (imgIndex+1) % strdb.at(dbIndex).size();
    
    if(preloadedImages.count(strdb[dbIndex][imgIndex])){
       loadFromPreloadedImgs();
        
    }else{
        loadImage(strdb[dbIndex][imgIndex]);
    }
}



void Uzi::load(){
    ofDirectory dir("uzi/");
    dir.listDir();
    vector<string> dirs;
    for(int i=0;i<dir.size();i++){
        string d = dir.getPath(i);
        if(d.find("DB")!=string::npos){
            vector<string> strs;
            vector<ofImage*> imgs;
            ofDirectory dir2(d);
            dir2.listDir();
            for(int j=0;j<dir2.size();j++){
                string picpath = dir2.getAbsolutePath() + "/" + dir2.getName(j);
                if(isImgPath(picpath) && dir2.getName(j)[0]!='.'){
                    strs.push_back(picpath);
                }else{
                    if(picpath.find(".obj")!=string::npos){
                        loadMesh(picpath);//put here obj code
                        strs.push_back(picpath);
                    }
                }
            }
            strdb.push_back(strs);
            imgdb.push_back(imgs);
        }
    }
    
//    depth.loadImage("./uzi/dali.jpg");
    
}



void Uzi::loadDirectImage(string path){
    
    loadImage(path);
    rawImage->loadImage(path);
    if(rawImage->getWidth() > WIDTH)
        app->deltaMap[resize] = 0;
    
    currentMedia = path;
    imgDirty = true;
    
    bool found = false;
    for(int db=0;db<strdb.size();db++){
        for(int i=0;i<strdb[db].size();i++){
            ofFile f1(path);
            ofFile f2(strdb[db][i]);
            if(!f1.getFileName().compare(f2.getFileName())){
                //                    app->parameterMap[switchImg] = true;
                dbIndex = db;
                imgIndex = i;
                found = true;
                break;
            }
        }
        if(found)
            break;
    }
}

void Uzi::keyPressed(int key){
    switch(key){
        case ' ':{
//            app->parameterMap[switchImg] = app->deltaMap[switchImg] =  1;
//            app->deltaMap[ak47Frame] = true;
        manualNext();
        }
            break;

    }
    
}


void Uzi::saveMacro(stringstream *xml){
    (*xml) << "<uzi>" << endl;
    
    saveParameterXML(xml, "imgpath", strdb[dbIndex][imgIndex]);
    
    (*xml) << "</uzi>" << endl;
}

void Uzi::loadMacro(TiXmlHandle *xml){
    
    string path = xml->FirstChild("imgpath").ToElement()->GetText();
    bool found = false;
    for(int db=0;db<strdb.size();db++){
        for(int i=0;i<strdb[db].size();i++){
            ofFile f1(path);
            ofFile f2(strdb[db][i]);
            if(!f1.getFileName().compare(f2.getFileName())){
                dbIndex = db;
                imgIndex = i;
                found = true;
                currentMedia = path;
                scheduleLoad = true;
                break;
            }
        }
        if(found)
            break;
    }
}


void Uzi::loadMesh(string path){
    ofMesh mesh;
    
    string t = ofBufferFromFile(path).getText();
    vector<string > v1,v2,v3;
    vector<ofVec3f > vertices, normals, textures;
    if(t.compare("")){
        vector<string> lines = ofSplitString(t, "\n");
        for(string line: lines){
            if(!line.substr(0,2).compare("v ")){
                vector<string > fields = ofSplitString(line, " ");
                if(fields.size()<4)
                    continue;
                vertices.push_back(ofVec3f(ofToFloat(fields[1]), ofToFloat(fields[2]), ofToFloat(fields[3])));
                continue;
            }
            if(!line.substr(0,2).compare("vn")){
                vector<string > fields = ofSplitString(line, " ");
                if(fields.size()<4)
                    continue;
                normals.push_back(ofVec3f(ofToFloat(fields[1]), ofToFloat(fields[2]), ofToFloat(fields[3])));
                continue;
            }
            if(!line.substr(0,2).compare("vt")){
                vector<string > fields = ofSplitString(line, " ");
                if(fields.size()<3)
                    continue;
                textures.push_back(ofVec2f(ofToFloat(fields[1]), ofToFloat(fields[2])));
                continue;
            }
            if(!line.substr(0,2).compare("f ")){
                vector<string > fields = ofSplitString(line, " ");
                if(fields.size()<4)
                    continue;
                v1 = ofSplitString(fields[1], "/");
                v2 = ofSplitString(fields[2], "/");
                v3 = ofSplitString(fields[3], "/");
                if(v1.size()==3 && v2.size()==3 && v3.size()==3){
                    ofMeshFace f;
                    f.setVertex(0, ofVec3f(ofToFloat(v1[0]), ofToFloat(v2[0]),ofToFloat(v3[0])));
                    //                        f.setTexCoord(0,
                    //                                      ofVec3f(ofToFloat(v1[1]), ofToFloat(v2[1]),ofToFloat(v3[1]))
                    //                                      );
                    f.setNormal(0, ofVec3f(ofToFloat(v1[2]), ofToFloat(v2[2]),ofToFloat(v2[2])));
                } else{
                    if(v1.size()==2 && v2.size()==2 && !v3.empty()){
                        mesh.addVertex(vertices[ofToInt(v1[0])-1]);
                        //                            mesh.addTexCoord(textures[ofToInt(v1[1])-1]);
                        
                        mesh.addVertex(vertices[ofToInt(v2[0])-1]);
                        //                            mesh.addTexCoord(textures[ofToInt(v2[1])-1]);
                        
                        mesh.addVertex(vertices[ofToInt(v3[0])-1]);
                        //                            mesh.addTexCoord(textures[ofToInt(v3[1])-1]);
                    }
                }
                continue;
            }
        }
    }
    
    meshdb[path] = mesh;
    
    string str = path;
    ofStringReplace(str, ".obj", ".png");
    ofImage tex;
    if(ofFile(str).exists()){
        tex.loadImage(str);
    }else{
        ofStringReplace(str, ".png", ".jpg");
        if(ofFile(str).exists()){
            tex.loadImage(str);
        }else{
            tex.loadImage("uzi/zzzzDB/default_texture.png");
        }
    }
    texturedb[&meshdb[path]] = tex;
}

std::string Uzi::getInfo(){
    stringstream str;
    str << "Media " << endl
    << "name :     " << strdb[dbIndex][imgIndex] << endl;
    return str.str();
}



