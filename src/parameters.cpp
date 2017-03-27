#ifndef eqkoscope_parameters_h
#define eqkoscope_parameters_h

#include "eqkoscope.h"

void eqkoscope::initParameters(){
    parameterMap[post_traitement] = 1;
    parameterMap[omg3D] = false;
    parameterMap[omg3DAngle] = 60;
    parameterMap[test] = false;
    parameterMap[test2] = false;
    parameterMap[test3] = false;
    parameterMap[omg3D2] = 0;
    parameterMap[omg3D2Dist] = 0.5;
    parameterMap[omg3D2X] = 0;
    parameterMap[omg3D2Y] = 0;
    parameterMap[omg3D2Alpha0] = 1;
    parameterMap[omg3D2AlphaZ] = false;
    parameterMap[omg3D2RollerX] = 0;
    parameterMap[omg3D2RollerY] = 0;
    parameterMap[omg3D2Symetry] = false;
    parameterMap[omg3D2Rotation] = 0;
    parameterMap[omg3D2Divergence] = 0;
    parameterMap[omg3D2YDivergence] = 0;
    parameterMap[omg3D2Speed] = 0.005;
    parameterMap[omg3D2FreeRotation] = false;
    parameterMap[omg3D2AvoidCenter] = 0;
    parameterMap[omg3D2Scale] = 0;
    parameterMap[omg3D2Strobe] = 0;
    parameterMap[omg3D2Depth] = 1;
    parameterMap[ball] = false;
    parameterMap[thresholdAfterBall] = false;
    parameterMap[glowResolution] = 25;
    parameterMap[glowIntensity] = 1;
    parameterMap[borderMask] = 0;
    
    parameterMap[randHHide]=0;
    parameterMap[randVHide]=0;
    parameterMap[glow] = false;
    parameterMap[sobel] = false;
    parameterMap[sobelMix] = 0;
    parameterMap[gradient] = false;
    
    parameterMap[_gamma] = 1;
    parameterMap[contrast] = 0;
    parameterMap[_invert] = false;
    parameterMap[vblur] = 0;
    parameterMap[hblur]= 0;
    parameterMap[blurOriginalMix] = 1;
    parameterMap[sharpen] = 0;
    parameterMap[sharpenMode] = 0;
    parameterMap[nBlocks] = 0;
    parameterMap[nFreeze] = 0;
    
    parameterMap[strobe] = false;
    parameterMap[xpixellate] = 0;
    parameterMap[ypixellate]=0;
    parameterMap[embedUzi] = false;
    parameterMap[embedCinema] = false;
    parameterMap[threeDeeScene] = false;
    parameterMap[toLine] = 0;
    parameterMap[toCircle] = 0;
    parameterMap[lineDa] = 0.75;
    parameterMap[lineDx] = 0;
    parameterMap[blendType] = MULTIPLY;
    parameterMap[flash] = false;
    parameterMap[triumMode] = 0; //-1 : dual
    parameterMap[triumSpace1] = 0;
    parameterMap[triumSpace2] = 0;
    parameterMap[triumSpace3] = 0;
    
    parameterMap[warp] = 0;
    
    parameterMap[mediaX] = 0;
    parameterMap[mediaY] = 0;
    parameterMap[mediaZ] = 0;
    parameterMap[mediaRotX] = 0;
    parameterMap[mediaRotY] = 0;
    parameterMap[mediaRotZ] = 0;
    
    
    parameterMap[paint] = false;
    parameterMap[paint2] = false;
    parameterMap[multiFbos] = false;
    
    /** COLOR MGMT **/
    parameterMap[tintSaturation]=1;
    parameterMap[tintHue]=0;
    parameterMap[tintBrightness] = 1;
    parameterMap[black] = false;
    parameterMap[tintCenter]=0;
    parameterMap[tintAmp] = 1;
    parameterMap[sidesSaturation] = 0.8;
    parameterMap[_reTint] = 0;
    parameterMap[tintMode] = -1;
    parameterMap[randomTint] = false;
    
    
    parameterMap[bpm] = 120;
    parameterMap[bpmLock] = 0;
    
    /** KALEIDO**/
    parameterMap[kalei] = 0;//
    parameterMap[kaleiCopyBorder] = 1;//
    parameterMap[kaleiNb] = 1;
    parameterMap[kalei_2] = 0;//
    parameterMap[kaleiNb_2] = 8;
    parameterMap[kaleiOffX] = 0;
    parameterMap[kaleiOffY] = 0;
    parameterMap[kaleiScale] = 1;
    
    parameterMap[stripesAmp]=0.;
    parameterMap[stripesSize]=0.1;
    parameterMap[skewAmp]=0.;
    parameterMap[skewDAmp]=0.;
    parameterMap[skewAAmp]=0.;
    parameterMap[skewVAmp]=0.0;
    parameterMap[skewSpeed]=0.2;
    parameterMap[skewVAmpMidi];
    parameterMap[skewAmpMidi];
    parameterMap[skewBorderCopy]=false;
    parameterMap[displaceAmp] = 0.;
    parameterMap[displaceVAmp]=0.;
    parameterMap[displaceProba] = 0.1;
    parameterMap[chromaOffset] = 100;
    parameterMap[chromasepAngle]=0;
    parameterMap[chromasepHue]=50;
    parameterMap[chromaSep]=0.0;
    parameterMap[chromaSepAlpha]=1.0;
    parameterMap[chromaSepMode]=0;
    parameterMap[doubleChromaSep] = false;
    parameterMap[chromaSens] = true;
    parameterMap[sortXThresh] = 0;
    parameterMap[sortYThresh] = 0;
    
    parameterMap[audio] = 0;
    parameterMap[_audioIn] = 0;
    
    parameterMap[carder] = 0;
    
    parameterMap[dualFocus] = -1;
    parameterMap[dualFocusIntensity] = 4;
    
    parameterMap[currentScene] = 0;
    parameterMap[antiAliasing] = 1;
    
    parameterMap[FXMode] = 0; //0: all, 1: scene 0 only, -1: scene 1 only, etc...
    
    
    parameterMap[skewUpdateRate] = 0;
    parameterMap[skewJumpRate]= 0.4; //B4 0.025
    parameterMap[skewHard] = 0;
    parameterMap[skewBandingProb] = 0.025;
    parameterMap[skewBandingJumpProb] = 0.25;
    
    parameterMap[omg3D2HardRotation] = 1;
    parameterMap[omg3D2Nb] = 15;
    
    parameterMap[kaleiCopy]=0;
    parameterMap[kaleiCopyAdjust]=0;
    parameterMap[warp]=0;
    parameterMap[warpRemap] =1;
    parameterMap[warpX]=0;
    parameterMap[warpY]=0;
    
    parameterMap[stressTest] = 0;
    parameterMap[stressTestRate] = 1;
    
    parameterMap[reSaturate] = 1;
    
    parameterMap[prollage] = 0.02;
    
    parameterMap[dispose] = 0;
    parameterMap[disposeFreq] = 0.2;
    parameterMap[disposeX] = 0.2;
    parameterMap[leapAutoReco] = 0;
    
    parameterMap[hueShift] = 0.;
    parameterMap[hueShiftCenter] = 0;
    parameterMap[hueShiftRange] = 0.1;
    parameterMap[hueShiftCrazy] = 0;
    
    parameterMap[kaleiMirror] = 0;
    
    parameterMap[omg3D2ZScale] = 0;
    parameterMap[omg3D2LumaKey] = 1;
    
    parameterMap[invertCircle] = 0;

    
    parameterMap[debug] = 0;
    
    
    
    /** AGENTS **/
    parameterMap[agentSpeed] = 0.25;
    parameterMap[agentNb] = 25;
    parameterMap[agentShapeNb] = 3;
    parameterMap[agentShapeNbRand] = 0;
    parameterMap[agentNbShapes] = 4;
    parameterMap[agentFill] = 0;
    parameterMap[agentRandDist] = 0;
    parameterMap[agentAngleFreq] = 1;
    
    /** FEEBACK **/
    parameterMap[feedMode] = MODE_FEEDBACK;
    
    parameterMap[nbPoints]=0;
    parameterMap[mediaAlpha] = 1;
    parameterMap[mediaSaturation] = 1;
    parameterMap[blackCenter] = true;
    parameterMap[pointx]=0;
    parameterMap[pointy]=0;
    parameterMap[feedbackRemanence] = 1;
    
    parameterMap[noSource] = false;
    parameterMap[erode] = false;
    parameterMap[fastErode] = 1;
    
    parameterMap[threshold] = 1;
    
    parameterMap[rot] = 0;
    parameterMap[upRot] = 0.1;
    parameterMap[pitchRot] = 0;
    parameterMap[scale] = 0.95;
    parameterMap[offx]=0;
    parameterMap[offy]=0;
    parameterMap[pace] = 0.1f;
    
    parameterMap[f_strobe] = 0;
    parameterMap[f_invertFrame] = false;
    parameterMap[doubleInversion] = false;
    parameterMap[backMask] = true;
    
    parameterMap[curShapeRot] = 0.1;
    parameterMap[shapeRot] = 0.1;
    parameterMap[shapeNbPts] = 3;
    parameterMap[shapeStyle] = GEO;
    parameterMap[shapeWeight]=1;
    parameterMap[randomShapeColor] = false;
    
    /** PRINT **/
    parameterMap[randomText] = false;
    parameterMap[seqText] = true;
    parameterMap[randomFont] = true;
    parameterMap[txt_randPosition] = false;
    parameterMap[txt_randSize] = 0;
    parameterMap[nWords] = 1;
    parameterMap[txt_accumulate] = false;
    parameterMap[txt_fill] = false;
    parameterMap[fontSize] = 125;
    parameterMap[txt_rate] = 387; //ms
    parameterMap[wordLimit] = 1000;

    
    /** LINES **/
    parameterMap[updateLen] = 200;
    parameterMap[speed] = 10;
    parameterMap[shapeWeight] = 0;
    parameterMap[lines_yWeight] = 1;
    parameterMap[lines_zWeight] = 1;
    parameterMap[lines_zSquare] = 0;
    parameterMap[lines_ySquare] = 0;
    parameterMap[lines_yres] = 0.1;
    parameterMap[lines_mesh] = 1; //choose between polyline or mesh : to be tested
   
    /** CINEMA **/
    parameterMap[crtPos]=0;
    parameterMap[posterize] = 255;
    parameterMap[movieSpeed] = 1;
    parameterMap[backMask] = false;
    parameterMap[brightness]=1;
    parameterMap[adjust] = false;
    parameterMap[loopLength] = 0;
    parameterMap[loopStart] = 0;
    parameterMap[loopMode] = 0; //1 : fwdrev.must use inv !
    parameterMap[supZoom] = 1;
    parameterMap[supRot] = 0;
    parameterMap[jumpTo] = 0;
    parameterMap[hMirror] = false;
    parameterMap[vMirror] = false;
    parameterMap[mixer] = true;
    parameterMap[selectedPlayer] = 0;
    parameterMap[syncToTempo] = false;
    parameterMap[circle] = 0; //1 : circle
    parameterMap[circleSpeed] = 1/20.0;
    parameterMap[circleRotation] = 1;
    parameterMap[circleDist] = 1;
    parameterMap[z] = 1;
    parameterMap[postT] = true;
    parameterMap[blendType] = TRUED;
    parameterMap[feedback] = false;
    parameterMap[randomSpeed] = 0;
    parameterMap[selectedPlayer] = 0;

    /** UZI **/
    parameterMap[switchImg] = false;
    parameterMap[autoSwitchImg] = false;
    parameterMap[randomUzi] = false;
    parameterMap[uziPeriod] = 4;
    parameterMap[nextImg] = false;
    parameterMap[doubleInversion] = false;
    parameterMap[backMask] = false;
    parameterMap[roundMask] = false;
    parameterMap[resize] = true;
    parameterMap[kenBurns] = 0;
    parameterMap[kenBurnsZ_end] = 1;
    parameterMap[kenBurnsZ] = 0.0;
    parameterMap[kenBurnsZ_pos] = 0;
    parameterMap[kenBurnsZ_min] = 1.;
    parameterMap[kenBurnsZ_max] = 1.5;
    parameterMap[autoRot] = 0;
    parameterMap[autoRotFreq] = 1;
    parameterMap[ak47Mode] = false;
    parameterMap[ak47Frame] = false;
    parameterMap[lastAk47Index] = 0;
    parameterMap[parallax] = 0;
    parameterMap[parallax_res] = 1;
    parameterMap[swapTint] = 0;
    parameterMap[transluscentUzi] = 0;
    
    /** DRAW **/
    parameterMap[draw_recording] = 0;
    parameterMap[draw_maxConnectionLength] = 80; //should be in pts
    parameterMap[draw_minConnectionLength]  = 5;
    parameterMap[draw_pace]  = 0.05;
    parameterMap[draw_divergence]  = 0;
    
    parameterMap[draw_destroy]  = 0;
    parameterMap[draw_destroyMode]  = 0;
    parameterMap[draw_recInterval]  = 7; //should be in pts
    parameterMap[draw_currentDrawing]  = 0;
    parameterMap[draw_random]  = 0.1;
    parameterMap[draw_randomRate]  = 5;
    parameterMap[draw_lines] = 1;
    parameterMap[draw_points] = 0;
    parameterMap[draw_fresk] = 0;
    parameterMap[draw_ox] = 0;
    parameterMap[draw_oy] = 0;
    parameterMap[draw_oz] = 0;
    parameterMap[draw_zSpeed] = 0;
    
    parameterMap[draw_rotY] = 0;
    parameterMap[draw_freskSpeed] = 1;
    
    parameterMap[draw_consecutive] = 0;
    //        parameterMap[draw_simplify] = 0;
    
    parameterMap[fractalMode] = 2;
    parameterMap[draw_destruction] = 0;
    
    /** FRACTALS **/
    parameterMap[fractalP1] = 0;
    parameterMap[fractalP2] = 0.5;
    parameterMap[fractalScale] = 1;
    parameterMap[fractalMode] = 0;
    parameterMap[fractalIterations] = 60;
    
    /** GLITCH **/
    parameterMap[glitch_mode] = PTs;
    parameterMap[glitch_thresh] = 1;
    
    /** MAPPED **/
     parameterMap[map_event] = 1;
     parameterMap[map_remanence] = 0;
     parameterMap[map_mode] = BLINK;
     parameterMap[map_sync]  = SYNC_INDIV;
     parameterMap[map_pace]  = 0.05;
     parameterMap[map_prog]  = 0;
    
    /** KINECT **/
    parameterMap[kinect] = 0;
    parameterMap[kinectScale] = 1;
    parameterMap[kinectMasking] = false;
    parameterMap[kinectRender] = 1;
    
    /** UTILS **/
    parameterMap[MIDIMappingAutoLoad] = 0;
    
    /** ECHOES **/
    parameterMap[echoRandom] = 0;
    parameterMap[echoNb] = 0;
    parameterMap[echoPeriod] = 1000;
    parameterMap[drawBrightDist] = 1;
    parameterMap[echoAdjust] = 1;
    
    /** LED **/
    parameterMap[ledMode]=12;
    parameterMap[ledSync]=0;
    parameterMap[ledPeriod]=1000/restrictFrameRate;
    parameterMap[ledInfo]=0;
    parameterMap[ledBrightness]=0.25;
    parameterMap[ledEvent] = 0;
    parameterMap[ledTint] = -1;
    
    /** USER **/
    parameterMap[user1] = 0;
    parameterMap[user2] = 0;
    parameterMap[user3] = 0;
    parameterMap[user4] = 0;
    parameterMap[user5] = 0;

    /** PERTURBATION **/
    parameterMap[pert] = 0;
    parameterMap[pertEvo] = 0;
    parameterMap[pertPersistance] = .5;
    parameterMap[pertFreq] = 10;
    
    parameterMap[glitchFreq] = 45;
    }

void eqkoscope::randomParameters(){
    for(int p=0;p<=parameterNameMap.size();p++){
        float v = 0;
        if(ofRandom(1)<0.05)
            v = ofRandom(1);
        deltaMap[p] = parameterMap[p] = v;
    }
}

#endif
