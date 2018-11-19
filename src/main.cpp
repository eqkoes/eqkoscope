#include "ofMain.h"
#include "eqkoscope.h"
#include "ofGLProgrammableRenderer.h"
#include "ofAppGLFWWindow.h"

int main(int argc, char *argv[]){
    
#ifndef OF_10
    ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
#endif
    

    glfwInit();
    int displayCount = 1;
    glfwGetMonitors(&displayCount);


    eqkoscope* app = new eqkoscope();
    
    app->loadConfig();
    
    if(displayCount==1){
        app->dualDisplay = false;
    }
   
    
    int w = app->FINALWIDTH;
    int h = app->FINALHEIGHT;
    if(app->attachedSecondDisplay && app->dualDisplay){
        w += app->attachedSecondDisplayWidth;
    }
    
    if(app->dualDisplay && app->addMirorDisplay)
        w += app->FINALWIDTH;
    
    if(app->MULTIPROJECTOR){
        ofAppGLFWWindow window;
        window.setMultiDisplayFullscreen(true);
        ofSetupOpenGL(&window,w,h,OF_WINDOW); //Multi-projectors
    }else{
//        ofSetupOpenGL(w, h, OF_WINDOW);
//        ofWindowSettings st;
//        st.setSize(w, h);
//        ofGLWindowSettings settings(st); //test
#ifdef OF_10
        ofGLWindowSettings settings; //test
        settings.setGLVersion(3,2);
        ofCreateWindow(settings);
        ofSetWindowShape(w, h);
#else
              ofSetupOpenGL(w, h, OF_WINDOW);
#endif
        
    }
    glfwWindowHint(GLFW_DECORATED, false);

    
    if(app->liveMode){
        glfwWindowHint(GLFW_DECORATED, false);
        ofSetWindowPosition(0, 0);
    }
    
    if(app->dualDisplay){
        int x = -app->FINALWIDTH;
        if(app->addMirorDisplay){
            x -= app->FINALWIDTH;
        }
        ofSetWindowPosition(x, 0);
    }
//    ofSetWindowPosition(-WIDTH, 0); /@/hack for installation
    

    
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    
    ofRunApp(app);
    
}
