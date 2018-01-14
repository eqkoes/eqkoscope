//
//  kinect.h
//  eqkoscope
//
//  Created by Raphaël on 25/12/2015.
//
//

#ifndef eqkoscope_kinect_h
#define eqkoscope_kinect_h



//a mettre dans Kinectscene
if(1==0){
    //            kinectMesh.clear();
    //            int index2 = 0;
    //            for(int i=0;i<10000;i++){
    //                float x = ofRandom(100,-100)*3;
    //                float y = ofRandom(100,-100)*2;
    //                float z = ofRandom(1);
    //                kinectMesh.addColor(ofColor::fromHsb(0, 0, ofRandom(255)));
    //                kinectMesh.addVertex(ofVec3f(x, y, z));
    //            }
    
    int numVerts = kinectMesh.getNumVertices();
    kinectMesh2.clear();
    //        while(kinectMesh2.getNumVertices()>5000)
    //            kinectMesh2.removeVertex(0);
    //        for(int i=0;i<kinectMesh2.getNumVertices();i++){
    //            ofVec3f v = kinectMesh2.getVertex(i);
    //            kinectMesh2.setVertex(i, ofVec3f(v.x, v.y+10, v.z+5));
    //        }
    //        int index2 = 0;
    //        for(int ii=0;ii<numVerts;ii++){
    //            float x = kinectMesh.getVertex(ii).x;
    //            float y = kinectMesh.getVertex(ii).y;
    //            float z = kinectMesh.getVertex(ii).z;
    //            int m = 0;
    //            for(int i=0;i<1;i++){
    //                float zz = z + ofRandom(-m,m);
    //                kinectMesh2.addVertex(ofVec3f(x + ofRandom(-m,m), y + ofRandom(-m,m), zz));
    //                kinectMesh2.addColor(ofColor::fromHsb(0, 0, 100+zz*150));
    //                index2++;
    //            }
    //        }
    
    kinectMesh.clearIndices();
    float mc = 15;
    float Mc = mc + 5;
    kinectMesh.clearColors();
    //            for( int i=0; i < kinectMesh.getVertices().size(); i++ ) kinectMesh.addNormal(ofPoint(0,0,0));
    for (int a=0; a<numVerts-1; ++a) {
        ofVec3f verta = kinectMesh.getVertex(a);
        //                            ofColor col =i.getColor(int(verta.x+320), int(verta.y));
        //                            col.setBrightness(25);
        ofColor col = ofColor::fromHsb(0, 0, ofRandom(255));
        kinectMesh.addColor(col);
        for (int b=a+1; b<numVerts; ++b) {
            ofVec3f vertb = kinectMesh.getVertex(b);
            float distance = verta.distance(vertb);
            if (distance >= mc && distance <= Mc && ofRandom(0.1)<1){
                //                    && verta.y -25 < ofGetMouseY() && verta.y +25 > ofGetMouseY()) {
                kinectMesh.addIndex(a);
                kinectMesh.addIndex(a+1);
                kinectMesh.addIndex(b);
                
                const int ia = a;
                const int ib = a+1;
                const int ic = b;
                
                //                        ofVec3f e1 = kinectMesh.getVertices()[ia] - kinectMesh.getVertices()[ib];
                //                        ofVec3f e2 = kinectMesh.getVertices()[ic] - kinectMesh.getVertices()[ib];
                //                        ofVec3f no = e2.cross( e1 );
                //                        kinectMesh.getNormals()[ia] += no;
                //                        kinectMesh.getNormals()[ib] += no;
                //                        kinectMesh.getNormals()[ic] += no;
            }
        }
    }
    
    //            ofBackground(0);
    ofSetColor(ofColor::white);
    //            ofNoFill();
    //<##>
    ofPushMatrix();
    ofTranslate(-WIDTH/2, HEIGHT/2, -HEIGHT/4); //,new scale
    ofScale(WIDTH/640.0, HEIGHT/480.0, 1);
    ofEnableBlendMode(OF_BLENDMODE_SCREEN);
    kinectMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    //            ofDisableArbTex();
    //            ofEnableNormalizedTexCoords();
    //            glEnable(GL_DEPTH_TEST);
    //            i.getTextureReference().bind();
    kinectMesh.draw();
    //            i.getTextureReference().unbind();
    //            glDisable(GL_DEPTH_TEST);
    //            ofDisableNormalizedTexCoords();
    //            ofEnableArbTex();
    
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    ofPopMatrix();
    
    if(threeDeeScene)
        cam.end();
        
        i.grabScreen(0, 0, WIDTH, HEIGHT);
        
    /** GLOW **/
        if(glow){
            tempFbo.begin();
            threshShader.begin();
            threshShader.setUniform1f("threshold", 0.5);
            threshShader.setUniform1f("brightness", 1);
            threshShader.setUniform1f("saturation", 1);
            ofBackground(0);
            i.draw(0, 0);
            threshShader.end();
            tempFbo.end();
            
            fbo.begin();
            gBlurHor.begin();
            gBlurHor.setUniform1f("vertical", 0);
            gBlurHor.setUniform1f("resolution", glowResolution);
            gBlurHor.setUniform1f("originalMix", 0);
            tempFbo.draw(0,0);
            gBlurHor.end();
            fbo.end();
            
            fbo2.begin();
            gBlurHor.begin();
            gBlurHor.setUniform1f("vertical", 1);
            gBlurHor.setUniform1f("resolution", glowResolution);
            gBlurHor.setUniform1f("originalMix", 0);
            fbo.draw(0,0);
            gBlurHor.end();
            fbo2.end();
            
            ofEnableBlendMode(OF_BLENDMODE_ADD);
            fbo2.draw(0,0, FINALWIDTH, FINALHEIGHT);
            tempFbo.draw(0,0, FINALWIDTH, FINALHEIGHT); //glow rendering
            ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        }else{
            threshShader.begin();
            threshShader.setUniform1f("threshold", 0.5);
            threshShader.setUniform1f("brightness", 1);
            threshShader.setUniform1f("saturation", 1);
            ofBackground(0);
            i.draw(0, 0, FINALWIDTH, FINALHEIGHT);
            threshShader.end();
        }
}

if(kinectMasking){
    /** KINECT MASK PROCESSING UNIT **/
    ofPushMatrix();
    ofBackground(255);
    glPointSize(5);
    kinectMesh.setMode(OF_PRIMITIVE_POINTS);
    ofTranslate(WIDTH/2, HEIGHT/2, -HEIGHT/4);
    ofScale(kinectScale, kinectScale, kinectScale);
    ofSetColor(ofColor::black);
    ofNoFill();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    kinectMesh.draw();
    glPointSize(1);
    kinectMask.grabScreen(0, 0, WIDTH, HEIGHT);
    ofPopMatrix();
}

#endif
