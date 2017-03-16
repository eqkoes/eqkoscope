#include "ofMain.h"
#include "eqkoscope.h"
#include "ofGLProgrammableRenderer.h"
#include "ofAppGlutWindow.h"
#include "ofAppGLFWWindow.h"

int main(int argc, char *argv[]){
    ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
    
    CGDisplayCount displayCount;
    CGDirectDisplayID displays[32];
    CGGetActiveDisplayList(32, displays, &displayCount);
    int numDisplays = displayCount;
    int whichDisplay = numDisplays-1;
    int displayHeight = CGDisplayPixelsHigh (displays[whichDisplay]);
    int displayWidth = CGDisplayPixelsWide (displays[whichDisplay]);
    CGRect displayBounds = CGDisplayBounds (displays[whichDisplay]);
    
    eqkoscope* app = new eqkoscope();
    app->loadConfig();
    
    if(displayCount==1)
        app->dualDisplay = false;
    
    if(app->MULTIPROJECTOR){
        ofAppGLFWWindow window;
        window.setMultiDisplayFullscreen(true);
        ofSetupOpenGL(&window,app->FINALWIDTH,app->FINALHEIGHT,OF_WINDOW); //Multi-projectors
    }else{
        ofSetupOpenGL(app->FINALWIDTH, app->FINALHEIGHT, OF_WINDOW);
    }
    glfwWindowHint(GLFW_DECORATED, false);

    
    if(app->liveMode){
        glfwWindowHint(GLFW_DECORATED, false);
        ofSetWindowPosition(0, 0);
    }
    if(app->dualDisplay)
        ofSetWindowPosition(-1280, 0);

    ofRunApp(app);
    
}
