#ifndef emptyExample_Utils_h
#define emptyExample_Utils_h

/// SYSTEM OPTIONS

//#define OF_10

#ifndef OF_10
#define ofDrawRectangle ofRect
#endif


/// END OF SYSTEM OPTIONS

#define u_long unsigned long


#define WIDTH app->crt_WIDTH
#define HEIGHT app->crt_HEIGHT

///****************************************** RELEASE
#define EXPORT 1

//#define LOW_ENERGY_MODE 1

//reduce those values if you use very large resolutions
#define MAX_OMG3D2_FBOS 100
#define MAX_NB_ECHOES 5
#define NB_PAINT_FRAMES 10
#define CAMERA 1


#define STIENIS 0

#ifdef DEBUG
#define LOAD_ASYNC if(ofGetFrameNum()%25==0)  
#endif

#define HEIGHT2 HEIGHT // set to WIDTH for rect fbos


///****************************************** SPECIAL MODES
//#define NO_STROBE 1
//#define ATELIERS 1

//#define THE_CIRCLE 1

//#define DEBUG 1

#define BLUR_RESOLUTION 10

///****************************************** MIDI
#define MIDI_MAX_NB_PORTS 25

#define MACRO_APC
#ifdef MACRO_APC
#define MACRO_INTERFACE "APC Key 25"
#else
#define MACRO_INTERFACE "Launchpad S"
#endif

#define GERARD_INTERFACE "BCF2000"


#ifdef MACRO_APC
#define NB_MACRO_PER_PAGE 40
#else
#define NB_MACRO_PER_PAGE 128
#endif

//#define VERBOSE 1


///************************************************************** EXPORT
#ifndef EXPORT

//#define VIDEO_EXPORT 1


///***************************** SPECIAL DEVICES
#define _LEDS_STRIP 1

///***************************** INSTALLATIONS

//#define BRASERO 1

#ifdef BRASERO
#define KINECT 1
#endif

#define MAPPING 1
//#define GAME 1
//#define LEAPMOTION 1


///************************************************************** END EXPORT
#endif

//#define GENETIC 1

///****************************************** BUGGY STUFF
#define USE_FREEZE 1


///****************************************** OFFLINE RENDERING
//#define NO_SCENE_CUTS 1
//#define REAL_TIME_FX 1
//#define OFFLINE_RENDER 1

#define MAX_MACRO_PAGES 4
#define PHI 1.61803
#define AUDIO_BUFFER_SIZE 2048
//#define AUDIO_BUFFER_SIZE 512
#define BOOL_PARAM_EPSILON 0.01

#define OSC_INPUT 5555

///****************************************** UTILS
#define miiin(a,b) ((a) < (b) ? (a) : (b))
#define maaax(a,b) ((a) > (b) ? (a) : (b))

///****************************************** PRINT
#ifdef VERBOSE
#define DEBUG_TIME_1 long dbg_s = ofGetElapsedTimeMicros();
#define DEBUG_TIME_2 cout << "elapsed dbg time " << ofGetElapsedTimeMicros() - dbg_s << endl;
#else
#define DEBUG_TIME_1
#define DEBUG_TIME_2
#endif



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
                app->parameterMap[tintAmp] = 0.3;
            }
                break;
            case 2 :{
                app->parameterMap[tintSaturation] = 1;
                app->parameterMap[tintAmp] = 0.25;
            }
                break;
            default:
//                app->tintSaturation = 1;
                app->parameterMap[tintAmp] = 1;
        }
        setHue(ofMap(x, 0, 1, 0-app->parameterMap[tintAmp]/2, app->parameterMap[tintAmp]/2));
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

static float bassCO = 200, highCO=2000;

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
