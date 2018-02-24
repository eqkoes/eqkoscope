#ifndef emptyExample_Utils_h
#define emptyExample_Utils_h

//#define MACOSX_CORE 1

#define WIDTH app->crt_WIDTH
#define HEIGHT app->crt_HEIGHT


//#define WIDTH 1688 //MAPPING DUMA
//#define HEIGHT 512

//#define WIDTH 1900 //HD
//#define HEIGHT 1080
//#define HEIGHT2 WIDTH // set to WIDTH for rect fbos

#define HEIGHT2 HEIGHT // set to WIDTH for rect fbos

#define CAMERA 1

/** SPECIAL MODES **/

//#define NO_STROBE 1
#define ATELIERS 1

//#define GENETIC 1

#define DEBUG 1


#define INSTALL_COCHON 1

//#define INSTALL_THOUGHTS 1
#ifdef INSTALL_THOUGHTS
#define MAPPING 1
#endif

#define MPD24_DEVICE "Akai MPD24"
#define LAUNCHCONTROL_DEVICE "Launch Control XL"

//#define ROZLAV 1
#ifndef ROZLAV
#define MAPPING 1
//#define GAME 1
#define LEAPMOTION 1
//#define BRASERO 1
    #ifdef BRASERO
    #define KINECT 1
    #endif

#define _LEDS_STRIP 1
//#define LAZERS 1
#define USE_FREEZE 1
//#define REAL_TIME_FX 1
#define MPD_DEVICE "io|2"

#else
#define MPD_DEVICE "P\216riph\216rique MIDI USB"
#endif

#define NO_SCENE_CUTS 1

//#define OFFLINE_RENDER 1

#define MASTER 1
#define SLAVE 1

#define EXTERNAL_MIDI_DEVICE "QUAD-CAPTURE"
#define NOVATION_DEVICE "SL MkII Port"

//#define MPD24
//#define CPU_SKEW

#define MAX_MACRO_PAGES 4
#define PHI 1.61803
#define AUDIO_BUFFER_SIZE 2048
//#define AUDIO_BUFFER_SIZE 512
#define BOOL_PARAM_EPSILON 0.01
//#define USE_PRELOADED_MOVIES 1

#define OSC_INPUT 8888

//UTILS
#define miiin(a,b) ((a) < (b) ? (a) : (b))
#define maaax(a,b) ((a) > (b) ? (a) : (b))

#define DEBUG_TIME_1 long dbg_s = ofGetElapsedTimeMicros();
#define DEBUG_TIME_2 cout << "elapsed dbg time " << ofGetElapsedTimeMicros() - dbg_s << endl;

#include <ofXml.h>
#include "AbstractApp.h"

/** TINT */

class Utils{
public:
    Utils(AbstractApp *app){
        this->app = app;
    }
     void setHue(float h){
        app->parameterMap[tintHue] = h;
    }
    
     float getAdjustedBrightness(){
        ofColor c = ofColor::fromHsb(app->parameterMap[tintHue]*255, 255, 255);
        float r = c.r/255.0;
        float g = c.g/255.0;
        float b = c.b/255.0;
        float perceivedLightness = (0.2126*r + 0.7152*g + 0.0722*b);
        float br = 1.05 - perceivedLightness/4;
        return br;
    }
    
     void setTintHue(double x){
        x = x<0 ? 0 : x;
        switch((int) app->parameterMap[tintMode]){
            case 0 :{ app->parameterMap[tintSaturation] = 0;
                app->parameterMap[tintAmp]=0;
                return;
            }
            case 1 :{
                app->parameterMap[tintSaturation] = 1;
                app->parameterMap[tintCenter] = 0.85; //blue
                app->parameterMap[tintAmp] = 0.3;
            }
                break;
            case 2 :{
                app->parameterMap[tintSaturation] = 1;
                app->parameterMap[tintCenter] = 0.125; //red
                app->parameterMap[tintAmp] = 0.25;
            }
                break;
            default:
//                app->tintSaturation = 1;
                app->parameterMap[tintCenter] = 0.5;
                app->parameterMap[tintAmp] = 1;
        }
        setHue(ofMap(x, 0, 1, app->parameterMap[tintCenter]-app->parameterMap[tintAmp]/2,app->parameterMap[tintCenter]+app->parameterMap[tintAmp]/2));
    }
    
     void setTintMode(int mode){
        app->parameterMap[tintMode] = mode;
        setTintHue(app->parameterMap[tintHue]);
    }
    
     float getTintHue(){
        return app->parameterMap[tintHue];
    }
    
     float getTintSaturation(){
        if(app->parameterMap[tintMode]==0)
            return 0;
        return app->parameterMap[tintSaturation];
    }
    
    private :
    AbstractApp *app;
    
};




/** SOUND */

float bassCO = 200, highCO=2000;

static int ofConstrain(int amt, int low, int high) {
	return (amt < low) ? low : ((amt > high) ? high : amt);
}

template<typename T>
static void erase(std::vector<T> vec, T *elt){
    vec->erase(std::remove(vec->begin(), vec->end(), (*elt)), vec->end());
}

/** STRINGS */

static bool strStartsWith(std::string str, std::string foo){
    if(foo.size()>str.size())
        return false;
    return !(str.substr(0, foo.length()).compare(foo));
}

static bool strEndsWith(std::string str, std::string foo){
    if(foo.size()>str.size())
        return false;
    return !(str.substr(str.length()-foo.length(), foo.length()).compare(foo));
}

/** PARAMS */

static void saveParameter(stringstream* macro, string name, float value){
    (*macro) << name << "," << value << endl;
}

static void saveParameter(stringstream* macro, string name, string value){
    (*macro) << name << value << endl;
}

static void saveParameterXML(stringstream* macro, string name, float value){
    (*macro) << "\t<" << name << ">" << value << "</" << name << ">" << endl;
}

static void saveParameterXML(stringstream* macro, string name, string value){
    (*macro) << "\t<" << name << ">" << value << "</" << name << ">" << endl;
}
#endif
