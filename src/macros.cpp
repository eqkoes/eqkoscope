#include "eqkoscope.h"
#include "ofFileUtils.h"

int analysis[128*MAX_MACRO_PAGES];
string macroComments;

vector<string> videosToPreload;

void eqkoscope::loadMacroMap(){
    stringstream path;
    path << macroPath << "macroMap.csv";
    string t = ofBufferFromFile(path.str()).getText();
    if(t.compare("")){
        vector<string> lines = ofSplitString(t, "\n");
        for(int x=0;x<lines.size();x++){
            if(!lines[x].compare(0, 2, "//"))
                continue;
            vector<string > fields = ofSplitString(lines[x], ",");
            if(fields.size()<2)
                continue;
            string codeStr = fields[0];
            int p;
            if(codeStr.find('x')==string::npos)
                p = ofToInt(codeStr);
            else{
                vector<string > cc = ofSplitString(codeStr, "x");
                p = ofToInt(cc[0])*16 + ofToInt(cc[1]);
            }
            string value = fields[1];
            macroMap[p] = value;
            macroNames[p] = value;
        }
    }
}

void eqkoscope::setMacroPage(int page){
    if(page>(MAX_MACRO_PAGES-1) || page<0)
        return;
    macroPage = page;
    paintMacros();
}

void eqkoscope::setMacroCode(int c){
    if(currentMacroCode>=0 && currentMacroCode<=128*MAX_MACRO_PAGES)
        launchpadOut.sendNoteOn(1, currentMacroCode, analysis[currentMacroCode]);
    currentMacroCode = c;
}

void eqkoscope::updateMacros(){
    int f = ofGetFrameNum();
    int r1 = 20;
    if(f%r1==0) //next
        launchpadOut.sendControlChange(1, 107, 15);
    if(f%r1==r1/2)
        launchpadOut.sendControlChange(1, 107, 0);
    
    int r2 = 10;
    if(f%r2==0) //current
        launchpadOut.sendNoteOn(1, currentMacroCode, 0);
    if(f%r2==r2/2)
        launchpadOut.sendNoteOn(1, currentMacroCode, analysis[currentMacroCode + macroPage*128]);
}

void eqkoscope::analyzeMacros(){
    
    videosToPreload.clear();
    
    if(!is_launchpad)
        return;
    
    int feedbackCode = 15;
    int cinemaCode = 60;
    int uziCode = 62;
    int linesCode = 62;
    int drawCode = 29;
    int printCode = 62;
    int triCode = 63;
    int fractalCode = 30;
    int ledCode = 30;
    
    int sceneCode;
    for(int i=0;i<128*MAX_MACRO_PAGES;i++){
        string p = getMacroFromMIDI(i);
        if(p.empty())
            continue;
        ofFile file(p, ofFile::ReadOnly);
        
        if(file.exists()){
            sceneCode = 0;
            string cinemaStr = "";
            
            ofBuffer buffer = file.readToBuffer();
            ofXml macro;
            macro.loadFromBuffer(buffer.getText());
            
            if(!macro.getAttribute("version").compare("v2")){
                
                macro.setTo("FX");
                macro.setTo("triumMode");
                float v = macro.getFloatValue();
                if(v!=0)
                    sceneCode = triCode;
                macro.setToParent();
                macro.setToParent();
            }
            
            if(!macro.exists("scenes")){
                sceneCode = ledCode;
            }else{
                if(sceneCode != triCode){
                    if(!macro.exists("scenes")){ //old solo scene loading
                        if(macro.exists("feedback"))
                            sceneCode = feedbackCode;
                        if(macro.exists("uzi"))
                            sceneCode = uziCode;
                        if(macro.exists("cinema")){
                            sceneCode = cinemaCode;
                            macro.setTo("cinema");
                            cinemaStr = macro.getValue("vidPath0");
                            videosToPreload.push_back(cinemaStr);
                            macro.setToParent();
                        }
                        if(macro.exists("print"))
                            sceneCode = printCode;
                        if(macro.exists("lines")){
                            sceneCode = linesCode;
                        }
                        
                    }else{
                        macro.setTo("scenes");
                        for(int i=0;i<3;i++){
                            bool ret = macro.setToChild(i); //<scene>
                            if(!ret){
                                continue;
                            }
                            
                            string layerStr = macro.getAttribute("layer");
                            int layer = 0;
                            if(!layerStr.compare("1"))
                                layer = 1;
                            else
                                if(!layerStr.compare("2"))
                                    layer = 2;
                            
                            macro.setToChild(0); //the scene
                            
                            string sceneStr = macro.getName();
                            
                            if(!sceneStr.compare("feedback"))
                                sceneCode = feedbackCode;
                            else
                                if(!sceneStr.compare("fractal"))
                                    sceneCode = fractalCode;
                                else
                                    if(!sceneStr.compare("uzi")){
                                        sceneCode = uziCode;
                                    }else
                                        if(!sceneStr.compare("cinema")){
                                            sceneCode = cinemaCode;
                                            macro.setTo("cinema");
                                            cinemaStr = macro.getValue("vidPath0");
                                            videosToPreload.push_back(cinemaStr);
                                            macro.setToParent();
                                        }else
                                            if(!sceneStr.compare("print"))
                                                sceneCode = printCode;
                                            else
                                                if(!sceneStr.compare("lines"))
                                                    sceneCode = linesCode;
                                                else
                                                    if(!sceneStr.compare("draw"))
                                                        sceneCode = drawCode;
                        }
                    }
                }
            }
            analysis[i] = sceneCode;
            //            launchpadOut.sendNoteOn(1, i, analysis[i]);
        }else{
            //            launchpadOut.sendNoteOn(1, i, 0);
        }
    }
    
    paintMacros();
    
    cinema->preloadVideos(videosToPreload);
}

void eqkoscope::paintMacros(){
    for(int i=0;i<128;i++){
        if(analysis[i]){
            launchpadOut.sendNoteOn(1, i, analysis[i+128*macroPage]);
        }else{
            launchpadOut.sendNoteOn(1, i, 0);
        }
    }
    launchpadOut.sendControlChange(1, 104, macroPage>0 ? 60 : 0);
    launchpadOut.sendControlChange(1, 105, macroPage<MAX_MACRO_PAGES-1 ? 60 : 0);
    
}

void eqkoscope::saveCurrentMacro(){
    saveMacro(currentMacroStr);
}

void eqkoscope::saveMacro(string path){
    currentMacroStr = path;
    
    if(parameterMap[stressTest])
        return;
    
    ofFile file(path, ofFile::WriteOnly);
    
    if(!file.exists())
        file.create();
    ofBuffer b;
    stringstream str;
    
    str << "<xml version=\"v3\">" << endl;
    str << "<FX>" << endl;
    
    typedef std::map<std::string, float>::iterator it_type;
    for(int i=0;i<N_PARAM;i++){
        if(parameterIDMap[i].compare(""))
            saveParameter(&str, parameterIDMap[i], parameterMap[i]);
    }
    
    str << "</FX>" << endl;
    
    ofStringReplace(macroComments, "\n\n\n", "");
    str << "<comments>" << endl << macroComments << endl << "</comments>" << endl;
    
    //    str << "<parameterMapping path=\"" << controlFile << "\"/>" << endl;
    str << "<parameterMapping>" << endl;
    for(Auto* a : audioAutos)
        str << a->toString() << endl;
    for(Auto* a : midiCCAutos)
        str << a->toString() << endl;
    for(Auto* a : midiNoteonAutos)
        str << a->toString() << endl;
    for(Auto* a : leapAutos)
        str << a->toString() << endl;
    for(Auto* a : oscAutos)
        str << a->toString() << endl;
    for(Auto* a : timedAutos)
        str << a->toString() << endl;
    str << "</parameterMapping>" << endl;
    
    str << "<featuredParameter id=\"" << featuredParameter << "\"/>" << endl;
    str << "<leapXParameter id=\"" << leapXParameter << "\"/>" << endl;
    str << "<leapYParameter id=\"" << leapYParameter << "\"/>" << endl;
    
    
    str << "<scenes>" << endl;
    
    for(int i=0;i<scenes.size();i++){
        if(scenes[i]!=0){
            str << "<scene layer=\"" << i << "\">" << endl;
            scenes[i]->saveMacro(&str);
            str << "</scene>" << endl;
        }
    }
    str << "</scenes>" << endl;
    
    str << "</xml>" << endl;
    
    b.set(str.str());
    file.writeFromBuffer(b); //read only ??
    file.close();
    
    analyzeMacros();
}

void eqkoscope::parseMacro(string path){
    cout << "PARSING MACRO " << path << endl;
    
    ofFile file(path);
    if(file.exists()){
        fbo.begin(); //reset fbos
        ofSetColor(0, 0, 0);
        ofRect(0, 0, WIDTH, HEIGHT);
        fbo.end();
        fbo2.begin();
        ofSetColor(0, 0, 0);
        ofRect(0, 0, WIDTH, HEIGHT);
        fbo2.end();
        
        macroMorphing = 0;
        if(strEndsWith(path, "linesOmg.xml")){
            macroMorphing = 10;
        }
        
//        if(strEndsWith(path, "volcaBase.xml")  ||
//           strEndsWith(path, "RED.xml") || strEndsWith(path, "v3.xml")
//           || strEndsWith(path, "yes.xml")
//           || strEndsWith(path, "ffff.xml")
//           || strEndsWith(path, "ciiircle.xml")
//           || strEndsWith(path, "trip.xml")|| strEndsWith(path, "waaaa.xml")){
//            macroMorphing = 10;
//        }
        
        if(macroMorphing>0){
            for(int i=0;i<N_PARAM;i++)
                macroMorphParameters[i] = deltaMap[i];
        }
        
        ofBuffer buffer = file.readToBuffer();
        ofXml macro;
        macro.loadFromBuffer(buffer.getText());
        
        bool v2 = ofStringTimesInString(buffer.getText(), "<scenes>");
        bool v3 = !macro.getAttribute("version").compare("v3");
        bool doNotLoadScenes = false;
        // check if the macro is a complete macro
        float oldBright = parameterMap[tintBrightness];
        
        if(macro.exists("scenes")>0 || macro.exists("scene")>0){//full macro load
            initParameters();
            currentMacroStr = path;
            macroComments = "";
            if(macro.setTo("comments")){
                macroComments = macro.getValue();
                macro.setToParent();
            }
        }else{//partial macro load (LED, etc)
            doNotLoadScenes = true;
        }
        
        string mappingStr;
        if(macro.exists("parameterMapping") && macro.setTo("parameterMapping")){
            controlFile = macro.getAttribute("path");
            if(!controlFile.compare("")){
                mappingStr = macro.getValue();
            }
            macro.setToParent();
        }else
            controlFile = "";
        
        if(controlFile.compare("")){
            loadMapping(ofBufferFromFile("parameterMappings/"+controlFile).getText(), false, !doNotLoadScenes);
        }else{
            loadMapping(mappingStr, false, !doNotLoadScenes);
        }
        
        if(v2 || v3){
            macro.setTo("FX");
            
            /** COMPATIBILITY **/
            if(macro.getValue("kenBurnsZ").empty())
                parameterMap[kenBurnsZ] = 0;
            
            if(v3){
                string csvFXParams = macro.getValue();
                vector<string> splitFX = ofSplitString(csvFXParams, "\n");
                for(string line: splitFX){
                    vector<string> ss = ofSplitString(line, ",");
                    if(ss.size()==2){
                        string name = ss[0];
                        if(name.compare("tintBrightness") &&
                           name.compare("draw_recording") && name.compare("draw_destroy")
                           && name.compare("draw_destroyMode") && name.compare("tintBrightness") && name.compare("draw_currentDrawing")
                           && name.compare("#comment")) //todo opt and put away from map
                            deltaMap[parameterNameMap[name]] = ofToFloat(ss[1]);
                    }
                }
                macro.setToParent();
                parameterMap[tintBrightness] = deltaMap[tintBrightness] = oldBright;
            }else{
                if(macro.getNumChildren()>0){
                    macro.setToChild(0);
                    do{
                        string name = macro.getName();
                        if(name.compare("draw_recording") && name.compare("draw_destroy")
                           && name.compare("draw_destroyMode") && name.compare("tintBrightness") && name.compare("draw_currentDrawing") && name.compare("#comment")) //todo opt and put away from map
                            
                             deltaMap[parameterNameMap[macro.getName()]] = macro.getFloatValue();;
                    }while(macro.setToSibling());
                }
                macro.loadFromBuffer(buffer.getText());
            }
        }else{
            macro.setTo("FX");
            
            parameterMap[bpm] = macro.getFloatValue("bpm");
            parameterMap[toLine] = macro.getFloatValue("toLine");
            parameterMap[stripesAmp] = macro.getFloatValue("stripesAmp");
            parameterMap[stripesSize] = macro.getFloatValue("stripesSize");
            parameterMap[skewAmp] = macro.getFloatValue("skewAmp");
            parameterMap[skewVAmp] = macro.getFloatValue("skewVAmp");
            parameterMap[skewSpeed] = macro.getFloatValue("skewSpeed");
            parameterMap[skewBorderCopy] = macro.getFloatValue("skewBorderCopy")==1;
            parameterMap[displaceAmp] = macro.getFloatValue("displaceAmp");
            parameterMap[displaceVAmp] = macro.getFloatValue("displaceVAmp");
            parameterMap[displaceProba] = macro.getFloatValue("displaceProba");
            parameterMap[chromaOffset] = macro.getFloatValue("chromaOffset");
            parameterMap[chromasepAngle] = macro.getFloatValue("chromasepAngle");
            parameterMap[chromaSep] = macro.getFloatValue("chromaSep");
            parameterMap[doubleChromaSep] =macro.getFloatValue("doubleChromaSep")==1;
            deltaMap[chromaSep] = parameterMap[chromaSep];
            parameterMap[chromaSens] = macro.getFloatValue("chromaSens")==1;
            parameterMap[sortXThresh] = macro.getFloatValue("sortXThresh");
            parameterMap[sortYThresh] = macro.getFloatValue("sortYThresh");
            parameterMap[post_traitement] = macro.getFloatValue("post_traitement")==1;
            parameterMap[nBlocks] = macro.getFloatValue("nBlocks");
            parameterMap[nFreeze] = macro.getFloatValue("nFreeze");
            parameterMap[kalei] = macro.getFloatValue("kalei");
            parameterMap[kaleiNb] = macro.getFloatValue("kaleiNb");
            parameterMap[kaleiOffX] = macro.getFloatValue("kaleiOffX");
            parameterMap[kaleiOffY] = macro.getFloatValue("kaleiOffY");
            parameterMap[randomTint] = macro.getFloatValue("randomTint")==1;
            parameterMap[_gamma] = macro.getFloatValue("gamma");
            parameterMap[_invert] = macro.getFloatValue("invert")==1;
            parameterMap[vblur] = macro.getFloatValue("vblur");
            parameterMap[hblur] = macro.getFloatValue("hblur");
            deltaMap[vblur]  = parameterMap[vblur];
            deltaMap[hblur]  = parameterMap[hblur];
            parameterMap[sharpenMode] =(int)  macro.getFloatValue("sharpenMode");
            parameterMap[sharpen] = macro.getFloatValue("sharpen");
            parameterMap[blurOriginalMix] = macro.getFloatValue("blurOriginalMix");
            texturing = macro.getFloatValue("texturing")==1;
            cinemaBackground = macro.getFloatValue("cinemaBackground")==1;
            cinemaTexture = macro.getFloatValue("cinemaTexture")==1;
            parameterMap[randHHide] = macro.getFloatValue("randHHide");
            parameterMap[randVHide] = macro.getFloatValue("randVHide");
            parameterMap[glow] = macro.getFloatValue("glow")==1;
            parameterMap[sobel] = macro.getFloatValue("sobel")==1;
            parameterMap[omg3D] = macro.getFloatValue("omg3D");
            parameterMap[omg3D2] = macro.getFloatValue("omg3D2");
            parameterMap[omg3D2Rotation] = macro.getFloatValue("omg3D2Rotation");
            deltaMap[omg3D2Rotation] = parameterMap[omg3D2Rotation];
            parameterMap[omg3D2Dist] = macro.getFloatValue("omg3D2Dist");
            deltaMap[omg3D2Dist] = parameterMap[omg3D2Dist];
            parameterMap[omg3D2Divergence] = macro.getFloatValue("omg3D2Divergence");
            deltaMap[omg3D2Divergence] = parameterMap[omg3D2Divergence];
            parameterMap[omg3D2Symetry] =macro.getFloatValue("omg3D2Symetry")==1;
            parameterMap[omg3D2FreeRotation] =macro.getFloatValue("omg3D2FreeRotation")==1;
            parameterMap[omg3D2Speed] = macro.getFloatValue("omg3D2Speed");
            parameterMap[xpixellate] = macro.getFloatValue("xpixellate");
            parameterMap[ypixellate] = macro.getFloatValue("ypixellate");
            deltaMap[xpixellate] = macro.getFloatValue("xpixellateDelta");
            deltaMap[ypixellate] = macro.getFloatValue("ypixellateDelta");
            parameterMap[paint] = macro.getFloatValue("paint");
            deltaMap[contrast] = parameterMap[contrast] = 0;
            
            if(macro.getNumChildren("blendType")>0)
                parameterMap[blendType] = macro.getFloatValue("blendType");
            else
                parameterMap[blendType] = MULTIPLY;
            
            parameterMap[triumMode] = macro.getFloatValue("triumMode");
            parameterMap[centerTrium] = macro.getFloatValue("centerTrium");
            parameterMap[currentScene] = macro.getFloatValue("currentScene");
            
            if(loadColorWithMacro){
                parameterMap[chromasepHue] = macro.getFloatValue("chromasepHue");
                deltaMap[chromaSep] = macro.getFloatValue("chromaSepDelta");
                if(macro.getNumChildren("tintSaturation")>0)
                    parameterMap[tintSaturation] = macro.getFloatValue("tintSaturation");
                else
                    parameterMap[tintSaturation] = 1;
                if(macro.getNumChildren("reTint")>0)
                    parameterMap[_reTint] = macro.getFloatValue("reTint");
                else
                    parameterMap[_reTint] = false;
                parameterMap[tintHue] = macro.getFloatValue("tintHue");
                //            tintBrightness = macro.getFloatValue("tintBrightness");
                //            dTintBrightness = macro.getFloatValue("dTintBrightness");
                parameterMap[tintMode] = macro.getFloatValue("tintMode");
                parameterMap[tintCenter] = macro.getFloatValue("tintCenter");
                parameterMap[tintAmp] = macro.getFloatValue("tintAmp");
                if(macro.getNumChildren("sidesSaturation")>0)
                    parameterMap[sidesSaturation] = macro.getFloatValue("sidesSaturation");
                else
                    parameterMap[sidesSaturation] = 1;
                
                parameterMap[gradient] = macro.getFloatValue("gradient")==1;
            }
        }
        
        if(macro.exists("featuredParameter") && macro.setTo("featuredParameter")){
            featuredParameter = parameterNameMap[macro.getAttribute("id")];
            macro.setToParent();
        }else
            featuredParameter = -1;
        
        if(macro.exists("leapXParameter") && macro.setTo("leapXParameter")){
            leapXParameter = parameterNameMap[macro.getAttribute("id")];
            macro.setToParent();
        }else
            leapXParameter = -1;
        
        if(macro.exists("leapYParameter") && macro.setTo("leapYParameter")){
            leapYParameter = parameterNameMap[macro.getAttribute("id")];
            macro.setToParent();
        }else
            leapYParameter = -1;
        if(macro.exists("leapZParameter") && macro.setTo("leapZParameter")){
            leapZParameter = parameterNameMap[macro.getAttribute("id")];
            macro.setToParent();
        }else
            leapZParameter = -1;
        if(macro.exists("leapDXParameter") && macro.setTo("leapDXParameter")){
            leapDXParameter = parameterNameMap[macro.getAttribute("id")];
            macro.setToParent();
        }else
            leapDXParameter = -1;
        if(macro.exists("leapDYParameter") && macro.setTo("leapDYParameter")){
            leapDYParameter = parameterNameMap[macro.getAttribute("id")];
            macro.setToParent();
        }else
            leapDYParameter = -1;
        if(macro.exists("leapDZParameter") && macro.setTo("leapDZParameter")){
            leapDZParameter = parameterNameMap[macro.getAttribute("id")];
            macro.setToParent();
        }else
            leapDZParameter = -1;
        if(macro.exists("leapRollParameter") && macro.setTo("leapRollParameter")){
            leapRollParameter = parameterNameMap[macro.getAttribute("id")];
            macro.setToParent();
        }else
            leapRollParameter = -1;
        
        if(!doNotLoadScenes){
            if(!v2){ //old solo scene loading
                macro.setToParent();
                
                parameterMap[triumMode] = 0;
                if(macro.exists("feedback"))
                    loadSoloScene(feedbackScene);
                if(macro.exists("fractal"))
                    loadSoloScene(fractal);
                if(macro.exists("agents"))
                    loadSoloScene(agents);
                if(macro.exists("pointback"))
                    loadSoloScene(pointback);
                if(macro.exists("shape"))
                    loadSoloScene(shape);
                if(macro.exists("uzi"))
                    loadSoloScene(uzi);
                if(macro.exists("cinema"))
                    loadSoloScene(cinema);
                if(macro.exists("print"))
                    loadSoloScene(print);
                if(macro.exists("lines"))
                    loadSoloScene(lines);
                
                macro.setToChild(1);
                scenes[0]->loadMacro(&macro);
                
                parameterMap[kenBurnsZ] = 0;
            }
            else{
                if(macro.exists("scenes")){
                    macro.setTo("scenes");
                    for(int i=0;i<3;i++){
                        if(!macro.setToChild(i)){
                            loadScene(0, i);
                            continue;
                        }
                        
                        string layerStr = macro.getAttribute("layer");
                        int layer = 0;
                        if(!layerStr.compare("1"))
                            layer = 1;
                        else
                            if(!layerStr.compare("2"))
                                layer = 2;
                        
                        macro.setToChild(0); //the scene
                        
                        string sceneStr = macro.getName();
                        
                        if(layer>0 && !sceneStr.compare("cinema")){
                            loadScene(cinemas[0], layer);
                        }else{
                            if(layer>0 && !sceneStr.compare("uzi")){
                                loadScene(uzis[0], layer);
                            }else{
                                Scene* theScene;
                                for(int si=0;si<allScenes.size();si++)
                                    if(!allScenes[si]->sceneID.compare(sceneStr)){
                                        theScene = allScenes[si];
                                        break;
                                    }
                                loadScene(theScene, layer);
                            }
                        }
                        
                        if(scenes[i]!=0)
                            scenes[i]->loadMacro(&macro);
                        macro.setToParent(); //<scene>
                        macro.setToParent(); //<scenes>
                    }
                }
            }
        }
    }
    
    if(macroMorphing>0){
        macroMorphEvo = 0;
        // The following parameters are not morphed
        macroMorphParameters[blendType] = deltaMap[blendType];
        macroMorphParameters[strobe] = deltaMap[strobe];
        macroMorphParameters[_invert] = deltaMap[_invert];
        macroMorphParameters[vMirror] = deltaMap[vMirror];
        macroMorphParameters[hMirror] = deltaMap[hMirror];
        macroMorphParameters[kalei] = deltaMap[kalei];
        macroMorphParameters[kalei_2] = deltaMap[kalei_2];
        
        
        
        if(macroMorphParameters[kaleiNb]<=1 || deltaMap[kaleiNb]<=1)
            macroMorphParameters[kaleiNb] = deltaMap[kaleiNb];
        
        
        //todo data structure with high level parameter and their low level attributes
        if(macroMorphParameters[omg3D2]>0 && deltaMap[omg3D2]==0){
            deltaMap[omg3D2Rotation] = macroMorphParameters[omg3D2Rotation];
            deltaMap[omg3D2Dist] = macroMorphParameters[omg3D2Dist];
            deltaMap[omg3D2YDivergence] = macroMorphParameters[omg3D2YDivergence];
            deltaMap[omg3D2Divergence] = macroMorphParameters[omg3D2Divergence];
            deltaMap[omg3D2Alpha0] = macroMorphParameters[omg3D2Alpha0];
            deltaMap[omg3D2Nb] = macroMorphParameters[omg3D2Nb];
        }
        
        if(macroMorphParameters[omg3D2]==0 && deltaMap[omg3D2]>0){
            macroMorphParameters[omg3D2Rotation] = deltaMap[omg3D2Rotation];
            macroMorphParameters[omg3D2Dist] = deltaMap[omg3D2Dist];
            macroMorphParameters[omg3D2YDivergence] = deltaMap[omg3D2YDivergence];
            macroMorphParameters[omg3D2Divergence] = deltaMap[omg3D2Divergence];
            macroMorphParameters[omg3D2Alpha0] = deltaMap[omg3D2Alpha0];
            macroMorphParameters[omg3D2Nb] = deltaMap[omg3D2Nb];
        }
        
        
        if(macroMorphParameters[tintSaturation]>0 && deltaMap[tintSaturation]==0){
            deltaMap[tintHue] = macroMorphParameters[tintHue];
        }
        
        if(macroMorphParameters[tintSaturation]==0 && deltaMap[tintSaturation]>0){
            macroMorphParameters[tintHue] = deltaMap[tintHue];
        }
        
        if((macroMorphParameters[_invert]>0 && deltaMap[_invert]==0) || (macroMorphParameters[_invert]==0 && deltaMap[_invert]>0)){
            macroMorphParameters[_invert] = deltaMap[_invert];
            macroMorphParameters[_gamma] = 1/macroMorphParameters[_gamma];
        }

    }else{
    for(int i=0;i<N_PARAM;i++)
        parameterMap[i] = deltaMap[i];
    }
    
}

void eqkoscope::loadMacro(string path){
    pendingMacros.push_back(path);
}

void eqkoscope::loadFirstMacro(){
    setMacroCode(0);
    string p = getMacroFromMIDI(currentMacroCode);
    if(!p.empty())
        loadMacro(p);
}

void eqkoscope::reloadMacro(){
    loadMacro(currentMacroStr);
}

void eqkoscope::loadNextMacro(){
    int c = currentMacroCode+1;
    if(c>128*MAX_MACRO_PAGES)
        c = 0;
    setMacroCode(c);
    string p = getMacroFromMIDI(currentMacroCode);
    if(!p.empty())
        loadMacro(p);
}

void eqkoscope::loadPrevMacro(){
    int c = currentMacroCode-1;
    if(c<0)
        c = 128*MAX_MACRO_PAGES;
    setMacroCode(c);
    string p = getMacroFromMIDI(currentMacroCode);
    if(!p.empty())
        loadMacro(p);
}

string eqkoscope::getMacroFromMIDI(int pitch){
    stringstream path;
    if(macroMap.count(pitch)){
        if(macroMap[pitch].empty())
            return "";
        path << macroPath << macroMap[pitch] << ".xml";
    }else
        path << macroPath << "m" << pitch << ".xml";
    return path.str();
}