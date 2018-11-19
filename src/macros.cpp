#include "eqkoscope.h"
#include "ofFileUtils.h"
#include "../libs/tinyxml.h"



static int analysis[NB_MACRO_PER_PAGE*MAX_MACRO_PAGES];
string macroComments;

void eqkoscope::loadMacroMap(){
    stringstream path;
    if(!strEndsWith(macroPath, "/")){
        macroPath += "/";
    }
    path << macroPath << "macroMap.csv";
    string t = ofBufferFromFile(path.str()).getText();
    ofStringReplace(t, "\r", "\n");
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
#ifdef MACRO_APC
                p = ofToInt(cc[0])*8 + ofToInt(cc[1]);
#else
                p = ofToInt(cc[0])*16 + ofToInt(cc[1]);
#endif
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
    if(currentMacroCode>=0 && currentMacroCode<=NB_MACRO_PER_PAGE*MAX_MACRO_PAGES)
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
    int macroPitch = currentMacroCode - macroPage*NB_MACRO_PER_PAGE;
    if(f%r2==0) //current
        launchpadOut.sendNoteOn(1, macroPitch, 0);
    if(f%r2==r2/2)
        launchpadOut.sendNoteOn(1, macroPitch, analysis[currentMacroCode + macroPage*NB_MACRO_PER_PAGE]);
    
    if(deltaMap[_loadMacro] == 1){
        loadNextMacro();
        deltaMap[_loadMacro] = 0;
    }
    
    if(deltaMap[_loadMacro] == -1){
        loadPrevMacro();
        deltaMap[_loadMacro] = 0;
    }
}

void eqkoscope::analyzeMacros(){
    
    if( ! (MIDI_device_connected.count(MACRO_INTERFACE) &&  MIDI_device_connected[MACRO_INTERFACE])){
        return;
    }
    
//#ifdef OF_10
#ifdef MACRO_APC
    int feedbackCode = 3;
    int cinemaCode = 1;
    int uziCode = 5;
    int linesCode = 3;
    int drawCode = 3;
#else
    
    int feedbackCode = 15;
    int cinemaCode = 60;
    int uziCode = 62;
    int linesCode = 62;
    int drawCode = 29;
#endif
    int printCode = 62;
    int triCode = 63;
    int fractalCode = 30;
    int ledCode = 30;
    
    int sceneCode;
    for(int i=0;i<NB_MACRO_PER_PAGE*MAX_MACRO_PAGES;i++){
        analysis[i] = 0;
        string p = getMacroFromMIDI(i);
        if(p.empty())
            continue;
        ofFile file(p, ofFile::ReadOnly);
        
        if(file.exists()){
            sceneCode = 0;
            string cinemaStr = "";
            
//            ofBuffer buffer = file.readToBuffer();
//            ofXml macro;
//            macro.load(buffer.getText());
            
            TiXmlDocument 	doc;
            bool loadOkay = doc.LoadFile(file.getAbsolutePath());
            TiXmlHandle macro = TiXmlHandle(&doc).ToNode()->FirstChild();
            
            TiXmlHandle scenes_xml = macro.FirstChild("scenes");
            if(! scenes_xml.ToNode()){
                sceneCode = ledCode;
            }else{
                if(scenes_xml.ToNode()){ //old solo scene loading
                    if(scenes_xml.FirstChild().ToNode()->FirstChild("feedback"))
                        sceneCode = feedbackCode;
                    if(scenes_xml.FirstChild().ToNode()->FirstChild("uzi"))
                        sceneCode = uziCode;
                    if(scenes_xml.FirstChild().ToNode()->FirstChild("cinema")){
                        sceneCode = cinemaCode;
                    }
                    if(scenes_xml.FirstChild().ToNode()->FirstChild("print"))
                        sceneCode = printCode;
                    if(scenes_xml.FirstChild().ToNode()->FirstChild("lines")){
                        sceneCode = linesCode;
                    }
                    
                }else{
                    TiXmlHandle scenes_child_xml = scenes_xml.FirstChild();
                    for(int i=0;i<3;i++){
                        
                        if(!scenes_child_xml.ToNode()){
                            continue;
                        }
                        
                        string layerStr = scenes_child_xml.ToElement()->Attribute("layer");
                        int layer = 0;
                        if(!layerStr.compare("1"))
                            layer = 1;
                        else
                            if(!layerStr.compare("2"))
                                layer = 2;
                        
                        TiXmlHandle scene_core_xml = scenes_child_xml.FirstChild();
                        
                        string sceneStr = scenes_child_xml.Element()->ValueStr();
                        {
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
                        
                        scenes_child_xml = scenes_child_xml.Node()->NextSibling();
                    }
                }
            }
            analysis[i] = sceneCode;
        }
    }
    
    paintMacros();
//#endif
}

void eqkoscope::paintMacros(){
    for(int i=0;i<NB_MACRO_PER_PAGE;i++){
        int pitch = i + NB_MACRO_PER_PAGE*macroPage;
        if(analysis[pitch]){
            launchpadOut.sendNoteOn(1, i, analysis[pitch]);
        }else{
            launchpadOut.sendNoteOn(1, i, 0);
        }
    }
    launchpadOut.sendControlChange(1, 104, macroPage>0 ? 60 : 0);
    launchpadOut.sendControlChange(1, 105, macroPage<MAX_MACRO_PAGES-1 ? 60 : 0);
}

int eqkoscope::getMacroPitch(int macroNb){
#ifdef MACRO_APC
    return macroNb + NB_MACRO_PER_PAGE*macroPage;
#else
    return macroNb + NB_MACRO_PER_PAGE*macroPage;
#endif
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
        str << a->toString() << ";" << endl;
    for(Auto* a : midiCCAutos)
        str << a->toString() << endl;
    for(Auto* a : midiNoteonAutos)
        str << a->toString() << ";" << endl;
    for(Auto* a : leapAutos)
        str << a->toString() << ";" << endl;
    for(Auto* a : oscAutos)
        str << a->toString() << ";" << endl;
    for(Auto* a : timedAutos)
        str << a->toString() << ";" << endl;
    ofStringReplace(autoComments, "\n\n", "\n");
    str << autoComments << endl;
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
    
    if(recordAndRenderFlag==0){
        processingStartDate = ofGetElapsedTimeMillis();
    }
}

void eqkoscope::parseMacro(string path){
    cout << "PARSING MACRO " << path << endl;
    
    try{
    
    ofFile file(path);
    if(file.exists()){
        fbo.begin(); //reset fbos
        ofSetColor(0, 0, 0);
        ofDrawRectangle(0, 0, crt_WIDTH, crt_HEIGHT);
        fbo.end();
        fbo2.begin();
        ofSetColor(0, 0, 0);
        ofDrawRectangle(0, 0, crt_WIDTH,crt_HEIGHT);
        fbo2.end();
        
        macroMorphing = parameterMap[macroFade];
        
        if(macroMorphing>0){
            for(int i=0;i<N_PARAM;i++)
                macroMorphParameters[i] = deltaMap[i];
        }

        TiXmlDocument 	doc;
        bool loadOkay = doc.LoadFile(file.getAbsolutePath());
        TiXmlHandle macro = TiXmlHandle(&doc).ToNode()->FirstChild();

        //        bool v3 = !macro.getAttribute("version").compare("v3");
        bool doNotLoadScenes = false;
        // check if the macro is a complete macro
        float oldBright = parameterMap[tintBrightness];
        
        TiXmlHandle scenes_xml = macro.FirstChild("scenes");
        TiXmlHandle scene_xml = macro.FirstChild("scene");
        if(scenes_xml.ToNode() || scene_xml.ToNode()){//full macro load
            initParameters();
            currentMacroStr = path;
            macroComments = "";
            TiXmlHandle comments_xml = macro.FirstChild("comments");
            if(comments_xml.ToNode()){
                auto c = comments_xml.ToElement()->GetText();
                if(c != 0)
                macroComments = c;
            }
        }else{//partial macro load (LED, etc)
            doNotLoadScenes = true;
        }
        
        string mappingStr;
        TiXmlHandle parameterMapping_xml = macro.FirstChild("parameterMapping");
        if(parameterMapping_xml.ToNode()){
            auto controlFileChar = parameterMapping_xml.ToElement()->Attribute("path");
            if(controlFileChar != NULL){
                controlFile = string(controlFileChar);
            }else{
                controlFile = "";
            }
            if(parameterMapping_xml.ToElement()->GetText()) //debg assertion
            mappingStr = parameterMapping_xml.ToElement()->GetText();
        }
        
        if(controlFile.compare("")){
            loadMappingFiles("parameterMappings/"+controlFile, false, !doNotLoadScenes);
        }else{
            loadMapping(mappingStr, false, !doNotLoadScenes);
        }
        
        TiXmlHandle fx_xml = macro.FirstChild("FX");
        if(!fx_xml.ToNode())
           return;
        
        string csvFXParams = fx_xml.ToElement()->GetText();
        vector<string> splitFX = ofSplitString(csvFXParams, "\n");
        if(splitFX.size()==1)
            splitFX = ofSplitString(csvFXParams, "\r");
        if(splitFX.size()==1)
            splitFX = ofSplitString(csvFXParams, " ");
        for(string line: splitFX){
            vector<string> ss = ofSplitString(line, ",");
            if(ss.size()==2){
                string name = ss[0];
                if(name.compare("tintBrightness") &&
                   name.compare("draw_recording") && name.compare("draw_destroy")
                   && name.compare("draw_destroyMode") && name.compare("tintBrightness") && name.compare("draw_currentDrawing")
                   && name.compare("oscIn")
                   //                           && name.compare("ledBrightness")
                   && name.compare("#comment")
                   && name.compare("")
                   && getParameterFromName(name)>=0){ //todo opt and put away from map
                    int pid = getParameterFromName(name);
                    float value =  ofToFloat(ss[1]);
                    if(pid == ledTint && value > 1) //todo retirer en 2k19
                        value /= 255.0;
                       deltaMap[pid] = value;
                }
            }
        }

        parameterMap[tintBrightness] = deltaMap[tintBrightness] = oldBright;
        
        TiXmlHandle tmp_xml = macro.FirstChild("featuredParameter");
        if(tmp_xml.ToNode()){
            string id = tmp_xml.ToElement()->Attribute("id");
            if(id.compare("-1") && getParameterFromName(id)>=0)
            featuredParameter = getParameterFromName(id);
        }else
            featuredParameter = -1;
        
         tmp_xml = macro.FirstChild("leapXParameter");
        if(tmp_xml.ToNode()){
            string id = tmp_xml.ToElement()->Attribute("id");
            if(id.compare("-1") && getParameterFromName(id)>=0)
            leapXParameter = getParameterFromName(id);
        }else
            leapXParameter = -1;

        tmp_xml = macro.FirstChild("leapYParameter");
        if(tmp_xml.ToNode()){
            string id = tmp_xml.ToElement()->Attribute("id");
            if(id.compare("-1") && getParameterFromName(id)>=0)
            leapYParameter = getParameterFromName(id);
        }else
            leapYParameter = -1;
         tmp_xml = macro.FirstChild("leapZParameter");
        if(tmp_xml.ToNode()){
            string id = tmp_xml.ToElement()->Attribute("id");
            if(id.compare("-1") &&getParameterFromName(id)>=0)
            leapZParameter = getParameterFromName(id);
        }else
            leapZParameter = -1;
         tmp_xml = macro.FirstChild("leapDXParameter");
        if(tmp_xml.ToNode()){
            string id = tmp_xml.ToElement()->Attribute("id");
            if(id.compare("-1") && getParameterFromName(id)>=0)
            leapDXParameter = getParameterFromName(id);
        }else
            leapDXParameter = -1;
         tmp_xml = macro.FirstChild("leapDYParameter");
        if(tmp_xml.ToNode()){
            string id = tmp_xml.ToElement()->Attribute("id");
            if(id.compare("-1") && getParameterFromName(id)>=0)
            leapDYParameter = getParameterFromName(id);
        }else
            leapDYParameter = -1;
         tmp_xml = macro.FirstChild("leapDZParameter");
        if(tmp_xml.ToNode()){
            string id = tmp_xml.ToElement()->Attribute("id");
            if(id.compare("-1") && getParameterFromName(id)>=0)
            leapDZParameter = getParameterFromName(id);
        }else
            leapDZParameter = -1;
         tmp_xml = macro.FirstChild("leapRollParameter");
        if(tmp_xml.ToNode()){
            string id = tmp_xml.ToElement()->Attribute("id");
            if(id.compare("-1") && getParameterFromName(id)>=0)
            leapRollParameter = getParameterFromName(id);
        }else
            leapRollParameter = -1;
        
        if(!doNotLoadScenes){
            if(scenes_xml.ToNode()){
                
                TiXmlHandle scenes_child_xml = scenes_xml.FirstChild();

                for(int i=0;i<3;i++){
                    if(!scenes_child_xml.ToNode()){
                        loadScene(0, i);
                        continue;
                    }
                    
                    string layerStr = scenes_child_xml.ToElement()->Attribute("layer");
                    int layer = 0;
                    if(!layerStr.compare("1"))
                        layer = 1;
                    else
                        if(!layerStr.compare("2"))
                            layer = 2;
                    
                    TiXmlHandle scene_core_xml = scenes_child_xml.FirstChild(); //the scene
                    
                    string sceneStr = "";
                    auto c  = scene_core_xml.ToElement()->Value() ;
                    if(c!=0)
                     sceneStr = c;
                    
                    if(layer>0 && !sceneStr.compare("cinema")){
                        loadScene(cinemas[0], layer);
                    }else{
                        if(layer>0 && !sceneStr.compare("uzi")){
                            loadScene(uzis[0], layer);
                        }else{
                            Scene* theScene = 0;
                            for(int si=0;si<allScenes.size();si++)
                                if(!allScenes[si]->sceneID.compare(sceneStr)){
                                    theScene = allScenes[si];
                                    break;
                                }
                            loadScene(theScene, layer);
                        }
                    }

                    if(scenes[i] != 0)
                        scenes[i]->loadMacro(&scene_core_xml);
                
                    scenes_child_xml = scenes_child_xml.ToElement()->NextSibling();
                }
            }
        }
    }
    
    ///debug && compatibility
    if( deltaMap[kalei] > 1 ) deltaMap[kalei] = 1;
    
#ifdef THE_CIRCLE
    deltaMap[_mask] = 0;
    deltaMap[borderMask] = 0.5;
#endif
    
    if(macroMorphing>0){
        macroMorphEvo = 0;
        // The following parameters are not morphed
        macroMorphParameters[blendType] = deltaMap[blendType];
        macroMorphParameters[strobe] = deltaMap[strobe];
        macroMorphParameters[_invert] = deltaMap[_invert];
        macroMorphParameters[vMirror] = deltaMap[vMirror];
        macroMorphParameters[hMirror] = deltaMap[hMirror];
        macroMorphParameters[pertEvo] = deltaMap[pertEvo];
        
        if(macroMorphParameters[kaleiNb]<=1 || deltaMap[kaleiNb]<=1)
            macroMorphParameters[kaleiNb] = deltaMap[kaleiNb];
        
        
        //todo data structure with high level parameter and their low level attributes
        if(macroMorphParameters[omg3D2]>0 && deltaMap[omg3D2]==0){
            deltaMap[omg3D2Rotation] = macroMorphParameters[omg3D2Rotation];
            deltaMap[omg3D2Dist] = macroMorphParameters[omg3D2Dist];
            deltaMap[divergenceY] = macroMorphParameters[divergenceY];
            deltaMap[divergence] = macroMorphParameters[divergence];
            deltaMap[omg3D2Alpha0] = macroMorphParameters[omg3D2Alpha0];
            deltaMap[omg3D2Nb] = macroMorphParameters[omg3D2Nb];
        }
        
        if(macroMorphParameters[omg3D2]==0 && deltaMap[omg3D2]>0){
            macroMorphParameters[omg3D2Rotation] = deltaMap[omg3D2Rotation];
            macroMorphParameters[omg3D2Dist] = deltaMap[omg3D2Dist];
            macroMorphParameters[divergenceY] = deltaMap[divergenceY];
            macroMorphParameters[divergence] = deltaMap[divergence];
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
        
        //media POs and rot
        
    }else{
        for(int i=0;i<N_PARAM;i++)
            parameterMap[i] = deltaMap[i];
    }
    
    //BCF2000
    for(int i=0;i<factorymidiCCAutos.size();i++){
        if(factorymidiCCAutos[i]->channel == 1){
        gerardpadOut.sendControlChange(1,
                                       factorymidiCCAutos[i]->maxId,
                                       127 * ( (parameterMap[factorymidiCCAutos[i]->parameterID]-factorymidiCCAutos[i]->values[0]) / (factorymidiCCAutos[i]->values[1] - factorymidiCCAutos[i]->values[0]) ) );
        }
    }

    if(recordAndRenderFlag==1){
        processingStartDate = ofGetElapsedTimeMillis();
        processingStartFrame = ofGetFrameNum();
        startSequenceSave();
    }
        
    }catch(exception e){
        cout << e.what() << endl;
    }
    
#ifdef NEW_GUI
    if(newGUI != NULL)
     newGUI->updateAutos();
#endif
        
    
}

void eqkoscope::loadMacro(string path){
    macroMutex.lock();
    pendingMacros.push_back(path);
    macroMutex.unlock();
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
    string p("");
    while(p.empty() && currentMacroCode < NB_MACRO_PER_PAGE*MAX_MACRO_PAGES){
        int c = currentMacroCode+1;
        if(c<0)
            c = NB_MACRO_PER_PAGE * MAX_MACRO_PAGES;
        setMacroCode(c);
        p = getMacroFromMIDI(currentMacroCode);
    }
    if(!p.empty())
        loadMacro(p);
}

void eqkoscope::loadPrevMacro(){
    string p;
    while(p.empty() && currentMacroCode>=0){
        int c = currentMacroCode-1;
        if(c<0)
            c = NB_MACRO_PER_PAGE * MAX_MACRO_PAGES;
        setMacroCode(c);
        p = getMacroFromMIDI(currentMacroCode);
    }
    if(!p.empty())
        loadMacro(p);
}

string eqkoscope::getMacroFromMIDI(int pitch){
    stringstream path;
    if(macroMap.count(pitch)){
        if(macroMap[pitch].empty())
            return "";
        path << macroPath << macroMap[pitch] << ".xml";
    }
    return path.str();
}
