////
////  distortion.cpp
////  eqkoscope
////
////  Created by Raphaël on 17/09/2018.
////
////
//
//#include <distortion.h>
//
//void FX::tunnelMeshDistort(ofFbo* src, ofFbo* dest){
//    int n = app->parameterMap[omg3D2Nb];
//    float r2 = src->getHeight()/2 * 0.9;
//    float r1 = src->getHeight()/2 * 0.5;
//    
//    tunnelMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
//    float offset = ofGetFrameNum() / 100.0;
//    float res = 200;
//    //    float res2 = 10;
//    
//    for(int i=0;i<tunnelMesh.getVertices().size();i++){
//        ofVec3f v = tunnelMesh.getVertex(i);
//        v.z += HEIGHT * app->parameterMap[omg3D2Speed];
//        if(v.z > 1 * src->getHeight()){
//            v.z -= 8 * HEIGHT/2;
//        }
//        tunnelMesh.setVertex(i, v);
//    }
//    
//    
//    res = n*3;
//    r2 = HEIGHT/2 * 0.02;
//    r1 = HEIGHT/2 * 0.02;
//    tunnelMesh2.clear();
//    for(int i=0;i<res;i++){
//        //        for(int j=0;j<res2;j++){
//        float z = 1.5*HEIGHT - 15 * HEIGHT/2 * i / res + std::fmod(offset* 100000 * app->parameterMap[omg3D2Speed], (src->getHeight()/2));
//        float z2 = 1.5*HEIGHT - 15 * src->getHeight()/2 * (i+res/n) / res + std::fmod(offset*500, (src->getHeight()/2));
//        float x = cos( n *2*M_PI*i/res + offset);
//        float y = sin( n *2*M_PI*i/res + offset);
//        tunnelMesh2.addVertex(ofVec3f(WIDTH/2 + r2*x + 0*(ofNoise(z*10, 1) - 0.5),
//                                      src->getHeight()/2 + r2*y + 0*(ofNoise(1, z*10) - 0.5),
//                                      z * app->parameterMap[omg3D2Dist]));
//        tunnelMesh2.addVertex(ofVec3f(WIDTH/2 + r1*x + 0*(ofNoise(z2*10, 1) - 0.5),
//                                      src->getHeight()/2  + r1*y + 0*(ofNoise(1, z2*10) - 0.5),
//                                      z2 * app->parameterMap[omg3D2Dist]
//                                      ));
//        tunnelMesh2.addTexCoord(ofVec2f( int(WIDTH * n*i/(res) * (360+app->parameterMap[omg3D2Rotation])/360.0 ) % WIDTH, src->getHeight()));
//        tunnelMesh2.addTexCoord(ofVec2f( int(WIDTH * n*i/(res) * (360+app->parameterMap[omg3D2Rotation])/360.0 ) % WIDTH, 0));
//        //    }
//    }
//    
//    ofPushMatrix();
//    ofRotateZ(app->parameterMap[rot]);
//    ofTranslate(WIDTH/2, HEIGHT/2);
//    dest->begin();
//    ofBackground(0);
//    ofRotateX(15*(ofNoise(1, ofGetFrameNum()/250.0*(1+app->parameterMap[user1]))-0.5));
//    ofRotateY(15*(ofNoise(ofGetFrameNum()/250.0*(1+app->parameterMap[user1]), 1)-0.5));
//    src->getTextureReference().bind();
//    tunnelMesh.draw();
//    tunnelMesh2.draw();
//    src->getTextureReference().unbind();
//    dest->end();
//    ofPopMatrix();
//}
//
//void FX::initTunnel(){
//    tunnelMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
//    tunnelMesh2.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
//    tunnelMesh.clear();
//    int n = app->parameterMap[omg3D2Nb];
//    float r2 = HEIGHT/2 * 0.9;
//    float r1 = HEIGHT/2 * 0.5;
//    tunnelMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
//    tunnelMesh.clear();
//    //        tunnelMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
//    float offset = ofGetFrameNum() / 100.0;
//    float res = 200;
//    for(int i=0;i<res;i++){
//        float z = HEIGHT - 15 * HEIGHT/2 * i / res + std::fmod(offset* 100000 * app->parameterMap[omg3D2Speed], (HEIGHT/2));
//        float z2 = HEIGHT - 15 * HEIGHT/2 * (i+res/n) / res + std::fmod(offset*500, (HEIGHT/2));
//        float x = cos( n *2*M_PI*i/res + offset);
//        float y = sin( n *2*M_PI*i/res + offset);
//        tunnelMesh.addVertex(ofVec3f(WIDTH/2 + r2*x, HEIGHT/2 + r2*y, z * app->parameterMap[omg3D2Dist]));
//        tunnelMesh.addVertex(ofVec3f(WIDTH/2 + r1*x, HEIGHT/2  + r1*y, z * app->parameterMap[omg3D2Dist]));
//        tunnelMesh.addVertex(ofVec3f(WIDTH/2 + r1*x, HEIGHT/2  + r1*y, z2 * app->parameterMap[omg3D2Dist]));
//        tunnelMesh.addTexCoord(ofVec2f( int(WIDTH * n*i/(res) * (360+app->parameterMap[omg3D2Rotation])/360.0 ) % WIDTH, HEIGHT));
//        tunnelMesh.addTexCoord(ofVec2f( int(WIDTH * n*i/(res) * (360+app->parameterMap[omg3D2Rotation])/360.0 ) % WIDTH, 0));
//    }
//    
//}
//
//void FX::sphere(ofFbo* src, ofFbo* dest){
//    dest->begin();
//    ofPushMatrix();
//    
//    ofBackground(0);
//    ofSetColor(ofColor::white);
//    ofNoFill();
//    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//    
//    ofDisableArbTex();
//    ofEnableNormalizedTexCoords();
//    glEnable(GL_DEPTH_TEST);
//    
//    src->getTextureReference().bind();
//    ofTranslate(WIDTH/2, HEIGHT/2, 0);
//    ofRotateY(ofGetFrameNum()*2);
//    //    ofRotateX(-45);
//    
//    sphereMesh.draw();
//    
//    src->getTextureReference().unbind();
//    glDisable(GL_DEPTH_TEST);
//    ofDisableNormalizedTexCoords();
//    ofEnableArbTex();
//    
//    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//    
//    ofPopMatrix();
//    dest->end();
//}
//
//void FX::blur(ofFbo* src, ofFbo* dest, float intensity, float mix, bool vertical, float tiltShift){
//    //    if(ofGetFrameNum()%25==0) gBlurHor.load("../shaders/gblurHD");
//    
//    dest->begin();
//    gBlurHor.begin();
//    gBlurHor.setUniform1f("vertical", vertical?1:0);
//    gBlurHor.setUniform1i("resolution", BLUR_RESOLUTION);
//    gBlurHor.setUniform1f("blurDistance", intensity);
//    gBlurHor.setUniform1f("originalMix", mix);
//    gBlurHor.setUniform1f("brightness", 1);
//    gBlurHor.setUniform1f("tiltShift", tiltShift);
//    gBlurHor.setUniform1f("WIDTH", WIDTH);
//    gBlurHor.setUniform1f("HEIGHT", HEIGHT);
//    src->draw(0,0);
//    gBlurHor.end();
//    dest->end();
//}
//
//
//void FX::displayOmg3D2(ofFbo* src, ofFbo* dest, float speed, bool pastFbos,
//                          float rotation, float internalDist, bool symetry,
//                          float addedRotation, int o_index){
//    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//    
//    bool isdancer = false;
//    if(dancer)
//        dancerPlayer.update();
//    
//    bool x2 = app->parameterMap[omg3D2x2];
//    float nb = min((float)MAX_OMG3D2_FBOS, app->parameterMap[omg3D2Nb]);
//    if(x2)
//        nb /= 2;
//    nb = (int) nb;
//    int MINDIST = 800;
//    int MAXDIST = 4000;//*internalDist;
//    
//    if(zs[o_index].size() != (int) nb){ //init //TODO reset zs on macro load ?
//        zs[o_index].clear();
//        for(int x=0; x<nb ;x++){
//            zs[o_index].push_back(ofVec4f((x==0 || zs[o_index][x-1].x > 0) ? ofRandom(-src->getWidth()/4, 0) : ofRandom(0, src->getWidth()/4)
//                                          ,ofRandom(-src->getWidth()/4, src->getWidth()/4)
//                                          ,x/nb
//                                          ,ofRandom(0, src->getWidth()/2)));
//            zsFbosOk.push_back(false);
//        }
//    }
//    
//    ofPushMatrix();{
//        float a = 60;
//        float l = 0.25;
//        int maxZIndex = 0;
//        float maxZ = 0;
//        
//#ifdef REAL_TIME_FX
//        speed /= app->restrictFrameRate/ofGetFrameRate();
//#endif
//        
//        for(int x=0; x<zs[o_index].size();x++){ //updates
//            zs[o_index][x].z += speed;
//            if(app->parameterMap[omg3D2Acc] && zs[o_index][x].z < 0.33)
//                zs[o_index][x].z += speed*3;
//            if(pastFbos){
//                if(zs[o_index][x].z<0 || zs[o_index][x].z>1){
//                    zsFbos[x]->begin();
//                    ofBackground(0);
//                    ofSetColor(ofColor::white);
//                    src->draw(-(src->getWidth() - zsFbos[x]->getWidth())/2,
//                              -(src->getHeight() - zsFbos[x]->getHeight())/2
//                              - (HEIGHT2-HEIGHT)/2);
//                    zsFbos[x]->end();
//                }
//                for(int index=0;index<zsFbos.size();index++) //force init
//                    if(!zsFbosOk[index]){
//                        zsFbos[index]->begin();
//                        ofBackground(0);
//                        ofSetColor(ofColor::white);
//                        src->draw(-(src->getWidth() - zsFbos[x]->getWidth())/2,
//                                  -(src->getHeight() - zsFbos[x]->getHeight())/2
//                                  - (HEIGHT2-HEIGHT)/2);
//                        zsFbos[index]->end();
//                        zsFbosOk[index] = true;
//                    }
//            }
//            
//            bool reset = false;
//            if(zs[o_index][x].z<0){
//                zs[o_index][x].z = 1;
//                reset = true;
//            }
//            if(zs[o_index][x].z>1){
//                zs[o_index][x].z = 0 ;
//                reset = true;
//            }
//            
//            if(reset){
//                zs[o_index][x].x = (x==0 || zs[o_index][x-1].x > 0) ? ofRandom(-src->getWidth()/4, 0) : ofRandom(0, src->getWidth()/4);
//                zs[o_index][x].y = ofRandom(-src->getWidth()/4, src->getWidth()/4);
//                zs[o_index][x].w = ofRandom(0, 1/2.0);
//            }
//            
//            if(zs[o_index][x].z >= maxZ){
//                maxZ = zs[o_index][x].z;
//                maxZIndex = x;
//            }
//        }
//        
//        dest->begin();
//        
//        ofBackground(0);
//        {
//#ifdef GAME
//            app->xOffset = HEIGHT2*ofGetMouseX()*0.001;
//            app->yOffset = HEIGHT2/2*ofGetMouseY()*0.001;
//            //        cam.setPosition(ofPoint(WIDTH/2 + app->xOffset,HEIGHT2/2 - app->yOffset,-500));
//            //        cam.setOrientation((ofVec3f((ofGetMouseY()-HEIGHT/2)/3.0, -(ofGetMouseX()-WIDTH/2)/3.0, 0)));
//            //        cam.lookAt(ofVec3f(WIDTH/2,HEIGHT/2,HEIGHT/2), ofVec3f(1,1,1));
//            cam.begin();
//            cam.enableOrtho();
//#endif
//        }
//        
//        ofTranslate(src->getWidth()/2, src->getHeight()/2);
//        ofRotateY(addedRotation);
//        {
//#ifdef GAME
//            //        ofSetColor(255,0,0,150);
//            //        ofPushMatrix();
//            //        ofTranslate(0, 0, -515);
//            //        ofRect(-FINALWIDTH/2, -HEIGHT2/2 + (HEIGHT2-HEIGHT)/2, WIDTH, src->getHeight());
//            //        ofRect(-1000000, -1000000, -1000000, -1000000);
//            //        ofPopMatrix();
//#endif
//        }
//        int i;
//        float rot = 0;
//        for (int index=0; index<zs[o_index].size();index++){
//            int iindex = index;
//            for(int add=0;add<= x2 ? 1 : 0;add++){
//                if(add==1)
//                    iindex = zs[o_index].size() - index;
//                
//                
//                if(app->parameterMap[omg3D2ZScale]>=0)
//                    i = (iindex + maxZIndex + 1) % zs[o_index].size();
//                else
//                    i = (zs[o_index].size()*2 - ((iindex - maxZIndex))) % zs[o_index].size();
//                
//                
//                //                if(dancer){
//                //                float dancerZ = ofGetMouseX()/(float)WIDTH;
//                //                if(!isdancer && zs[o_index][i].z > dancerZ){
//                //                    isdancer = true;
//                //                    ofPushMatrix();
//                //                   ofTranslate(-WIDTH/2,-HEIGHT/2,0);
//                //                    dancerPlayer.update();
//                //                    alphaShader.begin();
//                //                    alphaShader.setUniform1f("alphaOffset", 1);
//                ////                    ofScale(2,2);
//                //                    dancerPlayer.draw(0,0);
//                //                    alphaShader.end();
//                //                    ofPopMatrix();
//                //                }
//                //                }
//                
//                
//                ofPushMatrix();
//                {
//                    float x = zs[o_index][i].x * app->parameterMap[divergence] * 10;
//                    float y = zs[o_index][i].y * app->parameterMap[divergenceY] * 10;
//                    float z = zs[o_index][i].z;
//                    
//                    float ac = app->parameterMap[omg3D2AvoidCenter];
//                    if(ac>0 && abs(x)<ac*src->getHeight())
//                        x = ac*src->getHeight() * x/abs(x);
//                    if(ac<0 && abs(x)>ac*src->getHeight())
//                        x = -ac*src->getHeight()  * x/abs(x);
//                    if(ac>0 && abs(y)<ac*src->getHeight())
//                        y = ac*src->getHeight()  * y/abs(y);
//                    if(ac<0 && abs(y)>ac*src->getHeight())
//                        y = -ac*src->getHeight()  * y/abs(y);
//                    
//                    if(add==1)
//                        z = 1 - z;
//                    
//                    if(app->parameterMap[user1]){ //special scale
//                        float s = (1+app->parameterMap[omg3D2Depth])*(z*z);
//                        ofScale(s,s);
//                    }else{
//                        
//                        z = (z*(MINDIST + MAXDIST) - MAXDIST - MINDIST * (1-app->parameterMap[omg3D2Depth])) * app->parameterMap[omg3D2] * internalDist;
//                        
//                        if(app->parameterMap[omg3D2X]){
//                            x +=  (zs[o_index][i].z-0.5) * src->getWidth()/2.0 * app->parameterMap[omg3D2X];
//                            //                        z = 0;
//                        }
//                        if(app->parameterMap[omg3D2Y]){
//                            y +=  (zs[o_index][i].z-0.5) * src->getHeight() /(2.0) * app->parameterMap[omg3D2Y];
//                            //                        z = 0;
//                        }
//                        
//                        z += (zs[o_index][i].w - (1-app->parameterMap[omg3D2Depth]) * 1/4.0)
//                        * app->parameterMap[divergenceZ] * src->getHeight();
//                        
//                        
//                        ofTranslate(x, y, z);
//                    }
//                    
//                    
//                    if(app->parameterMap[omg3D2RollerX] || app->parameterMap[omg3D2RollerY]){
//                        float zzz = (0.8-zs[o_index][i].z);
//                        zzz *= zzz;
//                        ofTranslate(zzz*app->parameterMap[omg3D2RollerX]*src->getHeight(),
//                                    zzz*app->parameterMap[omg3D2RollerY]*src->getHeight());
//                    }
//                    
//                    rot = ofGetFrameNum()*app->parameterMap[omg3D2FreeRotation];
//                    if(! app->parameterMap[omg3D2HardRotation] )
//                        rot += (1-zs[o_index][i].z) * rotation ;
//                    else
//                        rot += rotation*i;
//                    
//                    ofRotateZ(rot);
//                    if(app->parameterMap[aDivergence]>0)
//                        ofTranslate(0, app->parameterMap[aDivergence]*src->getHeight()/2 * (1-zs[o_index][i].z));
//                    else
//                        ofTranslate(0, abs(app->parameterMap[aDivergence])*src->getHeight()/2 );
//                    
//                    
//                    if(app->parameterMap[omg3D2ZScale]!=00)
//                        ofScale(1+abs(app->parameterMap[omg3D2ZScale])*(1-zs[o_index][i].z),
//                                1+abs(app->parameterMap[omg3D2ZScale])*(1-zs[o_index][i].z));
//                    
//                    float alpha_ = 1;
//                    if(app->parameterMap[omg3D2Alpha0])
//                        alpha_ *= ofMap(zs[o_index][i].z, 0.85, 0.92 + (0.08 * (1-app->parameterMap[omg3D2Depth])), 1, min(1.0f, max(0.0f, 1-app->parameterMap[omg3D2Alpha0])), true); //TODO add maxdist/mindist so that the algo is ok with different z scales
//                    float aaa = app->parameterMap[omg3D2AlphaZ];
//                    if(aaa>0)
//                        alpha_ = alpha_*ofMap(zs[o_index][i].z, 0, 0.8, (1-aaa), 1, true);
//                    
//                    //                alpha_ = 1; //TEST
//                    
//                    if(app->parameterMap[omg3D2DOF]>0){
//                        blur(src, &tmpFbo, (1-alpha_)*10*app->parameterMap[omg3D2DOF], 0, 1, 0);
//                        blur(&tmpFbo, &tmpFbo2, (1-alpha_)*10*app->parameterMap[omg3D2DOF], 0, 0, 0);
//                    }
//                    
//                    if(app->parameterMap[omg3D2LumaKey]){
//                        alphaShader.begin();
//                        alphaShader.setUniform1f("alphaOffset", alpha_);
//                        if(app->parameterMap[omg3D2Strobe])
//                            alphaShader.setUniform1f("invert", i%2==1);
//                    }
//                    
//                    
//                    ofSetColor(255*alpha_,255*alpha_,255*alpha_,alpha_*255);//psychedelia !
//                    
//                    if(pastFbos){
//                        ofFbo* pf = app->getPastFbo(index);
//                        if(pf == 0){
//                            if(zsFbos.size() > i){
//                                zsFbos[i]->draw(-zsFbos[i]->getWidth() /2, -zsFbos[i]->getHeight() /2 + (HEIGHT2-HEIGHT)/2);
//                            }
//                        }else{
//                            pf->draw(-zsFbos[i]->getWidth() /2, -zsFbos[i]->getHeight() /2 + (HEIGHT2-HEIGHT)/2);
//                        }
//                    }else{
//                        ofPushMatrix();
//                        
//                        if(app->parameterMap[user2]){ /// ??? must be overture
//                            ofTranslate(0,src->getHeight()/2);
//                            ofRotate(ofMap(zs[o_index][i].z, 0.75, 0.9, 0, 45, true) * ((x<-1 && symetry) || (app->parameterMap[divergence]<=0.01 && symetry && i%2==0) ? 1 : -1) );
//                            ofTranslate(0,-src->getHeight()/2);
//                        }
//                        
//                        if((x<-1 && symetry) || (app->parameterMap[divergence]<=0.01 && symetry && i%2==0))
//                            ofScale(-1,1);
//                        //                    src->draw(-app->FINALWIDTH/2+app->FINALWIDTH, -HEIGHT2/2, -app->FINALWIDTH, src->getHeight()); //invert image
//                        //                else
//                        
//                        //                src->draw(-WIDTH/2, -HEIGHT/2); //sampling from lower res
//                        if(app->parameterMap[omg3D2DOF]>0){
//                            tmpFbo2.draw(-src->getWidth()/2, -src->getHeight()/2);
//                        }else{
//                            src->draw(-src->getWidth()/2, -src->getHeight()/2);
//                        }
//                        
//                        ofPopMatrix();
//                    }
//                    if(app->parameterMap[omg3D2LumaKey])
//                        alphaShader.end();
//                }ofPopMatrix();
//            }
//        }
//        
//        dest->end();
//        
//    }ofPopMatrix();
//    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//}
//
//void FX::alphaBlend(ofFbo* src){
//    alphaShader.begin();
//    alphaShader.setUniform1f("alphaOffset", 1);
//    src->draw(0,0);
//    alphaShader.end();
//}
//
//void FX::colorGrading(ofFbo* src, ofFbo* dest, bool invert, float tintAmp, float reTint, float sidesSaturation){
//    
//    dest->begin();
//    chromaShader.begin();
//    chromaShader.setUniform1f("WIDTH", src->getWidth());
//    chromaShader.setUniform1f("HEIGHT", src->getHeight());
//    chromaShader.setUniform1f("hue", app->parameterMap[reTintHue]);
//    chromaShader.setUniform1f("gradient", app->parameterMap[gradient]);
//    chromaShader.setUniform1f("reSaturate", app->parameterMap[reSaturate]);
//    chromaShader.setUniform1f("reTint", reTint);
//    src->draw(0,0);
//    chromaShader.end();
//    dest->end();
//}
//
//void FX::chromaSeparation(ofFbo* src, ofFbo* dest, bool invert, float chromaSep, float chromasepHue, float chromasepAngle, float chromaOffset, float sidesSaturation){
//    //    chromaShader.load("../shaders/chromaSep");
//    //    chromaSepShader.load("../shaders/chromaSepOnly");
//    dest->begin();
//    chromaSepShader.begin();
//    chromaSepShader.setUniform1f("WIDTH", src->getWidth());
//    chromaSepShader.setUniform1f("HEIGHT", src->getHeight());
//    chromaSepShader.setUniform1f("intensity", chromaSep);
//    chromaSepShader.setUniform1f("sepAlpha", app->parameterMap[chromaSepAlpha]);
//    float csHue = chromasepHue;
//    if(tintAmp<1)
//        csHue = ofMap(chromasepHue/255-app->parameterMap[tintHue], 0, 1, 0- app->parameterMap[int(tintAmp)]/2, app->parameterMap[int(tintAmp)]/2)*255;
//    chromaSepShader.setUniform1f("chromasepHue", csHue);
//    chromaSepShader.setUniform1f("chromaOffset", chromaOffset);
//    chromaSepShader.setUniform1f("chromasepAngle", chromasepAngle);
//    chromaSepShader.setUniform1f("chromaSepMode", app->parameterMap[chromaSepMode]);
//    chromaSepShader.setUniform1f("sidesSaturation", sidesSaturation);
//    chromaSepShader.setUniform1i("aberration", chromaOffset < 0 ? 1 : 0);
//    src->draw(0,0);
//    chromaSepShader.end();
//    dest->end();
//}
//
//
//void FX::tunnel(ofFbo* src, ofFbo* dest, float speed, bool pastFbos, float rotation, float internalDist, bool symetry, bool freeRotation){
//    
//    float a = -7;
//    
//    tunnelFbo.begin();
//    ofBackground(0);
//    tunnelFbo.end();
//    
//    tmpFbo.begin();
//    ofBackground(0);
//    tmpFbo.end();
//    
//    ofPushMatrix();
//    app->parameterMap[tintHue] = 0;
//    displayOmg3D2(src, &tmpFbo, speed/3, pastFbos, rotation, internalDist, symetry, a, 0);
//    ofPopMatrix();
//    
//    ofSetColor(ofColor::white);
//    dest->begin();
//    ofBackground(0);
//    tmpFbo.draw(0,0);
//    dest->end();
//    
//    
//    ofPushMatrix();
//    ofRotateY(a); //cyan
//    displayOmg3D2(src, &tmpFbo, speed/3, pastFbos, rotation, internalDist, symetry, a, 0);
//    ofPopMatrix();
//    ofSetColor(ofColor::white);
//    
//    app->parameterMap[reSaturate] = 1;
//    app->parameterMap[tintHue] = 0.5;
//    colorGrading(&tmpFbo, &tunnelFbo, 0, 0, 1, 0);
//    dest->begin();
//    //    dualShader.begin();
//    //    dualShader.setUniform1f("left", 1);
//    //    dualShader.setUniform1f("WIDTH", src->getWidth());
//    tunnelFbo.draw(0,0);
//    //    dualShader.end();
//    dest->end();
//    
//    ofPushMatrix();
//    ofRotateY(-a); //red
//    displayOmg3D2(src, &tmpFbo, speed/3, pastFbos, rotation, internalDist, symetry, -a, 0);
//    ofPopMatrix();
//    ofSetColor(ofColor::white);
//    
//    colorGrading(&tmpFbo, &tunnelFbo, 0, 0.5, 1, 0);
//    dest->begin();
//    //    dualShader.begin();
//    //    dualShader.setUniform1f("left", 0);
//    //    dualShader.setUniform1f("WIDTH", src->getWidth());
//    tunnelFbo.draw(0,0);
//    //    dualShader.end();
//    dest->end();
//    
//    app->parameterMap[reSaturate] = 0;
//}
//
//
//void FX::display3DWorld(ofFbo* src, ofFbo* dest){
//    int nbSprites = 100;
//    while(spritePositions.size() < nbSprites){
//        spritePositions.push_back(ofVec3f(ofRandom(-1,1), ofRandom(-1,1), ofRandom(-1,1)));
//        spriteMeshes.push_back(ofMesh::plane(WIDTH, src->getHeight()));
//    }
//    
//    position += (ofVec3f(ofNoise(position.x+1,position.y), ofNoise(position.y+1,position.x), ofNoise(position.x+1,-position.y)) - ofVec3f(0.2,0.2,0.2)) / 10.0 * app->parameterMap[user5];
//    rotation += (ofVec3f(ofNoise(position.x+1,position.y), ofNoise(position.y+1,position.x), ofNoise(position.x+1,-position.y))) * 2;
//    
//    if(position.x > 0.5 || position.x < -0.5 )  position.x = 0;
//    if(position.y > 0.5 || position.y < -0.5 )  position.y = 0;
//    if(position.z > 0.5 || position.z < -0.5 )  position.z = 0;
//    
//    dest->begin();
//    ofBackground(0);
//    ofPushMatrix();
//    ofTranslate(position*ofVec3f(src->getHeight(), src->getHeight(), src->getHeight()));
//    //    ofRotateX(rotation.x);
//    ofRotateY(rotation.y);
//    //    ofRotateZ(rotation.z);
//    
//    //    ofDisableArbTex();
//    //    ofEnableNormalizedTexCoords();
//    //    glEnable(GL_DEPTH_TEST);
//    
//    
//    alphaShader.begin();
//    alphaShader.setUniform1f("alphaOffset", 1);
//    for(int i = 0; i<spritePositions.size();i++){
//        ofVec3f p = spritePositions[i];
//        ofPushMatrix();
//        ofTranslate(p*ofVec3f(src->getHeight(), src->getHeight(), src->getHeight()));
//        //        ofMatrix4x4 matrix;
//        //        matrix.makeLookAtViewMatrix(position, p, ofVec3f(1,1,1));
//        //        ofVec3f r = matrix.getRotate().getEuler();
//        //        cout << r << endl;
//        //        ofRotateX(r.x);
//        //        ofRotateY(r.y);
//        //        ofRotateZ(r.z);
//        ofRotateY(0.1* i*ofGetFrameNum());
//        src->draw(0,0);
//        //        src->getTextureReference().bind();
//        //        spriteMeshes[i].draw();
//        //        src->getTextureReference().unbind();
//        
//        ofPopMatrix();
//    }
//    alphaShader.end();
//    
//    //    glDisable(GL_DEPTH_TEST);
//    //    ofDisableNormalizedTexCoords();
//    //    ofEnableArbTex();
//    
//    ofPopMatrix();
//    dest->end();
//    
//}
//
//void FX::displayOmg3D(ofFbo* src, ofFbo* dest){
//    ofEnableBlendMode(OF_BLENDMODE_SCREEN);
//    
//    float intensity = app->parameterMap[omg3D];
//    float aa = app->parameterMap[omg3DAngle]*intensity;
//    int W = app->FINALWIDTH;
//    
//    dest->begin();
//    ofBackground(0);
//    alphaShader.begin();
//    alphaShader.setUniform1f("alphaOffset", intensity);
//    ofPushMatrix();
//    {
//        ofTranslate(app->FINALWIDTH/2, app->FINALHEIGHT/2 + (HEIGHT2-HEIGHT)/2);
//        float a = aa ;
//        float l = 0.25*intensity;
//        
//        ofPushMatrix();
//        ofTranslate(-app->FINALWIDTH*l, 0);
//        ofRotateY(a);
//        src->draw(-app->FINALWIDTH/2, -HEIGHT2/2, app->FINALWIDTH, src->getHeight());
//        ofPopMatrix();
//        
//        ofPushMatrix();
//        ofTranslate(app->FINALWIDTH*l, 0);
//        ofRotateY(-a);
//        src->draw(-app->FINALWIDTH/2+app->FINALWIDTH, -HEIGHT2/2, -app->FINALWIDTH, src->getHeight());
//        ofPopMatrix();
//        
//        ofPushMatrix();
//        ofTranslate(0, app->FINALHEIGHT*l);
//        ofRotateX(a);
//        src->draw(-app->FINALWIDTH/2, -HEIGHT2/2, app->FINALWIDTH, src->getHeight());
//        ofPopMatrix();
//        
//        ofPushMatrix();
//        ofTranslate(0, -app->FINALHEIGHT*l);
//        ofRotateX(-a);
//        src->draw(-app->FINALWIDTH/2, -HEIGHT2/2 + HEIGHT2, app->FINALWIDTH, -src->getHeight());
//        ofPopMatrix();
//    }
//    ofPopMatrix();
//    alphaShader.end();
//    dest->end();
//    
//    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//}
//
//
//void FX::stripe(float size) {
//    size *= HEIGHT/2;
//    
//    if (app->parameterMap[tintSaturation]>0)
//        ofSetColor(ofColor::fromHsb(255*app->parameterMap[tintHue], 255*app->parameterMap[tintSaturation], ofRandom(255)));
//    else
//        ofSetColor(ofColor::fromHsb(ofRandom(255), ofRandom(200), ofRandom(255)));
//    
//    float h = ofRandom(size/2, size);
//    float y = ofRandom(HEIGHT2-size);
//    ofDrawRectangle(0, y-h/2, WIDTH, h);
//}
//
//void FX::doInvert(ofFbo* src, ofFbo* dest){
//    //sInvert.load("../shaders/invert");
//    dest->begin();
//    sInvert.begin();
//    sInvert.setUniform1f("WIDTH", src->getWidth());
//    sInvert.setUniform1f("HEIGHT", src->getHeight());
//    sInvert.setUniform1f("circle", app->parameterMap[invertCircle] * (app->parameterMap[_invert] ? -1 : 1));
//    sInvert.setUniform1i("mode", 1);
//    src->draw(0,0);
//    sInvert.end();
//    dest->end();
//}
//
//void FX::doStrobe(ofFbo* src, ofFbo* dest){
//    //sInvert.load("../shaders/invert");
//    dest->begin();
//    sInvert.begin();
//    sInvert.setUniform1i("mode", app->parameterMap[strobe]);
//    src->draw(0,0);
//    sInvert.end();
//    dest->end();
//}
//
//void FX::doPerturbation(ofFbo* src, ofFbo* dest){
//    //if(ofGetFrameNum()%25==0) perturbationShader.load("../shaders/perturbation_old");
//    dest->begin();
//    perturbationShader.begin();
//    perturbationShader.setUniform1f("WIDTH", src->getWidth());
//    perturbationShader.setUniform1f("HEIGHT", src->getHeight());
//    perturbationShader.setUniform1f("intensity", app->parameterMap[pert]);
//    perturbationShader.setUniform1f("evolution", app->parameterMap[pertEvo]);
//    perturbationShader.setUniform1f("scale", app->parameterMap[pertEvo]);
//    perturbationShader.setUniform1f("freq", app->parameterMap[pertFreq]);
//    perturbationShader.setUniform1f("persistance", app->parameterMap[pertPersistance]);
//    perturbationShader.setUniform1f("hueZ", app->parameterMap[pertHue]);
//    perturbationShader.setUniform1i("mode", (int) app->parameterMap[pertMode]);
//    src->draw(0,0);
//    perturbationShader.end();
//    dest->end();
//}
//
//void FX::doPNoise(ofFbo* src, ofFbo* dest){
//    //   perturbationShader.load("../shaders/perturbation");
//    dest->begin();
//    pnoiseShader.begin();
//    pnoiseShader.setUniform1f("WIDTH", src->getWidth());
//    pnoiseShader.setUniform1f("HEIGHT", src->getHeight());
//    pnoiseShader.setUniform1f("intensity", app->parameterMap[pnoise]);
//    pnoiseShader.setUniform1f("evolution", app->parameterMap[pertEvo]);
//    pnoiseShader.setUniform1f("freq", app->parameterMap[pertFreq]);
//    pnoiseShader.setUniform1f("persistance", app->parameterMap[pertPersistance]);
//    pnoiseShader.setUniform1f("hueZ", app->parameterMap[pertHue]);
//    pnoiseShader.setUniform1i("mode", (int) app->parameterMap[pertMode]);
//    src->draw(0,0);
//    pnoiseShader.end();
//    dest->end();
//}
//
//
//void FX::doLSD(ofFbo* src, ofFbo* dest){
//    dest->begin();
//    lsdShader.begin();
//    lsdShader.setUniform1f("intensity", app->parameterMap[warp]);
//    lsdShader.setUniform1f("remap", 1);
//    //    lsdShader.setUniform1f("cx", app->parameterMap[warpX]);
//    //    lsdShader.setUniform1f("cy", app->parameterMap[warpY]);
//    lsdShader.setUniform1f("WIDTH", src->getWidth());
//    lsdShader.setUniform1f("HEIGHT", src->getHeight());
//    src->draw(0,0);
//    lsdShader.end();
//    dest->end();
//}
//
//void FX::doToCircle(ofFbo* src, ofFbo* dest, float amt){
//    //    LOAD_ASYNC    circleShader2.load("../shaders/circle2");
//    
//    ofShader* shader = &circleShader;
//    if(amt<0)
//        shader = &circleShader2;
//    
//    dest->begin();
//    shader->begin();
//    shader->setUniform1f("WIDTH", src->getWidth());
//    shader->setUniform1f("HEIGHT", src->getHeight());
//    shader->setUniform1f("amount", abs(amt));
//    //    circleShader.setUniform1f("minRad", 0.2);
//    //    circleShader.setUniform1f("maxRad", 1);
//    ofBackground(0);
//    src->draw(0,0);
//    circleShader.end();
//    dest->end();
//}
//
//void FX::pixellate(ofFbo* src, ofFbo* dest, float x, float y){
//    dest->begin();
//    pixelShader.begin();
//    pixelShader.setUniform1f("WIDTH", src->getWidth());
//    pixelShader.setUniform1f("HEIGHT", src->getHeight());
//    pixelShader.setUniform1i("xresolution", (int)x);
//    pixelShader.setUniform1i("yresolution", (int)y);
//    //    pixelShader.setUniform1i("xfocus", ofGetMouseX());
//    //    pixelShader.setUniform1i("yfocus", HEIGHT-ofGetMouseY());
//    src->draw(0,0);
//    pixelShader.end();
//    dest->end();
//}
//
//void FX::sobelContours(ofFbo* src, ofFbo* dest, float intensity, float mix, float white){
//    ofShader* shader = app->parameterMap[sobel>0] ? &sobelShader : &sobelHDShader;
//    dest->begin();
//    //  sobelHDShader.load("../shaders/sobelHD");
//    shader->begin();
//    //    sobelShader.setUniform1i("fast", 1);
//    shader->setUniform1f("intensity", intensity);
//    shader->setUniform1f("mix_", mix);
//    shader->setUniform1f("white",white);
//    //    sobelShader.setUniform1i("hueLevels", ofGetMouseX()/10);
//    //    sobelShader.setUniform1i("lumLevels", ofGetMouseY()/10);
//    ofBackground(0);
//    src->draw(0,0);
//    shader->end();
//    dest->end();
//}
//
//
//
//void FX::blurBegin(float intensity, float mix, int resolution, ofFbo* a){
//    a->begin();
//    gBlurHor.begin();
//    gBlurHor.setUniform1f("vertical", 0);
//    gBlurHor.setUniform1i("resolution", BLUR_RESOLUTION);
//    gBlurHor.setUniform1f("blurDistance", intensity);
//    gBlurHor.setUniform1f("totalMix", mix);
//    gBlurHor.setUniform1f("brightness", 1);
//}
//
//void FX::blurEnd(float intensity, float mix, int resolution, ofFbo* a){
//    gBlurHor.end();
//    a->end();
//    
//    gBlurHor.begin();
//    gBlurHor.setUniform1f("vertical", 1);
//    gBlurHor.setUniform1i("resolution", BLUR_RESOLUTION);
//    gBlurHor.setUniform1f("blurDistance", intensity);
//    gBlurHor.setUniform1f("totalMix", mix);
//    gBlurHor.setUniform1f("brightness", 1);
//    a->draw(0,0);
//    gBlurHor.end();
//}
//
//void FX::doGamma(ofFbo* src, ofFbo* dest, float gamma){
//    dest->begin();
//    hdrShader.begin();
//    hdrShader.setUniform1f("contrast", app->parameterMap[contrast]);
//    hdrShader.setUniform1f("gamma", gamma);
//    src->draw(0,0);
//    hdrShader.end();
//    dest->end();
//}
//
//void FX::doKalei(ofFbo* src, ofFbo* dest, float mix, float kaleiNb, float kaleiOffX, float kaleiOffY){
//    dest->begin();
//    //    if(ofGetFrameNum()%25==0) kShader.load("../shaders/kalei");
//    kShader.begin();
//    kShader.setUniform1f("WIDTH", (float) src->getWidth());
//    kShader.setUniform1f("HEIGHT", (float) src->getHeight());
//    kShader.setUniform1f("sides", kaleiNb);
//    kShader.setUniform1f("x", (float )kaleiOffX*src->getWidth());
//    kShader.setUniform1f("y", (float )kaleiOffY*src->getHeight());
//    kShader.setUniform1f("copy", (float )app->parameterMap[kaleiCopy]);
//    kShader.setUniform1f("copyAdjust", (float )app->parameterMap[kaleiCopyAdjust]);
//    kShader.setUniform1f("copyBorder", (float )app->parameterMap[kaleiCopyBorder]);
//    kShader.setUniform1f("mirror", (float )app->parameterMap[kaleiMirror]);
//    kShader.setUniform1f("scale", (float )app->parameterMap[kaleiScale]);
//    kShader.setUniform1f("t", (float ) ofGetFrameNum());
//    kShader.setUniform1f("_mix", (float ) mix);
//    src->draw(0,0);
//    kShader.end();
//    dest->end();
//}
//
//void FX::doGlow(ofFbo* src, ofFbo* dest, ofFbo* tempFbo,  ofFbo* tempFbo2, float amount, float intensity, float resolution){
//    //lag not in the shader but tempFbo writing
//    //    tempFbo->clear();
//    
//    tempFbo->begin(); //6 IPS
//    ofSetColor(ofColor::black);
//    ofRectangle(0,0,src->getWidth(),src->getHeight());
//    ofSetColor(ofColor::white);
//    gBlurHor.begin();
//    gBlurHor.setUniform1f("vertical", 0);
//    gBlurHor.setUniform1i("resolution", resolution);
//    gBlurHor.setUniform1f("blurDistance", 2);
//    gBlurHor.setUniform1f("brightness", intensity);
//    gBlurHor.setUniform1f("originalMix", 0);
//    gBlurHor.setUniform1f("tiltShift", 0);
//    src->draw(0,0);
//    gBlurHor.end();
//    tempFbo->end();
//    
//    tempFbo2->begin(); //6IPS
//    ofSetColor(ofColor::black);
//    ofRectangle(0,0,src->getWidth(),src->getHeight());
//    ofSetColor(ofColor::white);
//    gBlurHor.begin();
//    gBlurHor.setUniform1f("vertical", 1);
//    gBlurHor.setUniform1i("resolution", resolution);
//    gBlurHor.setUniform1f("blurDistance", 2);
//    gBlurHor.setUniform1f("brightness", intensity);
//    gBlurHor.setUniform1f("originalMix", 0);
//    gBlurHor.setUniform1f("tiltShift", 0);
//    tempFbo->draw(0,0);
//    gBlurHor.end();
//    tempFbo2->end();
//    
//    //    dest->begin();
//    //    ofSetColor(ofColor::black);
//    //    ofRect(0,0,src->getWidth(),src->getHeight());
//    //    ofSetColor(ofColor::white);
//    //    tempFbo2->draw(0,0);
//    //    dest->end();
//}
//
//void FX::randHide(ofFbo* src, ofFbo* dest){
//    dest->begin();
//    src->draw(0,0);
//    float vv = app->parameterMap[randVHide];
//    float hh = app->parameterMap[randHHide];
//    ofSetColor(ofColor::black);
//    if(vv>0 && hh >0){
//        float y = ofRandom(src->getHeight());
//        float h = ofRandom(HEIGHT/4*vv);
//        float x = ofRandom(src->getWidth());
//        float w = ofRandom(WIDTH/4*hh);
//        ofRectangle(0, 0, x, y);
//        ofRectangle(x+w, 0, src->getWidth(), y);
//        ofRectangle(0, y+h, x, src->getHeight());
//        ofRectangle(x+w, y+h, src->getWidth(), src->getHeight());
//    }else
//        if(vv>0){
//            float y = ofRandom(src->getHeight());
//            float h = ofRandom(src->getHeight()/4*vv);
//            ofRectangle(0, 0, src->getWidth(), y);
//            ofRectangle(0, y+h, src->getWidth(), src->getHeight());
//        }else{
//            float x = ofRandom(src->getWidth());
//            float w = ofRandom(src->getWidth()/4*hh);
//            ofRectangle(0, 0, x, src->getHeight());
//            ofRectangle(x+w, 0, src->getWidth(), src->getHeight());
//        }
//    ofSetColor(ofColor::white);
//    dest->end();
//}
//
///****************************************** SKEW **/
//
//
//
//void FX::updateSkew(float jumpRate, bool hard,
//                       float bandingProb, float bandingJumpProb, float* skewVector){
//    float off = 0;
//    float banding = 0;
//    bool firstBand = true;
//    for (int i=0; i<WIDTH; i++) {
//        if(banding==0 && ofRandom(1)<bandingProb)
//            banding = ofRandom(-50,50);
//        if(banding||firstBand){
//            if(ofRandom(1)<bandingJumpProb || firstBand)
//                banding = banding + ofRandom(-50,50);
//            firstBand = false;
//        }
//        
//        skewVector[i] = off + ofRandom(-1, 1) + banding;
//        if(hard){ //mode bourrin
//            if(skewVector[i]<-0.9)
//                skewVector[i] = -1;
//            else if(skewVector[i]>0.9)
//                skewVector[i] = 1;
//            else if(skewVector[i]<0)
//                skewVector[i] = -0.9;
//            else if(skewVector[i]>0)
//                skewVector[i] = 0.9;
//        }
//        off = skewVector[i];
//        if (ofRandom(1)<jumpRate)
//            off = 0;
//    }
//}
//
//void FX::updateSkew(float *skewVector){
//    ofSeedRandom(ofGetFrameNum());
//    updateSkew(ofRandom(0.013), false, ofRandom(0.55), ofRandom(0.25), skewVector);
//}
//
//ofImage* getSkewTextureImage(){
//    return &skewImage;
//}
//
//int FX::getSkewOffset(){
//    return off;
//}
//
////mode 0 : hor 1: vert 2: dist, 3 : angle
//void FX::skew(ofFbo* src, ofFbo* dest, float intensity, float offset, int mode){
//    
//    ofShader* shader = app->parameterMap[selectiveShading] ? &skewShader_sel : &skewShader;
//    
//    dest->begin();
//    shader->begin();
//    skewShader_sel.setUniform1f("selective", app->parameterMap[selectiveShading]);
//    
//    shader->setUniform1f("WIDTH", src->getWidth());
//    shader->setUniform1f("HEIGHT", src->getHeight());
//    shader->setUniform1f("vertical", (mode==1)?1:0);
//    shader->setUniform1f("dist", (mode==2)?1:0);
//    shader->setUniform1f("angle", (mode==3)?1:0);
//    shader->setUniform1f("offset", offset);
//    shader->setUniform1f("intensity", intensity);
//    shader->setUniform1fv("skew", skewVector, src->getWidth());
//    src->draw(0,0);
//    shader->end();
//    dest->end();
//}
//
//
///***************************** DISPLACE **/
//
//float* getDisplaceVector(){
//    return displaceVector;
//}
//
//void FX::updateDisplace(float prob) {
//    unsigned char pixes[app->render_WIDTH*4];
//    float offx = 0;
//    for(int y=0;y<app->render_WIDTH; y++){
//        if (ofRandom(1)<prob)
//            offx = ofRandom(-1,1);
//        displaceVector[y] = offx;
//    }
//}
//
//void FX::doNoise(ofFbo* src, ofFbo* dest){
//    //   if(ofGetFrameNum()%25==0) noiseShader.load("../shaders/noise");
//    
//    dest->begin();
//    noiseShader.begin();
//    noiseShader.setUniform1f("intensity", app->parameterMap[noise]);
//    noiseShader.setUniform1f("seed", ofGetFrameNum());
//    src->draw(0,0);
//    noiseShader.end();
//    dest->end();
//}
//
//void FX::displace(ofFbo* src, ofFbo* dest, float intensity, float displaceProba, bool vertical){
//    ofShader* shader = app->parameterMap[selectiveShading] ? &displaceShader_sel : &displaceShader;
//    
//    updateDisplace(displaceProba);
//    
//    dest->begin();
//    shader->begin();
//    displaceShader_sel.setUniform1f("selective", app->parameterMap[selectiveShading]);
//    
//    shader->setUniform1i("WIDTH", src->getWidth());
//    shader->setUniform1i("HEIGHT", src->getHeight());
//    shader->setUniform1f("vertical", vertical?1:0);
//    shader->setUniform1f("intensity", intensity);
//    shader->setUniform1fv("displace", getDisplaceVector(), src->getWidth());
//    
//    src->draw(0,0);
//    shader->end();
//    
//    dest->end();
//}
//
//void FX::displaceGlitch(ofFbo* src, ofFbo* dest, float intensity, float displaceProba, bool vertical){
//    //    displaceGlitchShader.load("../shaders/displaceGlitch");
//    int n =10;
//    for(int i=0;i<n;i++){
//        dest->begin();
//        updateDisplace(displaceProba);
//        displaceGlitchShader.begin();
//        displaceGlitchShader.setUniform1f("x", ofGetMouseX());
//        displaceGlitchShader.setUniform1f("y", HEIGHT2 - ofGetMouseY());
//        float z = 25;
//        displaceGlitchShader.setUniform1f("w", 100);
//        displaceGlitchShader.setUniform1f("h", 10);
//        displaceGlitchShader.setUniform1f("offx", ofRandom(-50, 50));
//        displaceGlitchShader.setUniform1f("offy", ofRandom(-50, 50));
//        displaceGlitchShader.end();
//        dest->end();
//        if(i!=n-1){
//            ofFbo* tmp;
//            tmp = src;
//            src = dest;
//            dest = tmp;
//        }
//    }
//}
//
//void FX::doTrium(ofFbo* src, ofFbo* dest){
//    dest->begin();
//    triumShader.begin();
//    triumShader.setUniform1f("WIDTH", src->getWidth());
//    src->draw(0,0);
//    triumShader.end();
//    dest->end();
//    
//}
//
//void FX::doTiles(ofFbo* src, ofFbo* dest){
//    //    LOAD_ASYNC     tilesShader.load("../shaders/tiles");
//    float pos[10] = {0, 0.8, 0.5, 0.1, 0.6, 0.3, 0.7, 0.2, 0.4, 0};
//    float inv[10] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
//    dest->begin();
//    tilesShader.begin();
//    tilesShader.setUniform1f("WIDTH", src->getWidth());
//    tilesShader.setUniform1f("HEIGHT", src->getHeight());
//    tilesShader.setUniform1f("SRC_WIDTH", WIDTH);
//    tilesShader.setUniform1f("SRC_HEIGHT", HEIGHT);
//    //    tilesShader.setUniform1fv("pos", pos);
//    //    tilesShader.setUniform1fv("inv", inv);
//    tilesShader.setUniform1f("nb", app->parameterMap[tiles]);
//    src->draw(0,0);
//    triumShader.end();
//    dest->end();
//}
//
//
///*********************** SORT **/
//
////pixel sorting "quality"
//inline float FX::q(ofColor c) {
//    //return maaax((float)0, (c.r + c.b + c.g)/3.0 );
//    return c.getBrightness();
//}
//
////todo analyze 1/2 ?
//
//
//ofImage* FX::sortBrightXCenter(ofPixels *img, float thresh) {
//    ofColor c;
//    float b;
//    
//    float opt = 0; //optimization factor
//    int xres = 1 + opt*2;
//    
//    int interlace = 3; //2 is better but meh
//    
//    int r = ofGetFrameNum() % interlace;
//    
//    for (int y=r; y<img->getHeight(); y+=interlace) {
//        c = img->getColor(img->getWidth()/2,y);
//        b = q(c);
//        img->setColor(img->getWidth()/2,y,c);
//        for (int x=img->getWidth()/2; x<img->getWidth(); x+=xres) {
//            if (abs(q((img->getColor(x,y))) - b) > thresh) {
//                c = img->getColor(x,y);
//                b = q(c);
//            }
//            for(int zz=0;zz<xres;zz++)
//                sortImage.setColor(x+zz,y,c);
//        }
//        c = img->getColor(img->getWidth()/2,y);
//        b = q(c);
//        img->setColor(img->getWidth()/2-1,y,c);
//        for (int x=img->getWidth()/2; x>=xres; x-=xres) {
//            if (abs(q((img->getColor(x,y))) - b) > thresh) {
//                c = img->getColor(x,y);
//                b = q(c);
//            }
//            for(int zz=0;zz<xres;zz++)
//                sortImage.setColor(x-zz,y,c);
//        }
//    }
//    sortImage.update();
//    return &sortImage;
//}
//
//ofImage* FX::sortBrightYCenter(ofPixels *img, float thresh) {
//    ofColor c;
//    float b;
//    
//    float opt = 1; //optimization factor
//    int xres = 1 + opt*2;
//    
//    int interlace = 2; //2 is better but meh
//    
//    int r = ofGetFrameNum() % interlace;
//    
//    for (int x=r; x<img->getWidth(); x+=interlace) {
//        c = img->getColor(x,img->getHeight()/2);
//        b = q(c);
//        img->setColor(x,img->getHeight()/2,c);
//        for (int y=img->getHeight()/2; y<img->getHeight(); y+=xres) {
//            if (abs(q((img->getColor(x,y))) - b) > thresh) {
//                c = img->getColor(x,y);
//                b = q(c);
//            }
//            for(int zz=0;zz<xres;zz++)
//                sortImage.setColor(x,y+zz,c);
//        }
//        c = img->getColor(x,img->getHeight()/2);
//        b = q(c);
//        img->setColor(x,img->getHeight()/2-1,c);
//        for (int y=img->getHeight()/2; y>=xres; y-=xres) {
//            if (abs(q((img->getColor(x,y))) - b) > thresh) {
//                c = img->getColor(x,y);
//                b = q(c);
//            }
//            for(int zz=0;zz<xres;zz++)
//                sortImage.setColor(x,y+zz,c);
//        }
//    }
//    sortImage.update();
//    return &sortImage;
//}
//
////not optimized
////void FX::sortBrightXCenter(ofImage *img, float thresh) {
////    ofColor c;
////    float b;
////
////    float opt = 0.5; //optimization factor
////    int xres = 1 + opt*2;
////
////        for (int y=0; y<img->getHeight(); y++) {
////        c = img->getColor(img->getWidth()/2,y);
////        b = q(c);
////        img->setColor(img->getWidth()/2,y,c);
////        for (int x=img->getWidth()/2; x<img->getWidth(); x+=xres) {
////            if (abs(q((img->getColor(x,y))) - b) > thresh) {
////                c = img->getColor(x,y);
////                b = q(c);
////            }
////            for(int zz=0;zz<xres;zz++)
////                img->setColor(x+zz,y,c);
////        }
////        c = img->getColor(img->getWidth()/2,y);
////        b = q(c);
////        img->setColor(img->getWidth()/2-1,y,c);
////        for (int x=img->getWidth()/2; x>=0; x-=xres) {
////            if (abs(q((img->getColor(x,y))) - b) > thresh) {
////                c = img->getColor(x,y);
////                b = q(c);
////            }
////            for(int zz=0;zz<xres;zz++)
////                img->setColor(x-zz,y,c);
////        }
////    }
////    img->update();
////}
//
//
//ofFbo* FX::getPixelsHImage(ofFbo *img, float thresh, bool goofy) {
//    if(brightY.getWidth()==0)
//        brightY.allocate(WIDTH,HEIGHT2, OF_IMAGE_GRAYSCALE);
//    ofColor c;
//    float b;
//    
//    ofFloatPixels pix;
//    img->readToPixels(pix);
//    
//    for (int y=0; y<HEIGHT; y++) {
//        char cc = pix[y*WIDTH + WIDTH/2];
//        ofColor c = cc;
//        b = q(c);
//        int xx = WIDTH/2;
//        for (int x=WIDTH/2; x<WIDTH; x+=resolution) {
//            char ccc = pix[y*WIDTH + x];
//            
//            if (abs(q(ccc) - b) > thresh) {
//                c = ccc;
//                b = q(c);
//                xx = x + (goofy?1:0);
//            }
//            
//            for(int i=0;i<resolution;i++)
//                brightY.setColor(x-i,y,ofColor(xx*255/WIDTH));
//        }
//        cc = pix[y*WIDTH + WIDTH/2];
//        c = cc;
//        b = q(c);
//        xx = WIDTH/2;
//        for (int x=WIDTH/2; x>=0; x-=resolution) {
//            char ccc = pix[y*WIDTH + x];
//            
//            if (abs(q(ccc) - b) > thresh) {
//                c = ccc;
//                b = q(c);
//                xx = x - resolution - (goofy?1:0);
//            }
//            
//            for(int i=0;i<resolution;i++)
//                brightY.setColor(x+i,y,ofColor(xx*255/WIDTH));
//        }
//    }
//    //brightY.update();
//    
//    //img->setFromPixels(&pix);
//    
//    return img;
//}
//
//ofImage* FX::getPixelsRevHImage(ofImage *img, float thresh, bool goofy) {
//    if(brightY.getWidth()==0)
//        brightY.allocate(WIDTH,HEIGHT2, OF_IMAGE_GRAYSCALE);
//    ofColor c;
//    float b;
//    
//    for (int y=0; y<HEIGHT2; y++) {
//        int xx = 0;
//        c = img->getColor(xx,y);
//        b = q(c);
//        for (int x=0; x<WIDTH/2; x+=resolution) {
//            ofColor c2 = img->getColor(x,y);
//            
//            if (abs(q(c2) - b) > thresh) {
//                c = c2;
//                b = q(c);
//                xx = x - (goofy?1:0);
//            }
//            
//            for(int i=0;i<resolution;i++)
//                brightY.setColor(x+i,y,ofColor(xx*255/WIDTH));
//        }
//        xx = WIDTH-1;
//        c = img->getColor(xx,y);
//        b = q(c);
//        for (int x=WIDTH-1; x>=WIDTH/2; x-=resolution) {
//            ofColor c2 = img->getColor(x,y);
//            if (abs(q(c2) - b) > thresh) {
//                c = c2;
//                b = q(c);
//                xx = x + resolution + 1 + (goofy?1:0);
//            }
//            
//            for(int i=0;i<resolution;i++)
//                brightY.setColor(x-i,y,ofColor(xx*255/WIDTH));
//        }
//    }
//    brightY.update();
//    return &brightY;
//}
//
//ofImage* FX::getPixelsVImage(ofImage *img, float thresh, bool goofy) {
//    if(brightY.getWidth()==0)
//        brightY.allocate(WIDTH,HEIGHT2, OF_IMAGE_GRAYSCALE);
//    ofColor c;
//    float b;
//    
//    for (int x=0; x<WIDTH; x++) {
//        c = img->getColor(x,HEIGHT2/2);
//        b = q(c);
//        int yy = WIDTH/2;
//        for (int y=HEIGHT2/2; y<HEIGHT2; y+=resolution) {
//            ofColor c2 = img->getColor(x,y);
//            
//            if (abs(q(c2) - b) > thresh) {
//                c = c2;
//                b = q(c);
//                yy = y  - (goofy?1:0);;
//            }
//            
//            for(int i=0;i<resolution;i++)
//                brightY.setColor(x,y-i,ofColor(yy*255/HEIGHT));
//        }
//        c = img->getColor(x, HEIGHT2/2);
//        b = q(c);
//        yy = HEIGHT2/2;
//        for (int y=HEIGHT2/2; y>=0; y-=resolution) {
//            ofColor c2 = img->getColor(x,y);
//            if (abs(q(c2) - b) > thresh) {
//                c = c2;
//                b = q(c);
//                yy = y + 1 + resolution + (goofy?1:0);;
//            }
//            
//            for(int i=0;i<resolution;i++)
//                brightY.setColor(x,y+i,ofColor(yy*255/img->getHeight()));
//        }
//    }
//    brightY.update();
//    return &brightY;
//}
//
//
///*void FX::sortBrightYCenter(ofImage *img, float thresh) {
// ofColor c;
// float b;
// for (int x=0; x<WIDTH; x++) {
// c = img->getColor(x,HEIGHT/2);
// b = q(&c);
// img->setColor(x,HEIGHT/2,c);
// for (int y=HEIGHT/2-1; y>=0; y--) {
// if (abs(q(&(img->getColor(x,y))) - b) > thresh) {
// c = img->getColor(x,y);
// b = q(&c);
// }
// img->setColor(x,y,c);
// }
// c = img->getColor(x,HEIGHT/2);
// b = q(&c);
// img->setColor(x,HEIGHT/2,c);
// for (int y=HEIGHT/2; y<HEIGHT; y++) {
// if (abs(q(&(img->getColor(x,y))) - b) > thresh) {
// c = img->getColor(x,y);
// b = q(&c);
// }
// img->setColor(x,y,c);
// }
// }
// img->update();
// }*/
//
////void FX::sortPixels(ofImage *img){
////    std::vector<unsigned char> p;
////    unsigned char* pix = img->getPixels();
////    for(int i=0;i<img->getWidth()*img->height;i++)
////        p.push_back(pix[i]);
////    //ofSort(p);
////    int n = img->getWidth()*img->getHeight();
////    for(int i=0;i<n-1;i++){
////        bool swap = false;
////        if(p[i] > p[i+1]){
////            unsigned char temp = p[i+1];
////            p[i+1] = p[i];
////            p[i] = temp;
////            swap = true;
////        }
////    }
////    for(int x=0;x<img->getWidth();x++)
////        for(int y=0;y<img->getHeight();y++)
////            img->setColor(x,y,p[x+y*img->getWidth()]);
////}
//
//void FX::loadFXShaders(){
//    alphaShader.load("../shaders/alpha");
//    skewShader.load("../shaders/skew");
//    skewShader_sel.load("../shaders/skew_selective");
//    //    skewShader.load("../shaders_circle/skew");
//    displaceShader_sel.load("../shaders/displace_selective");
//    displaceShader.load("../shaders/displace");
//    //    displaceShader.load("../shaders_circle/displace");
//    noiseShader.load("../shaders/noise");
//    pixelShader.load("../shaders/pixellate");
//    //    pixelShader.load("../shaders_circle/pixellate");
//    sobelShader.load("../shaders/sobel");
//    //    sobelShader.load("../shaders_circle/sobel");
//    sobelHDShader.load("../shaders/sobelHD");
//    gBlurHor.load("../shaders/gblurHD");
//    hdrShader.load("../shaders/gamma");
//    sInvert.load("../shaders/invert");
//    kShader.load("../shaders/kalei");
//    //    kShader.load("../shaders_circle/kalei");
//    kShader.load("../shaders/kalei");
//    chromaShader.load("../shaders/chromaSep");
//    chromaSepShader.load("../shaders/chromaSepOnly");
//    circleShader.load("../shaders/circle");
//    circleShader2.load("../shaders/circle2");
//    displaceGlitchShader.load("../shaders/displaceGlitch");
//    lsdShader.load("../shaders/lsd");
//    triumShader.load("../shaders/trium");
//    //    perturbationShader.load("../shaders/perturbation");
//    perturbationShader.load("../shaders/perturbation_old");
//    pnoiseShader.load("../shaders/pnoise");
//    dualShader.load("../shaders/dual");
//    tilesShader.load("../shaders/tiles");
//}
//
//void FX::updateResolution(){
//    /** DATA VECTORS **/
//    free(skewVector);
//    skewVector = (float*) malloc(app->crt_WIDTH*sizeof(float));
//    
//    free(displaceVector);
//    displaceVector = (float*) malloc(app->crt_WIDTH*sizeof(float));
//}
//
//void FX::initGlitches(AbstractApp* a){
//    app = a;
//    
//    sphereMesh = ofMesh::sphere(HEIGHT/2.25, 100);
//    
//    for(int i=0;i<sphereMesh.getNumTexCoords();i++){
//        if(sphereMesh.getVertex(i).z<0)
//            sphereMesh.setTexCoord(i, (sphereMesh.getTexCoord(i)*ofVec3f(2, 2*HEIGHT/(float)WIDTH) - ofVec2f(0.5, 0.)));
//        else
//            sphereMesh.setTexCoord(i, (sphereMesh.getTexCoord(i)*ofVec3f(-2, 2*HEIGHT/(float)WIDTH) + ofVec2f(-0.5, 0.)));
//    }
//    for(int i=0;i<sphereMesh.getNumVertices();i++){
//        ofVec3f v = sphereMesh.getVertex(i);
//        if(v.z < 0 )
//            v.z *= -0.99;
//        sphereMesh.setVertex(i, v);
//    }
//    
//    loadFXShaders();
//    
//    //    tunnelFbo.allocate(WIDTH,src->getHeight());
//    tmpFbo.allocate(app->render_WIDTH,app->render_HEIGHT);
//    tmpFbo2.allocate(app->render_WIDTH,app->render_HEIGHT);
//    sortImage.allocate(app->render_WIDTH, app->render_HEIGHT, OF_IMAGE_COLOR_ALPHA);
//    
//    
//    for(int i=0;i<MAX_OMG3D2_FBOS;i++){
//        ofFbo* fbo = new ofFbo;
//        fbo->allocate(WIDTH, HEIGHT);
//        zsFbos.push_back(fbo);
//    }
//    
//    
//    /** DATA VECTORS **/
//    updateResolution();
//    updateSkew(skewVector);
//    
//    for(int i=0;i<2;i++){
//        std::vector<ofVec4f> z;
//        zs.push_back(z);
//    }
//    
//    if(dancer){
//        dancerPlayer.loadMovie("./assets/Dancer.mov");
//        dancerPlayer.setLoopState(OF_LOOP_NORMAL);
//        dancerPlayer.play();
//    }
//}
//
//
//
//void FX::glitchBuffer(string out_path, ofBuffer* buf){
//    char* data = buf->getBinaryBuffer();
//    
//    //    for(int i=0;i<buf->size()-2;i++){
//    //        if(data[i] == '\xFF' && data[i+1] == '\xDB'){
//    //            int index = i + app->parameterMap[jpgGlitch];
//    //            float size = data[index];
//    //            size += ofRandom(255);
//    //            data[index] = size;
//    //            break;
//    //        }
//    //    }
//    
//    float nb = int((app->parameterMap[jpgGlitch]));
//    for(int i=0;i<=nb;i++){
//        int index = int(ofRandom(buf->size()*5/100.0, buf->size()*100/100.0));
//        data[index] = ofRandom(255);
//    }
//    
//    buf->set(data, buf->size());
//    
//    //        ofPixels pixels;
//    //        source->getTextureReference().readToPixels(pixels);
//    //        ofSaveImage(pixels, buf);
//    
//    
//    
//    //        glitchFBO.begin();
//    //        ofPushMatrix();
//    //        ofRotateZ(90);
//    //        source->draw(0,0);
//    //        ofPopMatrix();
//    //        glitchFBO.end();
//    
//    
//    ofBufferToFile(out_path, *buf);
//}
//
//void FX::glitchBuffer(ofBuffer* buf){
//    char* data = buf->getBinaryBuffer();
//    
//    float nb = int((abs(app->parameterMap[jpgGlitch])));
//    for(int i=0;i<=nb;i++){
//        int index = int(ofRandom(buf->size()*5/100.0, buf->size()*100/100.0));
//        //        int index = int((buf->size()*(i+1)/float(nb+2)));
//        data[index] = ofRandom(255);
//    }
//    
//    buf->set(data, buf->size());
//}
//
//
//void FX::glitchImage(string out_path, string source){
//    DEBUG_TIME_1
//    ofBuffer buf = ofBufferFromFile(source);
//    glitchBuffer(out_path, &buf);
//    DEBUG_TIME_2
//}
//
////void FX::glitchImage(string path, ofFbo* source){
////    ofBuffer buf;
////    ofFile file( path );
////    buf.set(source->getTextureReference().get ) ;
////
////    glitchBuffer(path, buf);
////}
//
