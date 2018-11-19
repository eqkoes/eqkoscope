#include "UX278.h"

void UX278::setup(){
    ofApp::setup();
    feedback->shapeNbPts = 3;
    feedback->scale = 0.9;
    feedback->dScale = feedback->scale;
    parameterMap["kalei"] = 0;
    parameterMap["kaleiNb"] = 2;
    parameterMap["chromaSep"] = 0.;
    parameterMap["chromasepHue"] = 180;
    parameterMap["chromaOffset"] = 25;
    //setTintSaturation(0.5);
    feedback->updateRandomColor();
    parameterMap["chromaOffset"] = 0.1;
    parameterMap["glow"] = true;
    parameterMap["ball"] = false;//TEST
    kinectMesh = ofMesh::sphere(HEIGHT/2.5, 40);
    parameterMap["omg3D2"] = true;
    parameterMap["omg3D"] = false;
    parameterMap["xpixellate"] = 50;
    parameterMap["ypixellate"] = 50;
    parameterMap["sobel"] = false;
    parameterMap["post_traitement"] = true;
    thresholdAfterBall = false;
    feedback->backMask = false;
    }

void UX278::update(){
    
    //feedback->modShape();
    
    while(receiver.hasWaitingMessages()){
        ofxOscMessage m;
        receiver.getNextMessage(&m);
        
        /*if(m.getAddress()== "/Audio/Onset2"){
            skewAmp += 0.1;
         continue;
        }*/
//        if(m.getAddress()== "/Audio/RMS1"){
//            float rms = m.getArgAsFloat(0);
//            //chromaSep = ofMap(rms*100, 0, 1, 0, 0.3, true);
//            feedback->dScale = ofMap(rms*100, 0, 1, 1, 0.2, true);
//            feedback->scale = feedback->dScale;
//         continue;
//        }
        
        /*if(m.getAddress()== "/Audio/Centroid"){
            float centroid = m.getArgAsFloat(0);
            tintHue += centroid/1000000.0;
            if(centroid > 5000){
                feedback->shapeNbPts = int(ofRandom(2, 5));
                //cout << "centroid.........." << centroid << endl;
            }
         continue;
        }*/
        
        
        if(m.getAddress()== "/Kinect/HiArms"){
             hiArms = m.getArgAsFloat(0);
            //chromaSepDelta = hi > 0.5 ? 0.15 : 0;
            if(hiArms>0.5){
                feedback->upRot = 0;
                feedback->rot = 0;
            }else{
                feedback->upRot = ofMap(presence, 0, MAX_PRESENCE, 0.001, 1, true);
                feedback->rot = ofMap(presence, 0, MAX_PRESENCE, 0.001, 1, true);

            }
         continue;
        }
        
        if(m.getAddress()== "/Kinect/Movement"){
            float move = m.getArgAsFloat(0);
            if(move>0.0005)
                movementRatio += move;
            cout << "movement...." << movementRatio << endl;
            if(movementRatio>0.15){
                feedback->shapeNbPts += ofRandom(-1,1);
                feedback->shapeNbPts = ofClamp(feedback->shapeNbPts, 2, 25);
            }
         continue;
        }
        
        xExtent = 0;
        if(m.getAddress()=="/Kinect"){
            kinectMesh.clear();
            
            float N = m.getNumArgs()/3;
            meanZ = 0;
            float my = 0, mx = 0;
            for(int i=0;i<N;i++){
                float x = 640-m.getArgAsFloat(i*3);
                float y = m.getArgAsFloat(i*3+1);
                float z = m.getArgAsFloat(i*3+2);
                ofVec3f v(x, y, z);
                kinectMesh.addVertex(v);
                kinectMesh.addColor(ofColor::fromHsb(0, 0, ofRandom(255)));
                my += y;
                mx += x;
                meanZ += z;
            }
            
            mx = N>0 ? mx/N : 0;
            if(lastMeanX - mx > meanX)
                meanX = lastMeanX - mx;
            else
                meanX += (lastMeanX - mx - meanX)*0.25 ;
            lastMeanX = mx;
            
            
            my = N>0 ? my/N : 0;
            if(lastMeanY - my > meanY)
                meanY = lastMeanY - my;
            else
                meanY += (lastMeanY - my - meanY)*0.25 ;
            lastMeanY = my;
            
            meanZ = N>0 ? -(meanZ/N)-1000 : 0;
            
            presence = N/3;
            continue;
        }
    }
    
    ofApp::update();
    
    //meanY = mouseY-HEIGHT/2;
    /*presence = mouseX/2;
    presence = ofClamp(presence, 0, MAX_PRESENCE);
    meanZ = mouseY;*/
    //cout << "presence.........." << presence << endl;
    
    oscout("/Presence", ofMap(presence, 0, MAX_PRESENCE, 0, 1, true));

    
    // TRACKING
    if(abs(meanX)>MIN_MEANX && presence > MIN_PRESENCE)
        feedback->doffx += meanX/10.0;
    if(abs(meanY)>MIN_MEANY && presence > MIN_PRESENCE)
        feedback->doffy += meanY/10.0;
    
    //SCALE
        feedback->dScale = ofMap(meanZ, 0, MAX_MEANZ, 0.95, 0.5, true);
        feedback->scale = feedback->dScale;
        feedback->shapeWeight = ofMap(meanZ, 0, MAX_MEANZ, 0.5, 2, true);

    //mod shape
    feedback->modShape(ofMap(meanZ, MAX_MEANZ/2, MAX_MEANZ, 0, 75, true));
    
    //STROBE : recompense
   // feedback->strobe = presence > MAX_PRESENCE/2 && meanZ < 300;
    
    skewAmp *= 0.5;
    //chromaSep*=0.99;
    
    movementRatio *= 0.7;
    
    /** RESPIRATION **/
    breaths += ofMap(presence, 0, MAX_PRESENCE/6, 0.04, 0.15, true);
    respiration = std::cos(breaths);
    
    glowIntensity = ofMap(respiration, -1, 1, 0, 1, true);
    
    float breathAmp = 0.75;
    deltaMap["tintBrightness"] = ofMap(presence, 0, MAX_PRESENCE/4, 0.1, 1+breathAmp, true);
    deltaMap["tintBrightness"] += ofMap(respiration, -1, 1, -breathAmp, breathAmp, true);
    deltaMap["tintBrightness"] = ofClamp(deltaMap["tintBrightness"], 0, 1);
    tintBrightness = deltaMap["tintBrightness"];
    
    /** PIXELLATE **/
    xpixellate = ofMap(presence, 0, MAX_PRESENCE/2, 50, 0, true);
    ypixellate = ofMap(presence, 0, MAX_PRESENCE, 50, 0, true);
    
    tintSaturation = (ofMap(presence,0, MAX_PRESENCE, 0, 1, true));
    
}

void UX278::draw(){
    ofApp::draw();
    stringstream s;
    s << "meanX " << meanX << endl <<
     "hiArms " << hiArms << endl <<
    "meanY " << meanY << endl <<
    "precence " << presence << endl
    << "meanZ " << meanZ << endl;;
    ofDrawBitmapString(s.str(), 100,400);
    
    if(presence > MAX_PRESENCE/4)
    analyzeAudio();
}

void UX278::analyzeAudio(){
    for(int x=0;x<WIDTH;x+=25){
        bool line = true;
        for(int y=HEIGHT/8;y<7*HEIGHT/8;y++){
            if(i.getColor(x,y).getBrightness()<200){
                line = false;
                break;
            }
        }
        if(line){
            oscout("/NoteOn", int(ofRandom(70,98)));
        }
    }
    
    for(int rad=0;rad<HEIGHT-10;rad+=25){
        bool circle = true;
        for(float a=0;a<2*PI;a+=2*PI/5.0){
            float x = WIDTH/2+cos(a)*rad;
            float y = HEIGHT/2+sin(a)*rad;
            if(i.getColor(x,y).getBrightness()<150){
                circle = false;
                break;
            }
        }
        if(circle){
            oscout("/NoteOn", int(ofRandom(36,39)));
        }
    }
}
