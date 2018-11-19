#ifndef BORING_ANGEL_AbstractApp_h
#define BORING_ANGEL_AbstractApp_h

#include <ofBaseTypes.h>
#include <stdlib.h>
#include "pdefinition.h"

#include "ofMain.h"

//#include "ofxIldaFrame.h"

#define MIDI_CC 0
#define MIDI_NOTEON 1
#define MIDI_NOTEOFF 2

using namespace std;

class Auto;

class AbstractApp
{
public:
    
    ofColor getRandomColor(){
        return ofColor::fromHsb(ofRandom(255),
                                parameterMap[sidesSaturation]*255
                                , 255);
    }
    
    virtual ofFbo* getPastFbo(int index) = 0;
    
    void initParameterMap(){
        
        
        parameterNameMap["post_traitement"]=post_traitement;
        parameterNameMap["omg3D"]=omg3D;
        parameterNameMap["omg3DAngle"]=omg3DAngle;
        parameterNameMap["omg3D2"]=omg3D2;
        parameterNameAliasMap["o"]=omg3D2;
        parameterNameMap["omg3D2Dist"]=omg3D2Dist;
        parameterNameMap["oDist"]=omg3D2Dist;
        parameterNameMap["omg3D2Y"]=omg3D2Y;
        parameterNameAliasMap["oY"]=omg3D2Y;
        parameterNameMap["omg3D2x2"]=omg3D2x2;
        parameterNameMap["omg3D2Alpha0"]=omg3D2Alpha0;
        parameterNameMap["omg3D2AlphaZ"]=omg3D2AlphaZ;
        parameterNameMap["omg3D2RollerX"]=omg3D2RollerX;
        parameterNameMap["omg3D2RollerY"]=omg3D2RollerY;
        parameterNameMap["omg3D2Symetry"]=omg3D2Symetry;
        parameterNameMap["omg3D2Rotation"]=omg3D2Rotation;
        parameterNameAliasMap["oRot"]=omg3D2Rotation;
        parameterNameMap["divergence"]=divergence;
        parameterNameMap["divergenceZ"]=divergenceZ;
        parameterNameAliasMap["omg3D2Divergence"]=divergence;
        parameterNameMap["omg3D2Speed"]=omg3D2Speed;
        parameterNameMap["omg3D2FreeRotation"]=omg3D2FreeRotation;
        parameterNameMap["omg3D2AvoidCenter"]=omg3D2AvoidCenter;
        parameterNameMap["omg3D2DOF"]=omg3D2DOF;
        
        parameterNameMap["thresholdAfterBall"]=thresholdAfterBall;
        parameterNameMap["kinectScale"]=kinectScale;
        parameterNameMap["kinectMasking"]=kinectMasking;
        parameterNameMap["glowResolution"]=glowResolution;
        parameterNameMap["randHHide"]=randHHide;
        parameterNameMap["randVHide"]=randVHide;
        parameterNameMap["glow"]=glow;
        parameterNameMap["sobel"]=sobel;
        parameterNameMap["sobelPostFX"]=sobelPostFX;
        parameterNameMap["sobelMix"]=sobelMix;
        parameterNameMap["whiteSobel"]=whiteSobel;
        parameterNameMap["gradient"]=gradient;
        parameterNameMap["contrast"]=contrast;
        parameterNameMap["invert"]=_invert;
        parameterNameMap["vblur"]=vblur;
        parameterNameMap["hblur"]=hblur;
        parameterNameMap["blurOriginalMix"]=blurOriginalMix;
        parameterNameMap["sharpen"]=sharpen;
        parameterNameMap["sharpenMode"]=sharpenMode;
        parameterNameMap["nBlocks"]=nBlocks;
        parameterNameMap["nFreeze"]=nFreeze;
        parameterNameMap["strobe"]=strobe;
        parameterNameMap["xpixellate"]=xpixellate;
        parameterNameMap["ypixellate"]=ypixellate;
        parameterNameMap["embedUzi"]=embedUzi;
        parameterNameMap["embedCinema"]=embedCinema;
        parameterNameMap["threeDeeScene"]=threeDeeScene;
        parameterNameMap["toLine"]=toLine;
        parameterNameMap["toCircle"]=toCircle;
        parameterNameMap["lineDa"]=lineDa;
        parameterNameMap["lineDx"]=lineDx;
        parameterNameMap["blendType"]=blendType;
        parameterNameMap["flash"]=flash;
        parameterNameMap["camera"]=camera;
        parameterNameMap["warp"]=warp;
        parameterNameMap["mediaX"]=mediaX;
        parameterNameMap["mediaY"]=mediaY;
        parameterNameMap["mediaZ"]=mediaZ;
        parameterNameMap["mediaRotX"]=mediaRotX;
        parameterNameMap["mediaRotY"]=mediaRotY;
        parameterNameMap["mediaRotZ"]=mediaRotZ;
        parameterNameMap["paint"]=paint;
        parameterNameMap["multiFbos"]=multiFbos;
        parameterNameMap["saturation"]=tintSaturation;
        parameterNameAliasMap["tintSaturation"]=tintSaturation;
        parameterNameMap["reTintHue"]=reTintHue;
        parameterNameMap["hue"]=tintHue;
        parameterNameAliasMap["tintHue"]=tintHue;
        parameterNameMap["gradient"]=gradient;
        parameterNameMap["brightness"]=tintBrightness;
        parameterNameAliasMap["tintBrightness"]=tintBrightness;
        parameterNameMap["map_mask"]=map_mask;
        parameterNameMap["tintAmp"]=tintAmp;
        parameterNameMap["chromaSaturation"]=sidesSaturation;
        parameterNameMap["reTint"]=_reTint;
        parameterNameMap["tintMode"]=tintMode;
        parameterNameMap["randomTint"]=randomTint;
        parameterNameMap["bpm"]=bpm;
        parameterNameMap["bpmLock"]=bpmLock;
        parameterNameMap["kalei"]=kalei;
        parameterNameMap["kaleiCopyBorder"]=kaleiCopyBorder;
        parameterNameMap["kaleiNb"]=kaleiNb;
        parameterNameMap["kalei_2"]=kalei_2;
        parameterNameMap["kaleiNb_2"]=kaleiNb_2;
        parameterNameMap["kaleiOffX"]=kaleiOffX;
        parameterNameMap["kaleiOffY"]=kaleiOffY;
        parameterNameMap["kaleiScale"]=kaleiScale;
//        parameterNameMap["stripesAmp"]=stripesAmp;
//        parameterNameMap["stripesSize"]=stripesSize;
        parameterNameMap["skewAmp"]=skewAmp;
        parameterNameMap["skewDAmp"]=skewDAmp;
        parameterNameMap["skewAAmp"]=skewAAmp;
        parameterNameMap["skewVAmp"]=skewVAmp;
        parameterNameMap["skewSpeed"]=skewSpeed;
        parameterNameMap["skewVAmpMi"]=skewVAmpMi;
        parameterNameMap["skewAmpMi"]=skewAmpMi;
        parameterNameMap["skewBorderCopy"]=skewBorderCopy;
        parameterNameMap["displaceAmp"]=displaceAmp;
        parameterNameMap["displaceVAmp"]=displaceVAmp;
        parameterNameMap["displaceProba"]=displaceProba;
        parameterNameMap["chromaOffset"]=chromaOffset;
        parameterNameMap["chromaAngle"]=chromaSepAngle;
        parameterNameMap["chromaHue"]=chromaSepHue;
        parameterNameMap["chromaSep"]=chromaSep;
        parameterNameMap["chromaSepAlpha"]=chromaSepAlpha;
        parameterNameMap["doubleChroma"]=doubleChromaSep;
        parameterNameMap["chromaSens"]=chromaSens;
        parameterNameMap["sortXThresh"]=sortXThresh;
        parameterNameMap["sortYThresh"]=sortYThresh;
        parameterNameMap["audio"]=audio;
        parameterNameMap["audioIn"]=_audioIn;
        parameterNameMap["selSat"]=selSat;
        parameterNameMap["dualFocus"]=dualFocus;
        parameterNameMap["dualFocusIntensity"]=dualFocusIntensity;
        parameterNameMap["currentScene"]=currentScene;
        parameterNameMap["antiAliasing"]=antiAliasing;
        parameterNameMap["selectiveShading"]=selectiveShading;
        parameterNameMap["loadMacro"]=_loadMacro;
        parameterNameMap["skewJumpRate"]=skewJumpRate;
        parameterNameMap["skewHard"]=skewHard;
        parameterNameMap["skewBandingProb"]=skewBandingProb;
        parameterNameMap["skewBandingJumpProb"]=skewBandingJumpProb;
        parameterNameMap["omg3D2HardRotation"]=omg3D2HardRotation;
        parameterNameMap["omg3D2Nb"]=omg3D2Nb;
        parameterNameMap["kaleiCopy"]=kaleiCopy;
        parameterNameMap["kaleiCopyAdjust"]=kaleiCopyAdjust;
        parameterNameMap["surprise"]=surprise;
//        parameterNameMap["warpRemap"]=warpRemap;
//        parameterNameMap["warpX"]=warpX;
//        parameterNameMap["warpY"]=warpY;
        parameterNameMap["stressTest"]=stressTest;
        parameterNameMap["stressTestRate"]=stressTestRate;
        parameterNameMap["reSaturate"]=reSaturate;
        parameterNameMap["prollage"]=prollage;
        parameterNameMap["dispose"]=dispose;
        parameterNameMap["disposeFreq"]=disposeFreq;
        parameterNameMap["disposeX"]=disposeX;
        parameterNameMap["leapAutoReco"]=leapAutoReco;
        parameterNameMap["hueShift"]=hueShift;
        parameterNameMap["hueShiftCenter"]=hueShiftCenter;
        parameterNameMap["hueShiftRange"]=hueShiftRange;
        parameterNameMap["hueShiftCrazy"]=hueShiftCrazy;
        parameterNameMap["kaleiMirror"]=kaleiMirror;
        parameterNameMap["omg3D2ZScale"]=omg3D2ZScale;
        parameterNameMap["omg3D2LumaKey"]=omg3D2LumaKey;
        parameterNameMap["invertCircle"]=invertCircle;
        parameterNameMap["debug"]=debug;
        parameterNameMap["agentSpeed"]=agentSpeed;
        parameterNameMap["agentNb"]=agentNb;
        parameterNameMap["agentShapeNb"]=agentShapeNb;
        parameterNameMap["agentShapeNbRand"]=agentShapeNbRand;
        parameterNameMap["agentNbShapes"]=agentNbShapes;
        parameterNameMap["agentFill"]=agentFill;
        parameterNameMap["agentRandDist"]=agentRandDist;
        parameterNameMap["agentAngleFreq"]=agentAngleFreq;
        parameterNameMap["feedBack"]=feedBack;
        parameterNameMap["nbPoints"]=nbPoints;
        parameterNameMap["mediaAlpha"]=mediaAlpha;
//        parameterNameMap["mediaSaturation"]=mediaSaturation;
        parameterNameMap["blackCenter"]=blackCenter;
        parameterNameMap["customBorders"]=customBorders;
//        parameterNameMap["pointy"]=pointy;
        parameterNameMap["feedbackRemanence"]=feedbackRemanence;
        parameterNameMap["noSource"]=noSource;
        parameterNameMap["erode"]=erode;
        parameterNameMap["fastErode"]=fastErode;
        parameterNameMap["threshold"]=threshold;
        parameterNameMap["rot"]=rot;
        parameterNameMap["upRot"]=upRot;
        parameterNameMap["pitchRot"]=pitchRot;
        parameterNameMap["scale"]=scale;
        parameterNameMap["scan"]=scan;
//        parameterNameMap["offx"]=offx;
//        parameterNameMap["offy"]=offy;
        parameterNameMap["pace"]=pace;
        parameterNameMap["f_strobe"]=f_strobe;
        parameterNameMap["f_invertFrame"]=f_invertFrame;
        parameterNameMap["doubleInversion"]=doubleInversion;
        parameterNameMap["backMask"]=backMask;
        parameterNameMap["curShapeRot"]=curShapeRot;
        parameterNameMap["shapeRot"]=shapeRot;
        parameterNameMap["shapeNbPts"]=shapeNbPts;
        parameterNameMap["shapeStyle"]=shapeStyle;
        parameterNameMap["shapeWeight"]=shapeWeight;
        parameterNameMap["randomShapeColor"]=randomShapeColor;
        parameterNameMap["randomText"]=randomText;
        parameterNameMap["seqText"]=seqText;
        parameterNameMap["randomFont"]=randomFont;
        parameterNameMap["txt_randPosition"]=txt_randPosition;
        parameterNameMap["txt_randSize"]=txt_randSize;
        parameterNameMap["nWords"]=nWords;
        parameterNameMap["txt_accumulate"]=txt_accumulate;
        parameterNameMap["txt_fill"]=txt_fill;
        parameterNameMap["fontSize"]=fontSize;
        parameterNameMap["txt_rate"]=txt_rate;
        parameterNameMap["wordLimit"]=wordLimit;
        parameterNameMap["updateLen"]=updateLen;
        parameterNameMap["speed"]=speed;
        parameterNameMap["lines_yWeight"]=lines_yWeight;
        parameterNameMap["lines_zWeight"]=lines_zWeight;
        parameterNameMap["lines_zSquare"]=lines_zSquare;
        parameterNameMap["lines_ySquare"]=lines_ySquare;
        parameterNameMap["lines_yres"]=lines_yres;
        parameterNameMap["lines_mesh"]=lines_mesh;
        parameterNameMap["crtPos"]=crtPos;
        parameterNameMap["movieSpeed"]=movieSpeed;
        parameterNameMap["transluscentUzi"]=transluscentUzi;
        parameterNameMap["gamma"]=_gamma;
        parameterNameMap["adjust"]=adjust;
        parameterNameMap["loopLength"]=loopLength;
        parameterNameMap["loopStart"]=loopStart;
        parameterNameMap["loopMode"]=loopMode;
        parameterNameMap["supZoom"]=supZoom;
        parameterNameMap["supRot"]=supRot;
        parameterNameMap["jumpTo"]=jumpTo;
        parameterNameMap["hMirror"]=hMirror;
        parameterNameMap["vMirror"]=vMirror;
        parameterNameMap["paintResolution"]=paintResolution;
        parameterNameMap["selectedPlayer"]=selectedPlayer;
        parameterNameMap["syncToTempo"]=syncToTempo;
        parameterNameMap["circle"]=circle;
        parameterNameMap["circleSpeed"]=circleSpeed;
        parameterNameMap["circleRotation"]=circleRotation;
        parameterNameMap["circleDist"]=circleDist;
        parameterNameMap["macro"]=macro;
        parameterNameMap["divergenceZ"]=divergenceZ;
        parameterNameMap["divergenceY"]=divergenceY;
        parameterNameAliasMap["omg3D2YDivergence"]=divergenceY; //alias
        parameterNameMap["doubleFeedback"]=doubleFeedback;
        parameterNameMap["randomSpeed"]=randomSpeed;
        parameterNameMap["paintSizeDelta"]=paintSizeDelta;
        parameterNameMap["switchImg"]=switchImg;
        parameterNameMap["autoSwitchImg"]=autoSwitchImg;
        parameterNameMap["randomUzi"]=randomUzi;
        parameterNameMap["uziPeriod"]=uziPeriod;
        parameterNameMap["omg3D2X"]=omg3D2X;
        parameterNameAliasMap["oX"]=omg3D2X;
        parameterNameMap["mask"]=_mask;
        parameterNameMap["resize"]=resize;
        parameterNameMap["autoRot"]=autoRot;
        parameterNameMap["autoRotFreq"]=autoRotFreq;
        parameterNameMap["ak47Mode"]=ak47Mode;
        parameterNameMap["ak47Frame"]=ak47Frame;
        parameterNameMap["lastAk47Index"]=lastAk47Index;
        parameterNameMap["swapTint"]=swapTint;
        parameterNameMap["draw_recording"]=draw_recording;
        parameterNameMap["draw_maxConnectionLength"]=draw_maxConnectionLength;
        parameterNameMap["draw_minConnectionLength"]=draw_minConnectionLength;
        parameterNameMap["draw_pace"]=draw_pace;
        parameterNameMap["draw_hold"]=draw_hold;
        parameterNameMap["draw_destroy"]=draw_destroy;
        parameterNameMap["draw_destroyMode"]=draw_destroyMode;
        parameterNameMap["draw_recInterval"]=draw_recInterval;
        parameterNameMap["draw_currentDrawing"]=draw_currentDrawing;
        parameterNameMap["draw_random"]=draw_random;
        parameterNameMap["draw_randomRate"]=draw_randomRate;
        parameterNameMap["draw_lines"]=draw_lines;
        parameterNameMap["draw_points"]=draw_points;
        parameterNameMap["draw_fresk"]=draw_fresk;
        parameterNameMap["draw_ox"]=draw_ox;
        parameterNameMap["draw_oy"]=draw_oy;
        parameterNameMap["draw_oz"]=draw_oz;
        parameterNameMap["draw_zSpeed"]=draw_zSpeed;
        parameterNameMap["background"]=background;
        parameterNameMap["lcd"]=lcd;
        parameterNameMap["bw"]=bw;
        parameterNameMap["bwOffset"]=bwOffset;
        parameterNameMap["draw_freskSpeed"]=draw_freskSpeed;
        parameterNameMap["draw_consecutive"]=draw_consecutive;
        parameterNameMap["draw_simplify"]=draw_simplify;
        parameterNameMap["frameRate"]=frameRate;
        parameterNameMap["draw_destruction"]=draw_destruction;
        parameterNameMap["otherWorlds"]=otherWorlds;
        parameterNameMap["mediaScaleX"]=mediaScaleX;
        parameterNameMap["mediaScaleY"]=mediaScaleY;
        parameterNameMap["glitch_mode "]=glitch_mode ;
        parameterNameMap["glitch_thresh "]=glitch_thresh ;
        parameterNameMap["map_event"]=map_event;
        parameterNameMap["map_remanence"]=map_remanence;
        parameterNameMap["map_mode"]=map_mode;
        parameterNameMap["map_sync"]=map_sync;
        parameterNameMap["map_pace"]=map_pace;
        parameterNameMap["map_prog"]=map_prog;
        parameterNameMap["audioGain"]=audioGain;
        parameterNameMap["evolution"]=evolution;
        parameterNameMap["kinect"]=kinect;
        parameterNameMap["kinectRender"]=kinectRender;
        parameterNameMap["randomJump"]=randomJump;
        parameterNameMap["echoPeriod"]=echoPeriod;
        parameterNameMap["echoRandom"]=echoRandom;
        parameterNameMap["echoNb"]=echoNb;
        parameterNameMap["drawBrightDist"]=drawBrightDist;
        parameterNameMap["borderMask"]=borderMask;
        parameterNameMap["borderMaskd0"]=borderMaskd0;
        parameterNameMap["ledMode"]=ledMode;
        parameterNameMap["ledSync"]=ledSync;
        parameterNameMap["ledPeriod"]=ledPeriod;
        parameterNameMap["ledInfo"]=ledInfo;
        parameterNameMap["ledBrightness"]=ledBrightness;
        parameterNameMap["ledEvent"]=ledEvent;
        parameterNameMap["ledTint"]=ledTint;
        parameterNameMap["ledStrobe"]=ledStrobe;
        parameterNameMap["echoAdjust"] = echoAdjust;
        parameterNameMap["omg3D2Strobe"] = omg3D2Strobe;
        parameterNameMap["omg3D2Acc"] = omg3D2Acc;
        parameterNameMap["chromaMode"] = chromaSepMode;
        parameterNameAliasMap["chromaSepMode"] = chromaSepMode; //alias
        parameterNameMap["user1"] = user1;
        parameterNameMap["user2"] = user2;
        parameterNameMap["user3"] = user3;
        parameterNameMap["user4"] = user4;
        parameterNameMap["user5"] = user5;
        parameterNameMap["omg3D2Depth"] = omg3D2Depth;
        parameterNameMap["pnoise"] = pnoise;
        parameterNameMap["pert"] = pert;
        parameterNameMap["pertEvo"] = pertEvo;
        parameterNameMap["pertPersistance"] = pertPersistance;
        parameterNameMap["pertFreq"] = pertFreq;
        parameterNameMap["pertHue"] = pertHue;
        parameterNameMap["typhoon"] = typhoon;
        parameterNameMap["taijin"] = taijin;
        parameterNameMap["tiles"] = tiles;
        parameterNameMap["tiltShift"] = tiltShift;

        parameterNameMap["glitchFreq"] = glitchFreq;
        parameterNameMap["omg3D2ADivergence"] = aDivergence;
        parameterNameAliasMap["aDivergence"] = aDivergence; //alias
        parameterNameMap["ledAuto"] = ledAuto;
        parameterNameMap["fadeOutMode"] = fadeOutMode;
        parameterNameMap["pertEvoAuto"] = pertEvoAuto;
        parameterNameMap["useJoyStick"] = useJoyStick;
        parameterNameMap["ledSaturation"] = ledSaturation;
        parameterNameMap["uvLight"] = uvLight;
        parameterNameMap["noise"] = noise;
        parameterNameMap["mandala"] = mandala;
        parameterNameMap["mandalaBorder"] = mandalaBorder;
        parameterNameMap["squareMandala"] = squareMandala;
        parameterNameMap["bypassCTRL"] = bypassCTRL;
        parameterNameMap["hueKalei"] = hueKalei;
        parameterNameMap["hueFilter"] = hueFilter;
        parameterNameMap["psyShift"] = psyShift;
        parameterNameMap["whitePoint"] = whitePoint;
        parameterNameMap["blackPoint"] = blackPoint;
        parameterNameMap["pertMode"] = pertMode;
        parameterNameMap["oscIn"] = oscIn;
        parameterNameMap["FXInFeedbackLoop"] = post_traitement;
        parameterNameMap["jpgGlitch"] = jpgGlitch;
        parameterNameMap["keying"] = keying;
        parameterNameMap["macroFade"] = macroFade;
        parameterNameMap["engraving"] = engraving;
        parameterNameMap["illu"] = illu;

        parameterIDMap.resize(N_PARAM);
        for(map<string,int>::iterator it=parameterNameMap.begin();it!=parameterNameMap.end();it++)
            parameterIDMap[it->second] = it->first;
        
        for(int i=0;i<N_PARAM;i++)
            parametersInGUIBounds[i]=(ofVec2f(0, 1));

#ifdef EXPORT
        parametersInGUI.push_back(scale);
        parametersInGUI.push_back(upRot);
        parametersInGUI.push_back(feedbackRemanence);
        parametersInGUI.push_back(post_traitement);
        parametersInGUI.push_back(omg3D);
        parametersInGUI.push_back(omg3D2);
        parametersInGUI.push_back(omg3D2Dist);
        parametersInGUI.push_back(omg3D2Depth);
        parametersInGUI.push_back(omg3D2Rotation);
        parametersInGUI.push_back(omg3D2Speed);
        parametersInGUI.push_back(divergence);
        parametersInGUI.push_back(aDivergence);
        parametersInGUI.push_back(omg3D2X);
        parametersInGUI.push_back(omg3D2Y);
        parametersInGUI.push_back(omg3D2x2);
        parametersInGUI.push_back(bw);
        parametersInGUI.push_back(tintHue);
        parametersInGUI.push_back(gradient);
        parametersInGUI.push_back(_reTint);
        parametersInGUI.push_back(tintSaturation);
        parametersInGUI.push_back(_gamma);
        parametersInGUI.push_back(contrast);
        parametersInGUI.push_back(sharpen);
        parametersInGUI.push_back(_invert);
        parametersInGUI.push_back(blackPoint);
        parametersInGUI.push_back(whitePoint);
        parametersInGUI.push_back(chromaSep);
        parametersInGUI.push_back(chromaSepHue);
        parametersInGUI.push_back(sobel);
        parametersInGUI.push_back(whiteSobel);
        parametersInGUI.push_back(glow);
        parametersInGUI.push_back(mandala);
        parametersInGUI.push_back(toCircle);
        parametersInGUI.push_back(warp);
        parametersInGUI.push_back(toLine);
        parametersInGUI.push_back(hblur);
        parametersInGUI.push_back(vblur);
        parametersInGUI.push_back(xpixellate);
        parametersInGUI.push_back(ypixellate);
        parametersInGUI.push_back(kalei);
        parametersInGUI.push_back(kaleiNb);
        parametersInGUI.push_back(nFreeze);
        parametersInGUI.push_back(displaceAmp);
        parametersInGUI.push_back(displaceProba);
        parametersInGUI.push_back(skewAmp);
        parametersInGUI.push_back(pert);
        parametersInGUI.push_back(pertEvoAuto);
        parametersInGUI.push_back(echoNb);
#else
        parametersInGUI.push_back(mediaX);
        parametersInGUI.push_back(mediaY);
        parametersInGUI.push_back(mediaZ);
        parametersInGUI.push_back(mediaRotX);
        parametersInGUI.push_back(mediaRotY);
        parametersInGUI.push_back(mediaRotZ);
        parametersInGUI.push_back(omg3D);
        parametersInGUI.push_back(omg3D2);
        parametersInGUI.push_back(omg3D2Dist);
        parametersInGUI.push_back(omg3D2Depth);
//        parametersInGUI.push_back(omg3D2Nb);
        parametersInGUI.push_back(omg3D2Rotation);
        parametersInGUI.push_back(omg3D2Speed);
        parametersInGUI.push_back(divergence);
        parametersInGUI.push_back(aDivergence);
        parametersInGUI.push_back(omg3D2X);
        parametersInGUI.push_back(omg3D2Y);
        parametersInGUI.push_back(omg3D2x2);
        parametersInGUI.push_back(bw);
        parametersInGUI.push_back(tintHue);
        parametersInGUI.push_back(gradient);
        parametersInGUI.push_back(_reTint);
        parametersInGUI.push_back(tintSaturation);
        parametersInGUI.push_back(_gamma);
        
      
        parametersInGUI.push_back(contrast);
        parametersInGUI.push_back(sharpen);
        parametersInGUI.push_back(_invert);
        parametersInGUI.push_back(blackPoint);
        parametersInGUI.push_back(whitePoint);
        parametersInGUI.push_back(chromaSep);
        parametersInGUI.push_back(chromaSepHue);
        parametersInGUI.push_back(sobel);
        parametersInGUI.push_back(sobelMix);
        parametersInGUI.push_back(whiteSobel);
        parametersInGUI.push_back(glow);
        parametersInGUI.push_back(mandala);
        parametersInGUI.push_back(toCircle);
        parametersInGUI.push_back(warp);
        parametersInGUI.push_back(toLine);
        parametersInGUI.push_back(hblur);
        parametersInGUI.push_back(vblur);
        parametersInGUI.push_back(xpixellate);
        parametersInGUI.push_back(ypixellate);
        parametersInGUI.push_back(kalei);
        parametersInGUI.push_back(kaleiNb);
        parametersInGUI.push_back(nFreeze);
        parametersInGUI.push_back(displaceAmp);
//        parametersInGUI.push_back(displaceVAmp);
        parametersInGUI.push_back(displaceProba);
        parametersInGUI.push_back(skewAmp);
//        parametersInGUI.push_back(skewVAmp);
        parametersInGUI.push_back(pert);
        parametersInGUI.push_back(pertEvoAuto);
//        parametersInGUI.push_back(paint);
        parametersInGUI.push_back(echoNb);
        parametersInGUI.push_back(borderMask);
#endif
        
        parametersInGUIBounds[mediaX]=(ofVec2f(-0.5, 0.5));
        parametersInGUIBounds[mediaY]=(ofVec2f(-0.5, 0.5));
        parametersInGUIBounds[mediaZ]=(ofVec2f(-0.5, 0.5));
        parametersInGUIBounds[mediaRotX]=(ofVec2f(-180, 180));
        parametersInGUIBounds[mediaRotY]=(ofVec2f(-180, 180));
        parametersInGUIBounds[mediaRotZ]=(ofVec2f(-180, 180));
        parametersInGUIBounds[omg3D]=(ofVec2f(0, 1));
        parametersInGUIBounds[omg3D2]=(ofVec2f(0, 1));
        parametersInGUIBounds[omg3D2Rotation]=(ofVec2f(-720, 720));
        parametersInGUIBounds[omg3D2Speed]=(ofVec2f(-0.1, 0.1));
        parametersInGUIBounds[divergence]=(ofVec2f(-1, 1));
        parametersInGUIBounds[aDivergence]=(ofVec2f(-1, 1));
        parametersInGUIBounds[omg3D2X]=(ofVec2f(0, 1));
        parametersInGUIBounds[omg3D2Dist]=(ofVec2f(0, 2));
        parametersInGUIBounds[omg3D2Depth]=(ofVec2f(0, 1));
        parametersInGUIBounds[omg3D2Nb]=(ofVec2f(2, 50));
        parametersInGUIBounds[omg3D2Y]=(ofVec2f(0, 1));
        parametersInGUIBounds[omg3D2x2]=(ofVec2f(0, 1));
        parametersInGUIBounds[bw]=(ofVec2f(0, -5));
        parametersInGUIBounds[tintHue]=(ofVec2f(0, 1));
        parametersInGUIBounds[gradient]=(ofVec2f(-1, 1));
        parametersInGUIBounds[_reTint]=(ofVec2f(0, 1));
        parametersInGUIBounds[tintSaturation]=(ofVec2f(0, 1));
        parametersInGUIBounds[_gamma]=(ofVec2f(1, 5));
        parametersInGUIBounds[contrast]=(ofVec2f(0, 1));
   parametersInGUIBounds[sharpen]=(ofVec2f(0, 1));
    parametersInGUIBounds[_invert]=(ofVec2f(0, 1));
  parametersInGUIBounds[blackPoint]=(ofVec2f(0, 0.5));
 parametersInGUIBounds[whitePoint]=(ofVec2f(0.5, 1));
parametersInGUIBounds[chromaSep]=(ofVec2f(0, 1));
 parametersInGUIBounds[chromaSepHue]=(ofVec2f(0, 1));
  parametersInGUIBounds[sobel]=(ofVec2f(0, 1));
   parametersInGUIBounds[sobelMix]=(ofVec2f(0, 1));
    parametersInGUIBounds[whiteSobel]=(ofVec2f(0, 1));
 parametersInGUIBounds[glow]=(ofVec2f(0, 3));
  parametersInGUIBounds[mandala]=(ofVec2f(0, 15));
    parametersInGUIBounds[toCircle]=(ofVec2f(0, 1));
 parametersInGUIBounds[warp]=(ofVec2f(0, 1));
 parametersInGUIBounds[toLine]=(ofVec2f(0, 1));
 parametersInGUIBounds[hblur]=(ofVec2f(0, 1));
 parametersInGUIBounds[vblur]=(ofVec2f(0, 1));
parametersInGUIBounds[xpixellate]=(ofVec2f(0, 100));
  parametersInGUIBounds[ypixellate]=(ofVec2f(0, 100));
parametersInGUIBounds[kalei]=(ofVec2f(0, 1));
 parametersInGUIBounds[kaleiNb]=(ofVec2f(0, 8));
  parametersInGUIBounds[nFreeze]=(ofVec2f(0, 25));
 parametersInGUIBounds[displaceAmp]=(ofVec2f(0, 1));
                parametersInGUIBounds[displaceVAmp]=(ofVec2f(0, 1));
        parametersInGUIBounds[displaceProba]=(ofVec2f(0, 1));
        parametersInGUIBounds[skewAmp]=(ofVec2f(0, 1));
         parametersInGUIBounds[skewVAmp]=(ofVec2f(0, 1));
        parametersInGUIBounds[pert]=(ofVec2f(0, 1));
        parametersInGUIBounds[pertEvoAuto]=(ofVec2f(0, 0.1));
        parametersInGUIBounds[echoNb]=(ofVec2f(0, 10));
                parametersInGUIBounds[echoNb]=(ofVec2f(0, 10));
        parametersInGUIBounds[borderMask]=(ofVec2f(0, 2));

        
        stressTestFilterList.push_back(audio);
        stressTestFilterList.push_back(_audioIn);
        stressTestFilterList.push_back(paint);
        stressTestFilterList.push_back(_invert);
        stressTestFilterList.push_back(_mask);
        stressTestFilterList.push_back(movieSpeed);
        stressTestFilterList.push_back(useJoyStick);
        stressTestFilterList.push_back(tintBrightness);
        stressTestFilterList.push_back(leapAutoReco);
        stressTestFilterList.push_back(debug);
        stressTestFilterList.push_back(kinect);
        stressTestFilterList.push_back(user1);
        stressTestFilterList.push_back(user2);
        stressTestFilterList.push_back(user3);
        stressTestFilterList.push_back(user4);
        stressTestFilterList.push_back(user5);
        stressTestFilterList.push_back(draw_recording);
        stressTestFilterList.push_back(draw_maxConnectionLength);
        stressTestFilterList.push_back(draw_minConnectionLength);
        stressTestFilterList.push_back(draw_pace);
        stressTestFilterList.push_back(draw_hold);
        stressTestFilterList.push_back(draw_destroy);
        stressTestFilterList.push_back(draw_consecutive);
        stressTestFilterList.push_back(draw_destroyMode);
        stressTestFilterList.push_back(draw_currentDrawing);
        stressTestFilterList.push_back(draw_lines);
        stressTestFilterList.push_back(draw_random);
        stressTestFilterList.push_back(draw_randomRate);
        stressTestFilterList.push_back(draw_points);
        stressTestFilterList.push_back(draw_fresk);
        stressTestFilterList.push_back(draw_freskSpeed);
        stressTestFilterList.push_back(draw_recInterval);
        stressTestFilterList.push_back(draw_ox);
        stressTestFilterList.push_back(draw_oy);
        stressTestFilterList.push_back(draw_oz);
        stressTestFilterList.push_back(draw_zSpeed);
        stressTestFilterList.push_back(noSource);
        stressTestFilterList.push_back(bypassCTRL);
        stressTestFilterList.push_back(background);
        stressTestFilterList.push_back(fastErode);
        stressTestFilterList.push_back(txt_randPosition);
        stressTestFilterList.push_back(txt_randSize);
        stressTestFilterList.push_back(txt_fill);
        stressTestFilterList.push_back(txt_accumulate);
        stressTestFilterList.push_back(txt_rate);
        stressTestFilterList.push_back(seqText);
        stressTestFilterList.push_back(map_prog);
        stressTestFilterList.push_back(map_sync);
        stressTestFilterList.push_back(map_event );
        stressTestFilterList.push_back(map_remanence );
        stressTestFilterList.push_back(map_pace );
        stressTestFilterList.push_back(map_mode );
        stressTestFilterList.push_back(skewJumpRate);
        stressTestFilterList.push_back(nWords);
        stressTestFilterList.push_back(frameRate);
        stressTestFilterList.push_back(antiAliasing);
        stressTestFilterList.push_back(bpm);
        stressTestFilterList.push_back(bpmLock);
        stressTestFilterList.push_back(macro);
        stressTestFilterList.push_back(selectedPlayer);
        stressTestFilterList.push_back(doubleFeedback);
        stressTestFilterList.push_back(ledMode);
        stressTestFilterList.push_back(ledSync);
        stressTestFilterList.push_back(ledPeriod);
        stressTestFilterList.push_back(ledInfo);
        stressTestFilterList.push_back(ledBrightness);
        stressTestFilterList.push_back(ledEvent);
        stressTestFilterList.push_back(ledTint);
        stressTestFilterList.push_back(ledStrobe);
        stressTestFilterList.push_back(updateLen);
        stressTestFilterList.push_back(f_strobe);
        stressTestFilterList.push_back(f_invertFrame);
        stressTestFilterList.push_back(feedbackRemanence);
        stressTestFilterList.push_back(agentSpeed);
        stressTestFilterList.push_back(agentShapeNb);
        stressTestFilterList.push_back(agentNb);
        stressTestFilterList.push_back(agentShapeNbRand);
        stressTestFilterList.push_back(agentNbShapes);
        stressTestFilterList.push_back(agentRandDist);
        stressTestFilterList.push_back(agentFill);
        stressTestFilterList.push_back(agentAngleFreq);
        stressTestFilterList.push_back(nbPoints);
        stressTestFilterList.push_back(feedBack);
        stressTestFilterList.push_back(stressTest);
        stressTestFilterList.push_back(stressTestRate);
        stressTestFilterList.push_back(_loadMacro);
        stressTestFilterList.push_back(thresholdAfterBall);
        stressTestFilterList.push_back(kinectScale);
        stressTestFilterList.push_back(kinectMasking);
        stressTestFilterList.push_back(glowResolution);
        stressTestFilterList.push_back(strobe);
        stressTestFilterList.push_back(oscIn);
        stressTestFilterList.push_back(shapeNbPts);
        stressTestFilterList.push_back(loopMode);
        stressTestFilterList.push_back(flash);
        stressTestFilterList.push_back(shapeStyle);
        stressTestFilterList.push_back(ak47Frame);
        stressTestFilterList.push_back(ak47Mode);
        stressTestFilterList.push_back(lines_mesh);
        stressTestFilterList.push_back(lines_yres);
        stressTestFilterList.push_back(lines_ySquare);
        stressTestFilterList.push_back(lines_zSquare);
        stressTestFilterList.push_back(lines_yWeight);
        stressTestFilterList.push_back(lines_zWeight);
        stressTestFilterList.push_back(glitch_mode);
        stressTestFilterList.push_back(omg3D2Nb);


        
    }
    
    int getParameterFromName(string name){
        if(parameterNameMap.count(name))
            return parameterNameMap[name];
        if(parameterNameAliasMap.count(name))
            return parameterNameAliasMap[name];
        return -1;
    }
    
    float scaleGUIValue(int id, float v){
        return ofMap(v, 0, 1, parametersInGUIBounds[id].x, parametersInGUIBounds[id].y, true);
    }
    
    float normalizedGUIValue(int id, float v){
        return ofMap(v, parametersInGUIBounds[id].x, parametersInGUIBounds[id].y, 0, 1, true);
    }
    
    virtual int getNumberOfAutos()=0;
    virtual Auto* getAuto(int id)=0;
    
    void initParameters();
    void randomParameters();
    void niceRandom(int x);
    
    virtual void addCommand(string cmd, bool factory)=0;
    virtual Auto* createCommand(string cmd)=0;
    
    virtual void setResolution(int i)=0;
    virtual void setResolution(int w, int h)=0;

    
    bool safeMode = false;
    
    bool keyIsDown[255];
    
    ofxMidiOut midiOut;
    ofxMidiOut midiOutToLive;
    int programChangeFlag = 0;
    int lastProgram = 0;
    float currentParamIntensity = 1;
    
    float xOffset = 0, yOffset=0, xOffsetDelta = 0, yOffsetDelta =0;
    float trapeze = 0.;
    
    std::string controlFile = "";
    
    /** PIXELS **/

#ifndef LOW_ENERGY_MODE
//   int crt_WIDTH = 1080;
//   int crt_HEIGHT = 1080;
//    int crt_WIDTH = 2019*4/5;
//    int crt_HEIGHT = 1016*4/5;
    

    int crt_WIDTH = 1280;
    int crt_HEIGHT = 720;
//    int crt_WIDTH = 3545;
//    int crt_HEIGHT = 1200;
#else
    int crt_WIDTH = 1280/2;
    int crt_HEIGHT = 720/2;
#endif
    
//    int render_WIDTH = 2000;
//    int render_HEIGHT = 1000;
//        int render_WIDTH = 3545;
//        int render_HEIGHT = 1200;
    int render_WIDTH = crt_WIDTH;
    int render_HEIGHT = crt_HEIGHT;
    
    float FINALWIDTH = crt_WIDTH;
    float FINALHEIGHT = crt_HEIGHT;
    
    bool MULTIPROJECTOR = false;
    bool liveMode = false;
    bool dualDisplay = false;
    bool addMirorDisplay = true;
    bool attachedSecondDisplay = true;
    int attachedSecondDisplayWidth = 800;
        
    bool pause = false;

    float parameterMap[N_PARAM]; //1 usec d'appel en map
    float deltaMap[N_PARAM];
    float parameterEasingMap[N_PARAM];
    std::map<string, int> parameterNameMap;
    std::map<string, int> parameterNameAliasMap;
    std::vector<string> parameterIDMap;
    
    std::vector<int> parametersInGUI;
    std::map<int, ofVec2f> parametersInGUIBounds;
    
    vector<int> stressTestFilterList;
    
    ofImage i, audioImg, captureImg;
    
    int lastGlitchDate = 0;
    bool doGlitches = false;
    
    float currentFrameRate = 30;
#ifndef LOW_ENERGY_MODE
    float maxFrameRate = 60;
#else
    float maxFrameRate = 18;
#endif
    bool frameRateTurbo = false;
    
    bool savingGif = false;
    int gifCaptureStartFrame = 0;
    
    // -1 : bypass, 0 : record, 1 : render
    int recordAndRenderFlag = -1;
   long processingStartDate = 0;
   long processingStartFrame = 0;
    
    float extAutoDimmer = 1;
    bool audioOverOSC = false;
    
    float maxLedBrightness = 0.5;
    
    float currentTC = 0;
    bool saveMacroTC;//dirty flag;

    int featuredParameter = -1;

    ///TIME
    int localFrameNum = 0;

};

#endif
