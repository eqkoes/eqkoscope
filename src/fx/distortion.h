#include <ofGraphics.h>
#include <ofFbo.h>
#include <ofGLRenderer.h>
#include "stdlib.h"
#include "math.h"
#include "utils.h"

#ifndef emptyExample_distortion_h
#define emptyExample_distortion_h

using namespace std;

static ofEasyCam cam;
static ofMesh sphereMesh;
static ofShader triumShader,alphaShader, skewShader, displaceShader, pixelShader, noiseShader,sobelShader, gBlurHor, hdrShader, sInvert, kShader, chromaShader,circleShader, displaceGlitchShader, lsdShader, perturbationShader,dualShader;
static ofFbo tunnelFbo;
static ofImage iii;

static AbstractApp* app;

static std::vector<std::vector<ofVec4f> > zs;
static std::vector<ofFbo*> zsFbos;
static std::vector<bool> zsFbosOk;
static int MAX_OMG3D2_FBOS = 25;


static ofImage sortImage;


static void sphere(ofFbo* src, ofFbo* dest){
    dest->begin();
    ofPushMatrix();
    
    ofBackground(0);
    ofSetColor(ofColor::white);
    ofNoFill();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    
    ofDisableArbTex();
    ofEnableNormalizedTexCoords();
    glEnable(GL_DEPTH_TEST);
    
    src->getTextureReference().bind();
    ofTranslate(WIDTH/2, HEIGHT/2, 0);
    ofRotateY(ofGetFrameNum()*2);
    //    ofRotateX(-45);
    
    sphereMesh.draw();
    
    src->getTextureReference().unbind();
    glDisable(GL_DEPTH_TEST);
    ofDisableNormalizedTexCoords();
    ofEnableArbTex();
    
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    
    ofPopMatrix();
    dest->end();
}




static void displayOmg3D2(ofFbo* src, ofFbo* dest, bool closeEasing, float speed, bool pastFbos,
                          float rotation, float internalDist, bool symetry, bool freeRotation,
                          float addedRotation, int o_index){

    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    
    bool x2 = app->parameterMap[omg3D2x2];
    float nb = min((float)MAX_OMG3D2_FBOS, app->parameterMap[omg3D2Nb]);
    if(x2)
        nb /= 2;
    nb = (int) nb;
    int MINDIST = 800;
    int MAXDIST = 4000*internalDist;
    
    
    if(zs[o_index].size() != (int) nb){ //init //TODO reset zs on macro load ?
        zs[o_index].clear();
        for(int x=0; x<nb;x++){
            zs[o_index].push_back(ofVec4f((x==0 || zs[o_index][x-1].x > 0) ? ofRandom(-WIDTH/4, 0) : ofRandom(0, WIDTH/4)
                                 ,ofRandom(-WIDTH/4, WIDTH/4)
                                 , x/nb
                         ,(x%2)==0));
            zsFbosOk.push_back(false);
        }
    }
    
    ofPushMatrix();{
        float a = 60 ;
        float l = 0.25;
        int firstIndex = 0;
        float maxZ = 0;
        
#ifdef REAL_TIME_FX
        speed *= app->restrictFrameRate/ofGetFrameRate();
#endif
        
        for(int x=0; x<zs[o_index].size();x++){ //updates
            zs[o_index][x].z += speed;
            if(pastFbos){
                if(zs[o_index][x].z<0 || zs[o_index][x].z>1){
                    zsFbos[x]->begin();
                    ofBackground(0);
                    ofSetColor(ofColor::white);
                    src->draw(0, - (HEIGHT2-HEIGHT)/2);
                    zsFbos[x]->end();
                }
                for(int index=0;index<zsFbos.size();index++) //force init
                    if(!zsFbosOk[index]){
                        zsFbos[index]->begin();
                        ofBackground(0);
                        ofSetColor(ofColor::white);
                        src->draw(0, - (HEIGHT2-HEIGHT)/2);
                        zsFbos[index]->end();
                        zsFbosOk[index] = true;
                    }
            }
            
            bool reset = false;
            if(zs[o_index][x].z<0){
                zs[o_index][x].z = 1;
                reset = true;
            }
            if(zs[o_index][x].z>1){
                zs[o_index][x].z = 0 ;
                reset = true;
            }
            
            if(reset){
                zs[o_index][x].x = (x==0 || zs[o_index][x-1].x > 0) ? ofRandom(-WIDTH/4, 0) : ofRandom(0, WIDTH/4);
                zs[o_index][x].y = ofRandom(-WIDTH/4, WIDTH/4);
            }
            
            if(zs[o_index][x].z > maxZ){
                maxZ = zs[o_index][x].z;
                firstIndex = x;
            }
        }
        
        dest->begin();
        
#ifdef GAME
        cam.setPosition(ofPoint(WIDTH/2 + app->xOffset*5,HEIGHT2/2 - app->yOffset*5,-500));
        cam.setOrientation((ofVec3f((ofGetMouseY()-HEIGHT/2)/3.0, -(ofGetMouseX()-WIDTH/2)/3.0, 0)));
        cam.begin();
#endif
//        ofBackground(0);
        ofTranslate(app->FINALWIDTH/2, HEIGHT2/2);
        ofRotateY(addedRotation);
#ifdef GAME
        //        ofSetColor(255,0,0,150);
        //        ofPushMatrix();
        //        ofTranslate(0, 0, -515);
        //        ofRect(-FINALWIDTH/2, -HEIGHT2/2 + (HEIGHT2-HEIGHT)/2, WIDTH, HEIGHT2);
        //        ofRect(-1000000, -1000000, -1000000, -1000000);
        //        ofPopMatrix();
#endif
        
        int i;
        float rot = 0;
        float intensity = app->parameterMap[omg3D2];
        for (int index=0; index<zs[o_index].size();index++){
            int iindex = index;
            for(int add=0;add<= x2 ? 1 : 0;add++){
            if(add==1)
                iindex = zs[o_index].size() - index;
            
            if(app->parameterMap[omg3D2ZScale]>=0)
                i = (iindex + firstIndex) % zs[o_index].size();
            else
                i = zs[o_index].size() - 1 - ((iindex - firstIndex) % zs[o_index].size());
            //            for(int xxxx=0;xxxx<addZLen;xxxx+=addZres){ //rotate img edges if z is close . check .vert
            
            ofPushMatrix();
            {
            float x = zs[o_index][i].x * app->parameterMap[divergence] * 10;
            float y = zs[o_index][i].y * app->parameterMap[yDivergence] * 10;

            float ac = app->parameterMap[omg3D2AvoidCenter];
            if(ac>0 && abs(x)<ac*HEIGHT)
                x = ac*HEIGHT * x/abs(x);
            if(ac<0 && abs(x)>ac*HEIGHT)
                x = -ac*HEIGHT * x/abs(x);
            if(ac>0 && abs(y)<ac*HEIGHT)
                y = ac*HEIGHT * y/abs(y);
            if(ac<0 && abs(y)>ac*HEIGHT)
                y = -ac*HEIGHT * y/abs(y);
            
            float z = zs[o_index][i].z;
                if(add==1)
                    z = 1 - z;
             z = (z*(MINDIST + MAXDIST) - MAXDIST - MINDIST*(1-app->parameterMap[omg3D2Depth])) * intensity;
                
                
            if(app->parameterMap[omg3D2X]){
                x +=  (zs[o_index][i].z-0.5)*WIDTH*nb/2.5*intensity;
            }
            if(app->parameterMap[omg3D2Y]){
                y +=  (zs[o_index][i].z-0.5)*HEIGHT*nb/2.5*intensity;
            }

            ofTranslate(x, y, z);
            
            if(app->parameterMap[omg3D2RollerX] || app->parameterMap[omg3D2RollerY]){
                float zzz = (0.8-zs[o_index][i].z);
                zzz *= zzz;
                ofTranslate(zzz*app->parameterMap[omg3D2RollerX]*HEIGHT,
                            zzz*app->parameterMap[omg3D2RollerY]*HEIGHT);
            }
            
            if(app->parameterMap[omg3D2HardRotation]==0)
                rot = zs[o_index][i].z * rotation-1;
            else
                rot = rotation*i;
            if(freeRotation)
                rot += ofGetFrameNum();
            ofRotateZ(rot);
            ofTranslate(0, app->parameterMap[aDivergence]*HEIGHT/2);
            
            if(app->parameterMap[omg3D2ZScale]!=00)
            ofScale(1+abs(app->parameterMap[omg3D2ZScale])*(1-zs[o_index][i].z),
                    1+abs(app->parameterMap[omg3D2ZScale])*(1-zs[o_index][i].z));
            
            float alpha_ = 1;
            if(app->parameterMap[omg3D2Alpha0])
                alpha_ *= ofMap(zs[o_index][i].z, 0.85, 0.92 + (0.08 * (1-app->parameterMap[omg3D2Depth])), 1, min(1.0f, max(0.0f, 1-app->parameterMap[omg3D2Alpha0])), true); //TODO add maxdist/mindist so that the algo is ok with different z scales
            float aaa = app->parameterMap[omg3D2AlphaZ];
            if(aaa)
                alpha_ = alpha_*ofMap(zs[o_index][i].z, 0, 0.8, (1-aaa), 1, true);
            if(closeEasing)
                alpha_ = alpha_*ofMap(zs[o_index][i].z, 0.2, 0, 1, 0, true);
            
            if(app->parameterMap[omg3D2LumaKey]){
                alphaShader.begin();
                alphaShader.setUniform1f("alphaOffset", alpha_);
                if(app->parameterMap[omg3D2Strobe])
                    alphaShader.setUniform1f("invert", zs[o_index][i].w==1);
            }
            ofSetColor(255*alpha_,255*alpha_,255,alpha_*255);//psychedelia !
            if(pastFbos){
                zsFbos[i]->draw(-app->FINALWIDTH/2, -HEIGHT2/2 + (HEIGHT2-HEIGHT)/2);
            }else{
                if((x<-1 && symetry) || (app->parameterMap[divergence]<=0.01 && symetry && i%2==0))
                    src->draw(-app->FINALWIDTH/2+app->FINALWIDTH, -HEIGHT2/2, -app->FINALWIDTH, HEIGHT2); //invert image
                else
                    src->draw(-app->FINALWIDTH/2, -HEIGHT2/2);
            }
            if(app->parameterMap[omg3D2LumaKey])
                alphaShader.end();
            }ofPopMatrix();
                        }
//        }
        }
        
        dest->end();
    }ofPopMatrix();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
}

static void tunnel(ofFbo* src, ofFbo* dest,  bool closeEasing, float speed, bool pastFbos,
                   float rotation, float internalDist, bool symetry, bool freeRotation){
    
//    dualShader.load("../shaders/dual");

    float a = -7;
    
    ofPushMatrix();
//    ofRotateY(a);
    displayOmg3D2(src, &tunnelFbo, closeEasing, speed, pastFbos, rotation, internalDist, symetry, freeRotation, a, 0);
    ofPopMatrix();
    
    dest->begin();
    dualShader.begin();
    dualShader.setUniform1f("left", 1);
    dualShader.setUniform1f("WIDTH", WIDTH);
    tunnelFbo.draw(0,0);
    dualShader.end();
    dest->end();

    ofPushMatrix();
    ofRotateY(-a);
    displayOmg3D2(src, &tunnelFbo, closeEasing, speed, pastFbos, rotation, internalDist, symetry, freeRotation, -a, 0);
    ofPopMatrix();
    
    dest->begin();
    dualShader.begin();
    dualShader.setUniform1f("left", 0);
    dualShader.setUniform1f("WIDTH", WIDTH);
    tunnelFbo.draw(0,0);
    dualShader.end();
    dest->end();
    
}

static void displayOmg3D(ofFbo* src, ofFbo* dest){
    ofEnableBlendMode(OF_BLENDMODE_SCREEN);
    
    float intensity = app->parameterMap[omg3D];
    float aa = app->parameterMap[omg3DAngle]*intensity;
    int W = app->FINALWIDTH;
    
    dest->begin();
    ofBackground(0);
    alphaShader.begin();
    alphaShader.setUniform1f("alphaOffset", intensity);
    ofPushMatrix();
    {
        ofTranslate(app->FINALWIDTH/2, app->FINALHEIGHT/2 + (HEIGHT2-HEIGHT)/2);
        float a = aa ;
        float l = 0.25*intensity;
        
        ofPushMatrix();
        ofTranslate(-app->FINALWIDTH*l, 0);
        ofRotateY(a);
        src->draw(-app->FINALWIDTH/2, -HEIGHT2/2, app->FINALWIDTH, HEIGHT2);
        ofPopMatrix();
        
        ofPushMatrix();
        ofTranslate(app->FINALWIDTH*l, 0);
        ofRotateY(-a);
        src->draw(-app->FINALWIDTH/2+app->FINALWIDTH, -HEIGHT2/2, -app->FINALWIDTH, HEIGHT2);
        ofPopMatrix();
        
        ofPushMatrix();
        ofTranslate(0, app->FINALHEIGHT*l);
        ofRotateX(a);
        src->draw(-app->FINALWIDTH/2, -HEIGHT2/2, app->FINALWIDTH, HEIGHT2);
        ofPopMatrix();
        
        ofPushMatrix();
        ofTranslate(0, -app->FINALHEIGHT*l);
        ofRotateX(-a);
        src->draw(-app->FINALWIDTH/2, -HEIGHT2/2 + HEIGHT2, app->FINALWIDTH, -HEIGHT2);
        ofPopMatrix();
    }
    ofPopMatrix();
    alphaShader.end();
    dest->end();
    
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
}


static void stripe(float size) {
    size *= HEIGHT/2;
    
    if (app->parameterMap[tintSaturation]>0)
        ofSetColor(ofColor::fromHsb(255*app->parameterMap[tintHue], 255*app->parameterMap[tintSaturation], ofRandom(255)));
    else
        ofSetColor(ofColor::fromHsb(ofRandom(255), ofRandom(200), ofRandom(255)));
    
    float h = ofRandom(size/2, size);
    float y = ofRandom(HEIGHT2-size);
    ofRect(0, y-h/2, WIDTH, h);
}

static void doInvert(ofFbo* src, ofFbo* dest){
    //sInvert.load("../shaders/invert");
    dest->begin();
    sInvert.begin();
    sInvert.setUniform1f("WIDTH", WIDTH);
    sInvert.setUniform1f("HEIGHT", HEIGHT);
    sInvert.setUniform1f("circle", app->parameterMap[invertCircle] * (app->parameterMap[_invert] ? -1 : 1));
    sInvert.setUniform1i("mode", 1);
    src->draw(0,0);
    sInvert.end();
    dest->end();
}

static void doStrobe(ofFbo* src, ofFbo* dest){
    //sInvert.load("../shaders/invert");
    dest->begin();
    sInvert.begin();
    sInvert.setUniform1i("mode", app->parameterMap[strobe]);
    src->draw(0,0);
    sInvert.end();
    dest->end();
}

static void doPerturbation(ofFbo* src, ofFbo* dest){
//   perturbationShader.load("../shaders/perturbation");
    dest->begin();
    perturbationShader.begin();
    perturbationShader.setUniform1f("WIDTH", WIDTH);
    perturbationShader.setUniform1f("HEIGHT", HEIGHT);
    perturbationShader.setUniform1f("intensity", app->parameterMap[pert]);
    perturbationShader.setUniform1f("evolution", app->parameterMap[pertEvo]);
    perturbationShader.setUniform1f("freq", app->parameterMap[pertFreq]);
    perturbationShader.setUniform1f("persistance", app->parameterMap[pertPersistance]);
    perturbationShader.setUniform1i("mode", (int) app->parameterMap[pertMode]);
    src->draw(0,0);
    perturbationShader.end();
    dest->end();
    
}


static void doLSD(ofFbo* src, ofFbo* dest){
    dest->begin();
    lsdShader.begin();
    lsdShader.setUniform1f("intensity", app->parameterMap[warp]);
    lsdShader.setUniform1f("remap", app->parameterMap[warpRemap]);
    lsdShader.setUniform1f("cx", app->parameterMap[warpX]);
    lsdShader.setUniform1f("cy", app->parameterMap[warpY]);
    lsdShader.setUniform1f("WIDTH", WIDTH);
    lsdShader.setUniform1f("HEIGHT", HEIGHT);
    src->draw(0,0);
    lsdShader.end();
    dest->end();
}

static void doToCircle(ofFbo* src, ofFbo* dest, float amt){
    //    circleShader.load("../shaders/circle");
    
    dest->begin();
    circleShader.begin();
    circleShader.setUniform1f("WIDTH", WIDTH);
    circleShader.setUniform1f("HEIGHT", HEIGHT2);
    circleShader.setUniform1f("amount", amt);
    circleShader.setUniform1f("minRad", 0.2);
    circleShader.setUniform1f("maxRad", 1);
    ofBackground(0);
    src->draw(0,0);
    circleShader.end();
    dest->end();
}

static void pixellate(ofFbo* src, ofFbo* dest, float x, float y){
    dest->begin();
    pixelShader.begin();
    pixelShader.setUniform1f("WIDTH", WIDTH);
    pixelShader.setUniform1f("HEIGHT", HEIGHT2);
    pixelShader.setUniform1i("xresolution", (int)x);
    pixelShader.setUniform1i("yresolution", (int)y);
    //    pixelShader.setUniform1i("xfocus", ofGetMouseX());
    //    pixelShader.setUniform1i("yfocus", HEIGHT-ofGetMouseY());
    src->draw(0,0);
    pixelShader.end();
    dest->end();
}

static void sobelContours(ofFbo* src, ofFbo* dest){
    dest->begin();
//  sobelShader.load("../shaders/sobel");
    sobelShader.begin();
//    sobelShader.setUniform1i("fast", 1);
    sobelShader.setUniform1f("intensity", app->parameterMap[sobel]);
    sobelShader.setUniform1f("mix_", app->parameterMap[sobelMix]);
    //    sobelShader.setUniform1i("hueLevels", ofGetMouseX()/10);
    //    sobelShader.setUniform1i("lumLevels", ofGetMouseY()/10);
    src->draw(0,0);
    sobelShader.end();
    dest->end();
}

static void chromaSeparation(ofFbo* src, ofFbo* dest, bool invert, float chromaSep, float chromasepHue, float chromasepAngle, float chromaOffset, float tintAmp, float reTint, float sidesSaturation){
//        chromaShader.load("../shaders/chromaSep"); //dbg
    
    dest->begin();
    chromaShader.begin();
    chromaShader.setUniform1f("WIDTH", WIDTH);
    chromaShader.setUniform1f("HEIGHT", HEIGHT2);
    chromaShader.setUniform1f("intensity", chromaSep);
    chromaShader.setUniform1f("sepAlpha", app->parameterMap[chromaSepAlpha]);
    float csHue = chromasepHue;
    if(tintAmp<1)
        csHue = ofMap(chromasepHue/255-app->parameterMap[tintHue], 0, 1, app->parameterMap[tintCenter] - app->parameterMap[int(tintAmp)]/2, app->parameterMap[tintCenter] + app->parameterMap[int(tintAmp)]/2)*255;
    chromaShader.setUniform1f("chromasepHue", csHue);
    chromaShader.setUniform1f("chromaOffset", min(chromaOffset, tintAmp*255/2));
    chromaShader.setUniform1f("chromasepAngle", chromasepAngle);
    chromaShader.setUniform1f("chromaSepMode", app->parameterMap[chromaSepMode]);
    chromaShader.setUniform1f("hue", app->parameterMap[tintHue]);
    chromaShader.setUniform1f("reSaturate", app->parameterMap[reSaturate]);
    //chromaShader.setUniform1f("saturation", app->parameterMap[tintSaturation]*200 /255.0);
    chromaShader.setUniform1f("sidesSaturation", sidesSaturation);
    chromaShader.setUniform1f("reTint", app->parameterMap[_reTint]);
//    chromaShader.setUniform1f("invert", invert);
    src->draw(0,0);
    chromaShader.end();
    dest->end();
}

static void blur(ofFbo* src, ofFbo* dest, float intensity, float mix, int resolution, bool vertical){
    //gBlurHor.load("../shaders/gblurHD");

    dest->begin();
    gBlurHor.begin();
    gBlurHor.setUniform1f("vertical", vertical?1:0);
    gBlurHor.setUniform1i("resolution", 25);
    gBlurHor.setUniform1f("blurDistance", intensity);
    gBlurHor.setUniform1f("originalMix", mix);
    gBlurHor.setUniform1f("brightness", 1);
    src->draw(0,0);
    gBlurHor.end();
    dest->end();
}

static void blurBegin(float intensity, float mix, int resolution, ofFbo* a){
    a->begin();
    gBlurHor.begin();
    gBlurHor.setUniform1f("vertical", 0);
    gBlurHor.setUniform1i("resolution", 25);
    gBlurHor.setUniform1f("blurDistance", intensity);
    gBlurHor.setUniform1f("totalMix", mix);
    gBlurHor.setUniform1f("brightness", 1);
}

static void blurEnd(float intensity, float mix, int resolution, ofFbo* a){
    gBlurHor.end();
    a->end();
    
    gBlurHor.begin();
    gBlurHor.setUniform1f("vertical", 1);
    gBlurHor.setUniform1i("resolution", 25);
    gBlurHor.setUniform1f("blurDistance", intensity);
    gBlurHor.setUniform1f("totalMix", mix);
    gBlurHor.setUniform1f("brightness", 1);
    a->draw(0,0);
    gBlurHor.end();
}

static void doGamma(ofFbo* src, ofFbo* dest, float gamma){
    dest->begin();
    hdrShader.begin();
    hdrShader.setUniform1f("contrast", app->parameterMap[contrast]);
    hdrShader.setUniform1f("gamma", gamma);
    src->draw(0,0);
    hdrShader.end();
    dest->end();
}

static void doKalei(ofFbo* src, ofFbo* dest, float kaleiNb, float kaleiOffX, float kaleiOffY){
//    kShader.load("../shaders/kalei");
    dest->begin();
    kShader.begin();
    kShader.setUniform1f("WIDTH", (float) WIDTH);
    kShader.setUniform1f("HEIGHT", (float) HEIGHT2);
    kShader.setUniform1f("sides", (int) kaleiNb);
    kShader.setUniform1f("x", (float )kaleiOffX*WIDTH);
    kShader.setUniform1f("y", (float )kaleiOffY*HEIGHT2);
    kShader.setUniform1f("copy", (float )app->parameterMap[kaleiCopy]);
    kShader.setUniform1f("copyAdjust", (float )app->parameterMap[kaleiCopyAdjust]);
    kShader.setUniform1f("copyBorder", (float )app->parameterMap[kaleiCopyBorder]);
    kShader.setUniform1f("mirror", (float )app->parameterMap[kaleiMirror]);
    kShader.setUniform1f("scale", (float )app->parameterMap[kaleiScale]);
    kShader.setUniform1f("t", (float ) ofGetFrameNum());
    src->draw(0,0);
    kShader.end();
    dest->end();
}

static void doGlow(ofFbo* src, ofFbo* dest, ofFbo* tempFbo,  ofFbo* tempFbo2, float amount, float intensity, float resolution){
    //lag not in the shader but tempFbo writing
    //    tempFbo->clear();
    
    tempFbo->begin(); //6 IPS
    ofSetColor(ofColor::black);
    ofRect(0,0,WIDTH,HEIGHT);
    ofSetColor(ofColor::white);
    gBlurHor.begin();
    gBlurHor.setUniform1f("vertical", 0);
    gBlurHor.setUniform1i("resolution", resolution);
    gBlurHor.setUniform1f("blurDistance", 2);
    gBlurHor.setUniform1f("brightness", intensity);
    gBlurHor.setUniform1f("originalMix", 0);
    src->draw(0,0);
    gBlurHor.end();
    tempFbo->end();
    
    tempFbo2->begin(); //6IPS
    ofSetColor(ofColor::black);
    ofRect(0,0,WIDTH,HEIGHT);
    ofSetColor(ofColor::white);
    gBlurHor.begin();
    gBlurHor.setUniform1f("vertical", 1);
    gBlurHor.setUniform1i("resolution", resolution);
    gBlurHor.setUniform1f("blurDistance", 2);
    gBlurHor.setUniform1f("brightness", intensity);
    gBlurHor.setUniform1f("originalMix", 0);
    tempFbo->draw(0,0);
    gBlurHor.end();
    tempFbo2->end();
    
//    dest->begin();
//    ofSetColor(ofColor::black);
//    ofRect(0,0,WIDTH,HEIGHT);
//    ofSetColor(ofColor::white);
//    tempFbo2->draw(0,0);
//    dest->end();
}

static void randHide(ofFbo* src, ofFbo* dest){
    dest->begin();
    src->draw(0,0);
    float vv = app->parameterMap[randVHide];
    float hh = app->parameterMap[randHHide];
    ofSetColor(ofColor::black);
    if(vv>0 && hh >0){
        float y = ofRandom(HEIGHT2);
        float h = ofRandom(HEIGHT/4*vv);
        float x = ofRandom(WIDTH);
        float w = ofRandom(WIDTH/4*hh);
        ofRect(0, 0, x, y);
        ofRect(x+w, 0, WIDTH, y);
        ofRect(0, y+h, x, HEIGHT);
        ofRect(x+w, y+h, WIDTH, HEIGHT2);
    }else
    if(vv>0){
        float y = ofRandom(HEIGHT2);
        float h = ofRandom(HEIGHT/4*vv);
        ofRect(0, 0, WIDTH, y);
        ofRect(0, y+h, WIDTH, HEIGHT2);
    }else{
        float x = ofRandom(WIDTH);
        float w = ofRandom(WIDTH/4*hh);
        ofRect(0, 0, x, HEIGHT2);
        ofRect(x+w, 0, WIDTH, HEIGHT2);
    }
    ofSetColor(ofColor::white);
    dest->end();
}

/****************************************** SKEW **/

static int off = 0;

static ofPixels o; //make global

static void skewFbo(ofFbo* fbo, float amp, float offset, bool borderCopy, float* skewVector) {
    ofPixels o; //make global
    ofImage img;
    img.allocate(WIDTH, HEIGHT2,OF_IMAGE_COLOR);
    
    fbo->readToPixels(o);
    
    //o.allocate(img->width, img->height, OF_PIXELS_RGB);
    off = offset;
    if (off<0)
        off = WIDTH;
    int xx, yy;
    for (int y=0; y<HEIGHT2; y++) {
        for (int x=0; x<WIDTH; x++) {
            xx = int( x + amp*skewVector[(y+(off))%HEIGHT2]);
            if (borderCopy) {
                xx = (xx+WIDTH) % WIDTH;
            }
            xx = ofConstrain(xx, 0, WIDTH-1);
            img.setColor(x,y,o.getColor(xx,y));
        }
    }
    img.setFromPixels(o.getPixels(), WIDTH,  HEIGHT2, OF_IMAGE_COLOR);
    img.draw(0,0);
}


static ofImage skewImage;

static void updateSkew(float jumpRate, bool hard,
                       float bandingProb, float bandingJumpProb, float* skewVector){
    //    cout << "up skew " << jumpRate<< " "<< hard<<" " << bandingProb<< " " << bandingJumpProb << endl;
    float off = 0;
    float banding = 0;
    bool firstBand = true;
    for (int i=0; i<WIDTH; i++) {
        if(banding==0 && ofRandom(1)<bandingProb)
            banding = ofRandom(-50,50);
        if(banding||firstBand){
            if(ofRandom(1)<bandingJumpProb || firstBand)
                banding = banding + ofRandom(-50,50);
            firstBand = false;
        }
        
        skewVector[i] = off + ofRandom(-1, 1) + banding;
        if(hard){ //mode bourrin
            if(skewVector[i]<-0.9)
                skewVector[i] = -1;
            else if(skewVector[i]>0.9)
                skewVector[i] = 1;
            else if(skewVector[i]<0)
                skewVector[i] = -0.9;
            else if(skewVector[i]>0)
                skewVector[i] = 0.9;
        }
        off = skewVector[i];
        if (ofRandom(1)<jumpRate)
            off = 0;
    }
}

static void updateSkew(float *skewVector){
    ofSeedRandom(ofGetFrameNum());
    updateSkew(ofRandom(0.013), false, ofRandom(0.55), ofRandom(0.25), skewVector);
}

static ofImage* getSkewTextureImage(){
    return &skewImage;
}

static int getSkewOffset(){
    return off;
}

//mode 0 : hor 1: vert 2: dist, 3 : angle
static void skew(ofFbo* src, ofFbo* dest, float intensity, float offset, int mode, float* skewVector){
    
    dest->begin();
    skewShader.begin();
    skewShader.setUniform1f("WIDTH", WIDTH);
    skewShader.setUniform1f("HEIGHT", HEIGHT2);
    skewShader.setUniform1f("vertical", (mode==1)?1:0);
    skewShader.setUniform1f("dist", (mode==2)?1:0);
    skewShader.setUniform1f("angle", (mode==3)?1:0);
    skewShader.setUniform1f("offset", offset);
    skewShader.setUniform1f("intensity", intensity);
    skewShader.setUniform1fv("skew", skewVector, WIDTH);
    src->draw(0,0);
    skewShader.end();
    dest->end();
}

/***************************** DISPLACE **/

static float* displaceVector;
static float* getDisplaceVector(){
    return displaceVector;
}

static void updateDisplace(float prob) {
    unsigned char pixes[WIDTH*4];
    float offx = 0;
    for (int y=0; y<WIDTH; y++){
        if (ofRandom(1)<prob)
            offx = ofRandom(-1,1);
        displaceVector[y] = offx;
    }
}

static void doNoise(ofFbo* src, ofFbo* dest){
//    noiseShader.load("../shaders/noise");

    dest->begin();
    noiseShader.begin();
    noiseShader.setUniform1f("intensity", app->parameterMap[noise]);
    noiseShader.setUniform1f("seed", ofGetFrameNum());
    src->draw(0,0);
    noiseShader.end();
    dest->end();
}

static void displace(ofFbo* src, ofFbo* dest, float intensity, float displaceProba, bool vertical){
    dest->begin();
    updateDisplace(displaceProba);
    displaceShader.begin();
    displaceShader.setUniform1i("WIDTH", WIDTH);
    displaceShader.setUniform1i("HEIGHT", HEIGHT);
    displaceShader.setUniform1f("vertical", vertical?1:0);
    displaceShader.setUniform1f("intensity", intensity);
    displaceShader.setUniform1fv("displace", getDisplaceVector(), WIDTH);
    src->draw(0,0);
    displaceShader.end();
    dest->end();
}

static void displaceGlitch(ofFbo* src, ofFbo* dest, float intensity, float displaceProba, bool vertical){
//    displaceGlitchShader.load("../shaders/displaceGlitch");
    int n =10;
    for(int i=0;i<n;i++){
        dest->begin();
        updateDisplace(displaceProba);
        displaceGlitchShader.begin();
        displaceGlitchShader.setUniform1f("x", ofGetMouseX());
        displaceGlitchShader.setUniform1f("y", HEIGHT2 - ofGetMouseY());
        float z = 25;
        displaceGlitchShader.setUniform1f("w", 100);
        displaceGlitchShader.setUniform1f("h", 10);
        displaceGlitchShader.setUniform1f("offx", ofRandom(-50, 50));
        displaceGlitchShader.setUniform1f("offy", ofRandom(-50, 50));
        displaceGlitchShader.end();
        dest->end();
        if(i!=n-1){
            ofFbo* tmp;
            tmp = src;
            src = dest;
            dest = tmp;
        }
    }
}

static void doTrium(ofFbo* src, ofFbo* dest){
    dest->begin();
    triumShader.begin();
    triumShader.setUniform1f("WIDTH", WIDTH);
    src->draw(0,0);
    triumShader.end();
    dest->end();
    
}

/*********************** SORT **/

//pixel sorting "quality"
inline float q(ofColor c) {
    //return maaax((float)0, (c.r + c.b + c.g)/3.0 );
    return c.getBrightness();
}

//todo analyze 1/2 ?
static ofImage* sortBrightXCenter(ofImage *img, float thresh) {
    ofColor c;
    float b;
    
    float opt = 1; //optimization factor
    int xres = 1 + opt*2;
    
    int interlace = 3; //2 is better but meh
    
    int r = ofGetFrameNum() % interlace;
    
    for (int y=r; y<img->getHeight(); y+=interlace) {
        c = img->getColor(img->getWidth()/2,y);
        b = q(c);
        img->setColor(img->getWidth()/2,y,c);
        for (int x=img->getWidth()/2; x<img->getWidth(); x+=xres) {
            if (abs(q((img->getColor(x,y))) - b) > thresh) {
                c = img->getColor(x,y);
                b = q(c);
            }
            for(int zz=0;zz<xres;zz++)
                sortImage.setColor(x+zz,y,c);
        }
        c = img->getColor(img->getWidth()/2,y);
        b = q(c);
        img->setColor(img->getWidth()/2-1,y,c);
        for (int x=img->getWidth()/2; x>=0; x-=xres) {
            if (abs(q((img->getColor(x,y))) - b) > thresh) {
                c = img->getColor(x,y);
                b = q(c);
            }
            for(int zz=0;zz<xres;zz++)
                sortImage.setColor(x-zz,y,c);
        }
    }
    sortImage.update();
    return &sortImage;
}

static ofImage* sortBrightYCenter(ofImage *img, float thresh) {
    ofColor c;
    float b;
    
    float opt = 1; //optimization factor
    int xres = 1 + opt*2;
    
    int interlace = 2; //2 is better but meh
    
    int r = ofGetFrameNum() % interlace;
    
    for (int x=r; x<img->getWidth(); x+=interlace) {
        c = img->getColor(x,img->getHeight()/2);
        b = q(c);
        img->setColor(x,img->getHeight()/2,c);
        for (int y=img->getHeight()/2; y<img->getHeight(); y+=xres) {
            if (abs(q((img->getColor(x,y))) - b) > thresh) {
                c = img->getColor(x,y);
                b = q(c);
            }
            for(int zz=0;zz<xres;zz++)
                sortImage.setColor(x,y+zz,c);
        }
        c = img->getColor(x,img->getHeight()/2);
        b = q(c);
        img->setColor(x,img->getHeight()/2-1,c);
        for (int y=img->getHeight()/2; y>=0; y-=xres) {
            if (abs(q((img->getColor(x,y))) - b) > thresh) {
                c = img->getColor(x,y);
                b = q(c);
            }
            for(int zz=0;zz<xres;zz++)
                sortImage.setColor(x,y+zz,c);
        }
    }
    sortImage.update();
    return &sortImage;
}

//not optimized
//static void sortBrightXCenter(ofImage *img, float thresh) {
//    ofColor c;
//    float b;
//
//    float opt = 0.5; //optimization factor
//    int xres = 1 + opt*2;
//
//        for (int y=0; y<img->getHeight(); y++) {
//        c = img->getColor(img->getWidth()/2,y);
//        b = q(c);
//        img->setColor(img->getWidth()/2,y,c);
//        for (int x=img->getWidth()/2; x<img->getWidth(); x+=xres) {
//            if (abs(q((img->getColor(x,y))) - b) > thresh) {
//                c = img->getColor(x,y);
//                b = q(c);
//            }
//            for(int zz=0;zz<xres;zz++)
//                img->setColor(x+zz,y,c);
//        }
//        c = img->getColor(img->getWidth()/2,y);
//        b = q(c);
//        img->setColor(img->getWidth()/2-1,y,c);
//        for (int x=img->getWidth()/2; x>=0; x-=xres) {
//            if (abs(q((img->getColor(x,y))) - b) > thresh) {
//                c = img->getColor(x,y);
//                b = q(c);
//            }
//            for(int zz=0;zz<xres;zz++)
//                img->setColor(x-zz,y,c);
//        }
//    }
//    img->update();
//}

static ofImage brightY;
static int resolution = 2;
static ofFbo* getPixelsHImage(ofFbo *img, float thresh, bool goofy) {
    if(brightY.width==0)
        brightY.allocate(WIDTH,HEIGHT2, OF_IMAGE_GRAYSCALE);
    ofColor c;
    float b;
    
    ofFloatPixels pix;
    img->readToPixels(pix);
    
    for (int y=0; y<HEIGHT; y++) {
        char cc = pix[y*WIDTH + WIDTH/2];
        ofColor c = cc;
        b = q(c);
        int xx = WIDTH/2;
        for (int x=WIDTH/2; x<WIDTH; x+=resolution) {
            char ccc = pix[y*WIDTH + x];
            
            if (abs(q(ccc) - b) > thresh) {
                c = ccc;
                b = q(c);
                xx = x + (goofy?1:0);
            }
            
            for(int i=0;i<resolution;i++)
                brightY.setColor(x-i,y,ofColor(xx*255/WIDTH));
        }
        cc = pix[y*WIDTH + WIDTH/2];
        c = cc;
        b = q(c);
        xx = WIDTH/2;
        for (int x=WIDTH/2; x>=0; x-=resolution) {
            char ccc = pix[y*WIDTH + x];
            
            if (abs(q(ccc) - b) > thresh) {
                c = ccc;
                b = q(c);
                xx = x - resolution - (goofy?1:0);
            }
            
            for(int i=0;i<resolution;i++)
                brightY.setColor(x+i,y,ofColor(xx*255/WIDTH));
        }
    }
    //brightY.update();
    
    //img->setFromPixels(&pix);
    
    return img;
}

static ofImage* getPixelsRevHImage(ofImage *img, float thresh, bool goofy) {
    if(brightY.width==0)
        brightY.allocate(WIDTH,HEIGHT2, OF_IMAGE_GRAYSCALE);
    ofColor c;
    float b;
    
    for (int y=0; y<HEIGHT2; y++) {
        int xx = 0;
        c = img->getColor(xx,y);
        b = q(c);
        for (int x=0; x<WIDTH/2; x+=resolution) {
            ofColor c2 = img->getColor(x,y);
            
            if (abs(q(c2) - b) > thresh) {
                c = c2;
                b = q(c);
                xx = x - (goofy?1:0);
            }
            
            for(int i=0;i<resolution;i++)
                brightY.setColor(x+i,y,ofColor(xx*255/WIDTH));
        }
        xx = WIDTH-1;
        c = img->getColor(xx,y);
        b = q(c);
        for (int x=WIDTH-1; x>=WIDTH/2; x-=resolution) {
            ofColor c2 = img->getColor(x,y);
            if (abs(q(c2) - b) > thresh) {
                c = c2;
                b = q(c);
                xx = x + resolution + 1 + (goofy?1:0);
            }
            
            for(int i=0;i<resolution;i++)
                brightY.setColor(x-i,y,ofColor(xx*255/WIDTH));
        }
    }
    brightY.update();
    return &brightY;
}

static int distTest=0;
static ofImage* getPixelsVImage(ofImage *img, float thresh, bool goofy) {
    if(brightY.width==0)
        brightY.allocate(WIDTH,HEIGHT2, OF_IMAGE_GRAYSCALE);
    ofColor c;
    float b;
    
    for (int x=0; x<WIDTH; x++) {
        c = img->getColor(x,HEIGHT2/2);
        b = q(c);
        int yy = WIDTH/2;
        for (int y=HEIGHT2/2; y<HEIGHT2; y+=resolution) {
            ofColor c2 = img->getColor(x,y);
            
            if (abs(q(c2) - b) > thresh) {
                c = c2;
                b = q(c);
                yy = y  - (goofy?1:0);;
            }
            
            for(int i=0;i<resolution;i++)
                brightY.setColor(x,y-i,ofColor(yy*255/HEIGHT2));
        }
        c = img->getColor(x, HEIGHT2/2);
        b = q(c);
        yy = HEIGHT2/2;
        for (int y=HEIGHT2/2; y>=0; y-=resolution) {
            ofColor c2 = img->getColor(x,y);
            if (abs(q(c2) - b) > thresh) {
                c = c2;
                b = q(c);
                yy = y + 1 + resolution + (goofy?1:0);;
            }
            
            for(int i=0;i<resolution;i++)
                brightY.setColor(x,y+i,ofColor(yy*255/HEIGHT2));
        }
    }
    brightY.update();
    return &brightY;
}


/*static void sortBrightYCenter(ofImage *img, float thresh) {
 ofColor c;
 float b;
 for (int x=0; x<WIDTH; x++) {
 c = img->getColor(x,HEIGHT/2);
 b = q(&c);
 img->setColor(x,HEIGHT/2,c);
 for (int y=HEIGHT/2-1; y>=0; y--) {
 if (abs(q(&(img->getColor(x,y))) - b) > thresh) {
 c = img->getColor(x,y);
 b = q(&c);
 }
 img->setColor(x,y,c);
 }
 c = img->getColor(x,HEIGHT/2);
 b = q(&c);
 img->setColor(x,HEIGHT/2,c);
 for (int y=HEIGHT/2; y<HEIGHT; y++) {
 if (abs(q(&(img->getColor(x,y))) - b) > thresh) {
 c = img->getColor(x,y);
 b = q(&c);
 }
 img->setColor(x,y,c);
 }
 }
 img->update();
 }*/

static void sortPixels(ofImage *img){
    std::vector<unsigned char> p;
    unsigned char* pix = img->getPixels();
    for(int i=0;i<img->width*img->height;i++)
        p.push_back(pix[i]);
    //ofSort(p);
    int n = img->width*img->height;
    for(int i=0;i<n-1;i++){
        bool swap = false;
        if(p[i] > p[i+1]){
            unsigned char temp = p[i+1];
            p[i+1] = p[i];
            p[i] = temp;
            swap = true;
        }
    }
    for(int x=0;x<img->width;x++)
        for(int y=0;y<img->height;y++)
            img->setColor(x,y,p[x+y*img->width]);
}

static void loadFXShaders(){
    alphaShader.load("../shaders/alpha");
    skewShader.load("../shaders/skew");
    skewShader.load("../shaders_circle/skew");
    displaceShader.load("../shaders/displace");
    displaceShader.load("../shaders_circle/displace");
    noiseShader.load("../shaders/noise");
    pixelShader.load("../shaders/pixellate");
    pixelShader.load("../shaders_circle/pixellate");
    sobelShader.load("../shaders/sobel");
    sobelShader.load("../shaders_circle/sobel");
    gBlurHor.load("../shaders/gblurHD");
    hdrShader.load("../shaders/gamma");
    sInvert.load("../shaders/invert");
    kShader.load("../shaders/kalei");
    kShader.load("../shaders_circle/kalei");
    chromaShader.load("../shaders/chromaSep");
    circleShader.load("../shaders/circle");
    displaceGlitchShader.load("../shaders/displaceGlitch");
    lsdShader.load("../shaders/lsd");
    triumShader.load("../shaders/trium");
    perturbationShader.load("../shaders/perturbation");
    dualShader.load("../shaders/dual");

}

static float* skewVector;
static void updateResolution(){
    /** DATA VECTORS **/
    free(skewVector);
    skewVector = (float*) malloc(app->crt_WIDTH*sizeof(float));
    
    free(displaceVector);
    displaceVector = (float*) malloc(app->crt_WIDTH*sizeof(float));
}

static void initGlitches(AbstractApp* a){
    app = a;
    
    sphereMesh = ofMesh::sphere(HEIGHT/2.25, 100);
    
    for(int i=0;i<sphereMesh.getNumTexCoords();i++){
        if(sphereMesh.getVertex(i).z<0)
            sphereMesh.setTexCoord(i, (sphereMesh.getTexCoord(i)*ofVec3f(2, 2*HEIGHT/(float)WIDTH) - ofVec2f(0.5, 0.)));
        else
            sphereMesh.setTexCoord(i, (sphereMesh.getTexCoord(i)*ofVec3f(-2, 2*HEIGHT/(float)WIDTH) + ofVec2f(-0.5, 0.)));
    }
    for(int i=0;i<sphereMesh.getNumVertices();i++){
        ofVec3f v = sphereMesh.getVertex(i);
        if(v.z < 0 )
            v.z *= -0.99;
        sphereMesh.setVertex(i, v);
    }
    
    loadFXShaders();
    
    tunnelFbo.allocate(WIDTH,HEIGHT2);
    iii.allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR_ALPHA);
    
    sortImage.allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR_ALPHA);
    

    for(int i=0;i<MAX_OMG3D2_FBOS;i++){
        ofFbo* fbo = new ofFbo;
        fbo->allocate(WIDTH, HEIGHT);
        zsFbos.push_back(fbo);
    }
    
    
    /** DATA VECTORS **/
    updateResolution();
    updateSkew(skewVector);
    
    for(int i=0;i<2;i++){
        std::vector<ofVec4f> z;
        zs.push_back(z);
    }
}



#endif
