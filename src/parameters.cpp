#ifndef eqkoscope_parameters_h
#define eqkoscope_parameters_h

#include "eqkoscope.h"

void AbstractApp::initParameters(){
    parameterMap[post_traitement] = 1;
    parameterMap[omg3D] = false;
    parameterMap[omg3DAngle] = 60;
    parameterMap[omg3D2] = 0;
    parameterMap[omg3D2Dist] = 1;
    parameterMap[omg3D2x2] = 0;
    parameterMap[omg3D2X] = 0;
    parameterMap[omg3D2Y] = 0;
    parameterMap[omg3D2Alpha0] = 1;
    parameterMap[omg3D2AlphaZ] = 1;
    parameterMap[omg3D2RollerX] = 0;
    parameterMap[omg3D2RollerY] = 0;
    parameterMap[omg3D2Symetry] = 0;
    parameterMap[omg3D2Rotation] = 0;
    parameterMap[omg3D2Acc] = 0;
    parameterMap[omg3D2DOF] = 0;
    
    parameterMap[scan] = 0;
    
    parameterMap[divergence] = 0;
    parameterMap[divergenceZ] = 0;
    parameterMap[divergenceY] = 0;
    parameterMap[omg3D2Speed] = 0.005;
    parameterMap[omg3D2FreeRotation] = false;
    parameterMap[omg3D2AvoidCenter] = 0;
    parameterMap[aDivergence] = 0;
    parameterMap[omg3D2Strobe] = 0;
    parameterMap[omg3D2Depth] = 1;
    parameterMap[thresholdAfterBall] = false;
    parameterMap[glowResolution] = 25;
    parameterMap[borderMask] = 0;
    parameterMap[borderMaskd0] = 0;
    parameterMap[jpgGlitch] = 0;
    parameterMap[keying] = 0;
    parameterMap[background] = 0;
    parameterMap[engraving] = 0;
    
    parameterMap[randHHide]=0;
    parameterMap[randVHide]=0;
    parameterMap[glow] = false;
    parameterMap[sobel] = false;
    parameterMap[sobelPostFX] = false;
    parameterMap[sobelMix] = 0;
    parameterMap[whiteSobel] = false;
    parameterMap[gradient] = false;
    
    parameterMap[_gamma] = 1;
    parameterMap[contrast] = 0;
    parameterMap[_invert] = false;
    parameterMap[vblur] = 0;
    parameterMap[hblur]= 0;
    parameterMap[tiltShift]= 0;
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
    parameterMap[flash] = false;
    parameterMap[camera] = 0; //-1 : dual
    
    parameterMap[warp] = 0;
    
    parameterMap[mediaX] = 0;
    parameterMap[mediaY] = 0;
    parameterMap[mediaZ] = 0;
    parameterMap[mediaRotX] = 0;
    parameterMap[mediaRotY] = 0;
    parameterMap[mediaRotZ] = 0;
    
    
    parameterMap[paint] = false;
    parameterMap[paintSizeDelta] = 0.75;
    parameterMap[multiFbos] = false;
    
    /** COLOR MGMT **/
    parameterMap[tintSaturation]=1;
    parameterMap[tintHue]=0;
    parameterMap[gradient]=0;
    parameterMap[tintBrightness] = 1;
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
    
//    parameterMap[stripesAmp]=0.;
//    parameterMap[stripesSize]=0.1;
    parameterMap[skewAmp]=0.;
    parameterMap[skewDAmp]=0.;
    parameterMap[skewAAmp]=0.;
    parameterMap[skewVAmp]=0.0;
    parameterMap[skewSpeed]=0.2;
    parameterMap[skewBorderCopy]=false;
    parameterMap[displaceAmp] = 0.;
    parameterMap[displaceVAmp]=0.;
    parameterMap[displaceProba] = 0.1;
    parameterMap[chromaOffset] = 0;
    parameterMap[chromaSepAngle]=0;
    parameterMap[chromaSepHue]=0;
    parameterMap[chromaSep]=0.0;
    parameterMap[chromaSepAlpha]=1.0;
    parameterMap[chromaSepMode]=0;
    parameterMap[doubleChromaSep] = false;
    parameterMap[chromaSens] = true;
    parameterMap[sortXThresh] = 0;
    parameterMap[sortYThresh] = 0;
    
    parameterMap[audio] = 0;
    parameterMap[_audioIn] = 0;
    
    parameterMap[selSat] = 0;
    
    parameterMap[dualFocus] = -1;
    parameterMap[dualFocusIntensity] = 4;
    
    parameterMap[currentScene] = 0;
    parameterMap[antiAliasing] = 0;
    
    parameterMap[selectiveShading] = 0; //0: all, 1: scene 0 only, -1: scene 1 only, etc...
    
    
    parameterMap[_loadMacro] = 0;
    parameterMap[macroFade] = 0;
    parameterMap[skewJumpRate]= 0.4; //B4 0.025
    parameterMap[skewHard] = 0;
    parameterMap[skewBandingProb] = 0.025;
    parameterMap[skewBandingJumpProb] = 0.25;
    
    parameterMap[omg3D2HardRotation] = 0;
    parameterMap[omg3D2Nb] = 15;
    
    parameterMap[kaleiCopy]=0;
    parameterMap[kaleiCopyAdjust]=0;
    parameterMap[warp]=0;
//    parameterMap[warpRemap] =1;
//    parameterMap[warpX]=0;
//    parameterMap[warpY]=0;
    
    parameterMap[stressTest] = 0;
    parameterMap[stressTestRate] = 1;
    
    parameterMap[reSaturate] = 0;
    
    parameterMap[prollage] = 0.02;
    
    parameterMap[dispose] = 0;
    parameterMap[disposeFreq] = 0.2;
    parameterMap[disposeX] = 0.2;
    parameterMap[leapAutoReco] = 0;
    
    parameterMap[hueShift] = 0.;
    parameterMap[hueShiftCenter] = 0;
    parameterMap[hueShiftRange] = 0.1;
    parameterMap[hueShiftCrazy] = 0;
    parameterMap[hueKalei] = 0.;

    parameterMap[kaleiMirror] = 0;
    
    parameterMap[omg3D2ZScale] = 0;
    parameterMap[omg3D2LumaKey] = 1;
    
    parameterMap[invertCircle] = 0;

    
    parameterMap[debug] = 0;
    
    parameterMap[lcd] = 0;

    
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
    parameterMap[feedBack] = 0;
    
    parameterMap[nbPoints]=0;
    parameterMap[mediaAlpha] = 1;
    parameterMap[blackCenter] = true;
    parameterMap[customBorders]=0;
//    parameterMap[pointy]=0;
    parameterMap[feedbackRemanence] = 1;
    
    parameterMap[noSource] = false;
    parameterMap[erode] = false;
    parameterMap[fastErode] = 1;
    
    parameterMap[threshold] = 1;
    
    parameterMap[rot] = 0;
    parameterMap[upRot] = 0;
    parameterMap[pitchRot] = 0;
    parameterMap[scale] = 0.95;
//    parameterMap[offx]=0;
//    parameterMap[offy]=0;
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
    
    parameterMap[tiles] = 0;
    
    /** PRINT **/
    parameterMap[randomText] = false;
    parameterMap[seqText] = false;
    parameterMap[randomFont] = true;
    parameterMap[txt_randPosition] = false;
    parameterMap[txt_randSize] = 0;
    parameterMap[nWords] = 1;
    parameterMap[txt_accumulate] = false;
    parameterMap[txt_fill] = false;
    parameterMap[fontSize] = 70;
    parameterMap[txt_rate] = 387; //ms
    parameterMap[wordLimit] = 1000;

    
    /** LINES **/
    parameterMap[updateLen] = 200;
    parameterMap[speed] = 1;
    parameterMap[lines_yWeight] = 1;
    parameterMap[lines_zWeight] = 1;
    parameterMap[lines_zSquare] = 0;
    parameterMap[lines_ySquare] = 0;
    parameterMap[lines_yres] = 0.1;
    parameterMap[lines_mesh] = 1; //choose between polyline or mesh : to be tested
   
    /** CINEMA **/
    parameterMap[crtPos]=0;
    parameterMap[movieSpeed] = 1;
    parameterMap[backMask] = false;
    parameterMap[adjust] = false;
    parameterMap[loopLength] = 0;
    parameterMap[loopStart] = 0;
    parameterMap[loopMode] = 0; //1 : fwdrev.must use inv !
    parameterMap[supZoom] = 1;
    parameterMap[supRot] = 0;
    parameterMap[jumpTo] = 0;
    parameterMap[hMirror] = false;
    parameterMap[vMirror] = false;
    parameterMap[paintResolution] = 0.01;
    parameterMap[selectedPlayer] = 0;
    parameterMap[syncToTempo] = false;
    parameterMap[circle] = 0; //1 : circle
    parameterMap[circleSpeed] = 1/20.0;
    parameterMap[circleRotation] = 1;
    parameterMap[circleDist] = 1;
    parameterMap[macro] = 0;
    parameterMap[blendType] = ALPHABLACK;
    parameterMap[doubleFeedback] = false;
    parameterMap[randomSpeed] = 0;
    parameterMap[selectedPlayer] = 0;

    /** UZI **/
    parameterMap[switchImg] = false;
    parameterMap[autoSwitchImg] = false;
    parameterMap[randomUzi] = false;
    parameterMap[uziPeriod] = 4;
    parameterMap[doubleInversion] = false;
    parameterMap[backMask] = false;
    parameterMap[_mask] = false;
    parameterMap[resize] = true;
    parameterMap[autoRot] = 0;
    parameterMap[autoRotFreq] = 1;
    parameterMap[ak47Mode] = false;
    parameterMap[ak47Frame] = false;
    parameterMap[lastAk47Index] = 0;
    parameterMap[swapTint] = 0;
    parameterMap[transluscentUzi] = 0;
    
    /** DRAW **/
    parameterMap[draw_recording] = 0;
    parameterMap[draw_maxConnectionLength] = 80; //should be in pts
    parameterMap[draw_minConnectionLength]  = 5;
    parameterMap[draw_pace]  = 0.05;
    parameterMap[draw_hold]  = 0;
    
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
    
    parameterMap[bw] = 0;
    parameterMap[bwOffset] = 0;
    parameterMap[draw_freskSpeed] = 1;
    
    parameterMap[draw_consecutive] = 0;
    //        parameterMap[draw_simplify] = 0;
    
    parameterMap[frameRate] = 30;
    parameterMap[draw_destruction] = 0;
    
    parameterMap[otherWorlds] = 0;
    parameterMap[mediaScaleX] = 1;
    parameterMap[mediaScaleY] = 1;
    
    /** GLITCH **/
    parameterMap[glitch_mode] = PTs;
    parameterMap[glitch_thresh] = 1;
    
    /** MAPPED **/
     parameterMap[map_event] = 1;
     parameterMap[map_remanence] = 0.67;
     parameterMap[map_mode] = 101;
     parameterMap[map_sync]  = SYNC_INDIV;
     parameterMap[map_pace]  = 0.005;
     parameterMap[map_prog]  = 0;
     parameterMap[map_mask] = false;
 
    /** KINECT **/
    parameterMap[kinect] = 0;
    parameterMap[kinectScale] = 1;
    parameterMap[kinectMasking] = false;
    parameterMap[kinectRender] = 1;
    
    /** UTILS **/
    parameterMap[randomJump] = 0;
    
    /** ECHOES **/
    parameterMap[echoRandom] = 0;
    parameterMap[echoNb] = 0;
    parameterMap[echoPeriod] = 1000;
    parameterMap[drawBrightDist] = 1;
    parameterMap[echoAdjust] = 1;
    
    /** LED **/
    parameterMap[ledMode]=0;
    parameterMap[ledSync]=0;
    parameterMap[ledPeriod]=1000/maxFrameRate;
    parameterMap[ledInfo]=0;
    parameterMap[ledBrightness]=1.0;
    parameterMap[ledEvent] = 0;
    parameterMap[ledTint] = 180;
    parameterMap[ledSaturation] = 255;
    parameterMap[ledStrobe] = 0;
    parameterMap[ledAuto] = 0;
    parameterMap[uvLight] = 0;
    
    /** USER **/
    parameterMap[user1] = 0;
    parameterMap[user2] = 0;
    parameterMap[user3] = 0;
    parameterMap[user4] = 0;
    parameterMap[user5] = 0;

    /** PERTURBATION **/
    parameterMap[pnoise] = 0;
    parameterMap[pert] = 0;
    parameterMap[pertEvo] = 0;
    parameterMap[pertEvoAuto] = 0;
    parameterMap[pertPersistance] = .5;
    parameterMap[pertFreq] = 5;
    parameterMap[pertMode] = 0;
    parameterMap[pertHue] = 0;
    
    parameterMap[glitchFreq] = 45;
    
    parameterMap[fadeOutMode] = 1;
    
    parameterMap[useJoyStick] = 0;
    
    parameterMap[noise] = 0;
    
    parameterMap[mandala] = 0;
    parameterMap[mandalaBorder] = 0;
    parameterMap[squareMandala] = 0;
    
    parameterMap[hueFilter] = 0;
    parameterMap[psyShift] = 0;
    
    parameterMap[bypassCTRL] = 0;
   
    parameterMap[whitePoint] = 1;
    parameterMap[blackPoint] = 0;
    
    parameterMap[typhoon] = 0;
    parameterMap[taijin] = 0;
    parameterMap[illu]  = 0;
    parameterMap[surprise]  = 0;

    parameterMap[oscIn] = OSC_INPUT;
    
    for(int i=0;i<N_PARAM;i++)
        deltaMap[i] = parameterMap[i];
    }

void AbstractApp::randomParameters(){
    initParameters();
    for(int p=0;p<=N_PARAM;p++){
        if(std::find(stressTestFilterList.begin(), stressTestFilterList.end(),p)==stressTestFilterList.end()){
        float v = 0;
            if(ofRandom(1)<0.10){
                if(p==_gamma || p==tintSaturation)
                    v = 1+ofRandom(-0.5,0.5);
                else
                    v = ofRandom(1);
            }
        deltaMap[p] = parameterMap[p] = v;
        }
    }
    deltaMap[_gamma] = parameterMap[_gamma] = 1;
}

#endif
