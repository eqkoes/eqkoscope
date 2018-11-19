#include "eqkoscope.h"
#include "utils.h"
#include <ofSoundStream.h>

int coffset = 0;
float sig = 0;
bool sigSens = true;

float prev_audio[AUDIO_BUFFER_SIZE];

ofSoundStream mySoundStream;


void eqkoscope::initAudio(){
    
#ifdef VERBOSE
//    mySoundStream.listDevices();
#endif

#ifdef OF_10
    std::vector<ofSoundDevice> dvs =  mySoundStream.getDeviceList();
    int deviceIndex = 0;
    for(int d=0;d<dvs.size();d++){
        if(!dvs[d].name.compare("ASIO4ALL v2")){
            deviceIndex = d;
    break;
        }
    }
    
    mySoundStream.setDeviceID(deviceIndex);
#else
    mySoundStream.setDeviceID(0);

#endif

    mySoundStream.setup(0, 0, 44100, AUDIO_BUFFER_SIZE, 1);
//    mySoundStream.setup(0, 4, 44100, AUDIO_BUFFER_SIZE, 1);

    mySoundStream.setOutput(this);
    mySoundStream.setInput(this);

    
    for(int i=0;i<AUDIO_BUFFER_SIZE;i++)
        prev_audio[i] = -100;
}

void eqkoscope::audioRequested(float * output, int bufferSize, int nChannels){
    if(!parameterMap[audio]){
        #ifdef VIDEO_EXPORT
        if(savingGif && vidRecorder.isRecording()){
            vidRecorder.addAudioSamples(output, AUDIO_BUFFER_SIZE, 2); //we need to add dummy samples
        }
#endif
        return;
    }

    if(    mySoundStream.getNumOutputChannels()==0 )
        mySoundStream.setup(2, 0, 44100, AUDIO_BUFFER_SIZE, 1);

    if(nbFramesSinceAudioStart<10){ //debug hack
        nbFramesSinceAudioStart++;
        return;
    }
    
    int index=0;
    ofImage* img = &audioImg;
    float gain = parameterMap[audio];

    if(1==0){
        float freq = 1;
        float smooth = 1;
        float b = bufferSize*smooth;
        for(int i=0;i<bufferSize;i++){
            for(int chan=0;chan<nChannels;chan++){
                float s = 0;
                for(int j=0;j<smooth;j++){
                    int x,y;
                    coffset = audioCapture/2 - chan*audioCapture/4;
                    x = (audioCapture/2) + coffset*std::cos(2*PI*(i+ofGetFrameNum())/b);
                    y = (audioCapture/2) + coffset*std::sin(2*PI*(i+ ofGetFrameNum())/b);
                    ofColor c = img->getColor(x, y);
                    float tmp = max(c.r, max(c.b, c.g)) / (255.0);
                    s += tmp;
                }
                s /= smooth;
                //s = s*2 - 1; // on zero : zero*2 - 1 = -1
                NULL;//without DC offset correction (works on zero samples)
                output [i* nChannels + chan] = s*gain;
            }
        }
    }else{
        for(int i=0;i<bufferSize;i++){ //rasterization //TODO ADD RASTER or (+) (*)
            ofColor c = img->getColor(coffset%int(img->getWidth()), coffset/img->getWidth());
            float s =  2 * (c.r + c.b + c.g) / (3*255.0) - 0.5;
            coffset+=img->getHeight();
            if(coffset>img->getWidth()*img->getHeight())
                coffset = 0;
            for(int chan=0;chan<nChannels;chan++)
                output [i* nChannels] = s * gain;
        }
    }
    #ifdef VIDEO_EXPORT
    if(savingGif && vidRecorder.isRecording())
        vidRecorder.addAudioSamples(output, bufferSize, nChannels);
#endif
}

void eqkoscope::audioReceived(float *input, int bufferSize, int nChannels){
    if((!parameterMap[_audioIn] && audioAutos.size()==0) || ofGetFrameNum()<3 || audioOverOSC) return;
    
    int res = 4; //decimation
    float gain = pow(10, (parameterMap[audioGain])/20.0);
    for(int i=0;i<nChannels;i++){
        currentRms[i] = 0;
        for(int y = 0;y<bufferSize;y+=res)
            currentRms[i] += input[y+i]*input[y+i];
        currentRms[i] = sqrt(currentRms[i]/(bufferSize/(res)))*gain;
    }
}

void eqkoscope::analyzeAudio(){
    
    int i = ofGetFrameNum()*100 % WIDTH;
    detectLine(i, HEIGHT/2-10, i, HEIGHT/2+10);
//    detectLine(WIDTH, 0, WIDTH-1, HEIGHT-1);
//    detectLine(0, HEIGHT-1, WIDTH-1, HEIGHT-1);
//    detectLine(WIDTH-1, HEIGHT-1, WIDTH-1, HEIGHT-1);
//    detectCircle(i,HEIGHT/2, 25 + ((ofGetFrameNum()%HEIGHT/2) - 26));
    
    //taille details : freq
    // + random Pos
    //gros: ne detecte que les gros
    //petits : ne detectent que les petits !
    
//    float res = ofMap(ofGetMouseX(),0,WIDTH,25,250,true);
//    float entropy = 0;
//    float count = 0;
//    bool b = false;
//    for(int x=res;x<WIDTH-res;x+=res)
//        for(int y=res;y<HEIGHT-res;y+=res){
//            bool temp = detectCircle(x,y, res);
//            if(b!=temp)
//                entropy ++;
//            b = temp;
//            count ++;
//        }
//    
//    entropy /= count;
//    cout << "Entropy : " << entropy << endl;
//    for(float i=0.02;i<entropy;i+=0.02)
//        midiOut.sendNoteOn(1, int(ofRandom(36,52)));

    
//    midiOut.sendControlChange(1,10, skewAmp*127+skewVAmp*127);
    
    int res = 70;
    if(detectCircle(ofRandom(50, WIDTH-50), ofRandom(res, HEIGHT-res), res))
        midiOut.sendNoteOn(1, int(ofRandom(36,36))-24, 127);
    
    res = 25;
    if(detectCircle(ofRandom(res, WIDTH-res), ofRandom(res, HEIGHT-res), res))
        midiOut.sendNoteOn(1, int(ofRandom(42,54))-24, 127);
    
    res = 5;
    if(detectCircle(ofRandom(res, WIDTH-res), ofRandom(res, HEIGHT-res), res))
        midiOut.sendNoteOn(1, int(ofRandom(60,72))-24, 127);

}


bool eqkoscope::detectCircle(int x, int y, int rad){
    ofSetColor(ofColor::red);
    ofEllipse(x,y,rad,rad);
    bool circle = true;
    for(float a=0;a<2*PI;a+=2*PI/5.0){
        float xx = x + cos(a)*rad;
        float yy = y + sin(a)*rad;
        if(i.getColor(xx,yy).getBrightness()<100){
            circle = false;
            break;
        }
    }
    return circle;
}

bool eqkoscope::detectLine(int x1, int y1, int x2, int y2){
    ofSetColor(ofColor::red);
    ofLine(x1,y1,x2,y2);
    bool line = true;
    for(float a=0;a<1;a+=0.1){
        if(i.getColor(ofLerp(x1, x2, a), ofLerp(y1,y2,a)).getBrightness()<100){
            line = false;
            break;
        }
    }
    return line;
}


