//
//  gui.h
//  eqkoscope
//
//  Created by Raphaël on 03/05/2018.
//
//

#ifndef eqkoscope_gui_h
#define eqkoscope_gui_h

#include <stdio.h>
#include <stdlib.h>
#include "ofGraphics.h"
#include "AbstractApp.h"
#include "auto.h"

const static int EqkoGUIEltWidth = 200;
const static int EqkoGUIEltHeight = 20;

static ofTrueTypeFont guiFont;

class EqkoGUIComponent{
public:
    
    EqkoGUIComponent(){}
    
    EqkoGUIComponent(int x, int y, int width, int height){
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    }
    
    void setBounds(int x, int y, int width, int height){
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    }
    
    void setVisible( bool b ){
        visible = b;
    }
    
    bool isVisible(){
        return visible;
    }
    
    void setSelected(bool b){
        selected = b;
    }
    
    bool isSelected(){
        return selected;
    }
    
    bool isPressed(int mx, int my){
        return mx>=x && mx < x+width && my>=y && my  < y+height;
    }
    
    int x, y,width, height;
    
protected:
    bool visible = true;
    bool selected = false;
};

template<typename T>
class EqkoGUIParam: public EqkoGUIComponent{
public:

    EqkoGUIParam(int id, string name, T min=0, T max=1)
    : EqkoGUIComponent(0,0,100,25){
        this->name = name;
        this->id = id;
        this->min = min;
        this->max = max;
    }
    
    void setID(int id){
        this->id = id;
    }
    
    void setMinMax(T min, T max){
        this->min = min;
        this->max = max;
    }
    
    void setValue(T v){
        value = v;
    }
    
    void draw(){
        if(!visible)
            return;
        
        int alpha = 255;
        
        if(selected)
        ofSetColor(ofColor::white);
            else
                ofSetColor(ofColor(50,50,50,150));
        
        ofRectangle(x, y, width-1, height-1);
        
        if(showValue){
        if(selected)
            ofSetColor(ofColor(164,16,52));
        else
        ofSetColor(ofColor(164,16,52, alpha));
        ofRectangle(x, y, (width-1) * app->normalizedGUIValue(id, value), EqkoGUIEltHeight);
        }

        if(!selected)
            ofSetColor(ofColor(255,255,255,alpha));
        else
            ofSetColor(ofColor(0,0,0));
        guiFont.drawString(name, x, y+14);
        
        if(showValue){
        guiFont.drawString(ofToString(value), x+width-50, y+14);
        }
    }
    
    void setValueVisible(bool b){
        showValue = b;
    }
    
    int getID(){
        return id;
    }
    
    
private:
    string name;
    int id;
    T min, max;
    T value;
    bool showValue = true;

};

class EqkoGUICombo: public EqkoGUIComponent{
public:
    EqkoGUICombo(std::string str){
        enumStr = ofSplitString(str, " ");
    }
    
    void draw(){
        int alpha = 255;
        
        ofSetColor(ofColor(0,0,0,150));
        ofRectangle(x, y, EqkoGUIEltWidth-1, EqkoGUIEltHeight-1);
        
        ofSetColor(ofColor(255,255,255,alpha));
        guiFont.drawString(enumStr[selected], x, y+14);
    }
    
    void up(){
        setSelectedIndex((selected+1)%enumStr.size());
    }
    
    void setSelectedIndex(int i){
        selected = i;
    }
    
    int getSelectedIndex(){
        return selected;
    }
    
private:
    std::vector<std::string> enumStr;
    int selected = 0;
};


class EqkoGUIAuto: public EqkoGUIComponent{
public:
    EqkoGUIAuto(AbstractApp *app, Auto* a){
        this->app = app;
        _auto = a;
        
        width = eltWidth*5;
        height = EqkoGUIEltHeight;
        
        update();
    }
    
    ~EqkoGUIAuto(){
        delete typeCombo;
        delete channelCombo;
        delete theParam;
        delete pMin;
        delete pMax;
    }
    
    void update(){
        if(typeCombo!=0)
        delete typeCombo;
        if(channelCombo!=0)
 delete channelCombo;
        if(theParam!=0)
  delete theParam;
        if(pMin!=0)
  delete pMin;
        if(pMax!=0)
  delete pMax;
        
        int w = 0;
        int pad = 10;
        
        typeCombo = new EqkoGUICombo("AUDIO CC NOTEON BPM TIMED");
        typeCombo->setBounds(0, 0, 125, EqkoGUIEltHeight);
        
        w += typeCombo->width + pad;
        
        theParam = new EqkoGUIParam<float>(_auto->parameterID, app->parameterIDMap[_auto->parameterID],
                                           _auto->values[0], _auto->values[1]);
        theParam->setValueVisible(false);
        theParam->setBounds(w, 0, 150, EqkoGUIEltHeight);
        
        w += theParam->width + pad;
        
        pMin = new EqkoGUIParam<float>(0, "min", 0, 1);
        pMax = new EqkoGUIParam<float>(0, "max", 0, 1);
        pMin->setBounds(w, 0, 125, EqkoGUIEltHeight);
        w += pMin->width + pad;
        pMax->setBounds(w, 0, 125, EqkoGUIEltHeight);
        w += pMax->width + pad;
        
        timeParam = new EqkoGUIParam<float>(eltWidth*4, "Tau", 1, 16);
        timeParam->setBounds(w, 0, 100, EqkoGUIEltHeight);
        channelCombo = new EqkoGUIParam<int>(0, "Ch.", 0, 10);
        channelCombo->setBounds(w, 0, 100, EqkoGUIEltHeight);
        w += timeParam->width + pad;

        switch(_auto->type){
            case AUDIO: typeCombo->setSelectedIndex(0);
                break;
            case MIDICC: typeCombo->setSelectedIndex(1);
                break;
            case MIDION: typeCombo->setSelectedIndex(2);
                break;
            case BPM: typeCombo->setSelectedIndex(3);
                break;
            case TIMED: typeCombo->setSelectedIndex(4);
                break;
        }
        channelCombo->setValue(_auto->channel);
        
        channelCombo->setVisible(_auto->type == AUDIO
                                 || _auto->type == MIDICC
                                 || _auto->type == MIDION);
        timeParam->setVisible(_auto->type == BPM
                                 || _auto->type == TIMED);
        
//        pMin->setMinMax(_auto->values[0], _auto->values[1]);
//        pMax->setMinMax(_auto->values[0], _auto->values[1]);
        pMin->setID(_auto->parameterID);
        pMax->setID(_auto->parameterID);
    }
    
    void draw(){
//        if(selected)
//            ofSetColor(ofColor(164,16,52,255));
//        else
//            ofSetColor(ofColor(50,50,50,150));
//        ofRect(0, 0, width, height);
        
        pMin->setValue( _auto->values[0] );
        pMax->setValue( _auto->values[1] );
        
        if(_auto->type == BPM)
        timeParam->setValue(_auto->BPMDivision );
        else
            if(_auto->type == TIMED)
                timeParam->setValue(_auto->uDuration );
        
        channelCombo->setValue(_auto->channel);
        
        ofPushMatrix();
        typeCombo->draw();
        theParam->draw();
        pMin->draw();
        pMax->draw();
        channelCombo->draw();
        timeParam->draw();
//        IDCombo->draw();
        ofPopMatrix();
    }
    
    bool mousePressed(int mx, int my){
        if(!isPressed(mx, my))
            return false;
        
        mx -= x;
        my -= y;
        
        typeCombo->setSelected(false);
        pMin->setSelected(false);
        pMax->setSelected(false);
        theParam->setSelected(false);
        channelCombo->setSelected(false);
        
        if(pMin->isPressed(mx, my)){
            pMin->setSelected(true);
        }else if(pMax->isPressed(mx, my)){
            pMax->setSelected(true);
        }else if(theParam->isPressed(mx, my)){
            theParam->setSelected(true);
        }else if(channelCombo->isPressed(mx, my)){
            channelCombo->setSelected(true);
        }
        
        
        return pMin->isSelected() || pMax->isSelected()
        || theParam->isSelected() || channelCombo->isSelected();
    }
    
    void setSelected(bool b){
        EqkoGUIComponent::setSelected(b);
        if(!b){
            theParam->setSelected(false);
            pMin->setSelected(false);
            pMax->setSelected(false);
            channelCombo->setSelected(false);
            timeParam->setSelected(false);
        }
    }
    
    
    void mouseDragged(int mx, int my){
        if(pMin->isSelected()){
            float value = app->scaleGUIValue(pMin->getID(), ofGetMouseX()/(float)WIDTH);
            _auto->values[0] = value;
        }else if(pMax->isSelected()){
            float value = app->scaleGUIValue(pMax->getID(), ofGetMouseX()/(float)WIDTH);
            _auto->values[1] = value;
        }if(channelCombo->isSelected() ){
            _auto->channel = (ofMap(ofGetMouseX(), 0, float(WIDTH), 0, 10));
        }
    }
    
    void setParamID(int paramID){
        _auto->parameterID = paramID;
        update();
    }
    
    bool iChannelMod(){
        return channelCombo->isSelected();
    }
    
    bool iParamMod(){
        return theParam->isSelected();
    }
    
    void setChannel(int ch){
        _auto->channel = ch;
        update();
    }
    
private:
    AbstractApp *app;
    Auto* _auto;
    EqkoGUICombo* typeCombo=0;
    EqkoGUIParam<int> *channelCombo=0;
    EqkoGUIParam<float> *theParam=0;
    EqkoGUIParam<float> *timeParam=0;
    EqkoGUIParam<float>* pMin=0,*pMax=0;
    
    int eltWidth = 175;
};



class EqkoGUI{
public:
    EqkoGUI(AbstractApp *app){
        this->app = app;
        guiFont.load("Helvetica", 13);
    }
    
    void updateAutos(){
        for(EqkoGUIAuto* a : autos)
            delete a;
        autos.clear();
        for(int i = 0;i<app->getNumberOfAutos();i++){
            autos.push_back( new EqkoGUIAuto(app, app->getAuto(i) ) );
        }
        
        pack();
    }
    
    void draw(){
        int id =0;
        ofPushMatrix();
        ofTranslate(ox, oy);

        for(int x=0;x<w;x++){
            for(int y=0;y<h;y++){
                params[id].setValue(app->deltaMap[params[id].getID()]);

                params[id].draw();

                id++;
            }
        }
    
        
        for(EqkoGUIAuto* a : autos){
            ofPushMatrix();
            ofTranslate(a->x, a->y);
            a->draw();
            ofPopMatrix();
        }

        ofPopMatrix();
    }
    
    void mousePressed(int mx, int my){
        mx -=ox;
        my -=oy;
        
        ///find the selected parameter
        {
            int the_id = -1;
            int id = 0;
            for(int x=0;x<w;x++){
                for(int y=0;y<h;y++){
                    if(params[id].isPressed(mx, my)){
                        if( ! params[id].isSelected()){
                            the_id = id;
                            params[id].setSelected(true);
                            if(selectedAuto>=0 && autos[selectedAuto]->iParamMod()){
                                autos[selectedAuto]->setParamID(params[id].getID());
                                break;
                            }
                            
                            selectedParameter = params[id].getID();
                            selectedAuto = -1;
                        }else{
                            params[id].setSelected(false);
                            selectedParameter = -1;
                        }
                        
                    }else{
                        params[id].setSelected(false);
                    }
                    id++;
                }
            }
        }
        
        
//        if(selectedParameter==-1){
            int id = 0;
        for(EqkoGUIAuto* a : autos){
            if(a->mousePressed(mx, my)){
                a->setSelected(true);
                selectedAuto = id;
            }
//            else
//                a->setSelected(false);
            id++;
        }
//        }
    }
    
    void mouseDragged(int mx, int my){
        if(selectedAuto>=0){
            EqkoGUIAuto* a = autos[selectedAuto];
            a->mouseDragged(mx,my);
        }else if(selectedParameter>=0){
            app->deltaMap[selectedParameter] = app->parameterMap[selectedParameter] = app->scaleGUIValue(selectedParameter, ofMap(mx, 0, float(WIDTH), 0, 1));
        }
    }
    
    void addParam(int id, string name){
        params.push_back(EqkoGUIParam<float>(id, name));
        paramMap[id] = &(params[params.size()-1]);
        pack();
    }
    
    void addParam(int id, string name, float min, float max){
        params.push_back(EqkoGUIParam<float>(id, name, min, max));
        paramMap[id] = &(params[params.size()-1]);
        pack();
    }
    
    void setParamValue(int id, float value){
        paramMap[id]->setValue(value);
    }
    
    void pack(){
        w = 1 + params.size() / 25;
        h = params.size() / w;
        
        int id = 0;
        for(int x=0;x<w;x++){
            for(int y=0;y<h;y++){
                params[id].width = EqkoGUIEltWidth;
                params[id].height = EqkoGUIEltHeight;
                    params[id].x = x*EqkoGUIEltWidth;
                    params[id].y = y*EqkoGUIEltHeight;
                id++;
            }
        }
        
        int ai = 0;
        for(EqkoGUIAuto* a : autos){
            a->x = 0;
            a->y = h*EqkoGUIEltHeight + 25 + EqkoGUIEltHeight*(ai);
            ai++;
        }
    }
    

    
private:
    int w = 0;
    int h = 0;

    int ox = 25, oy=25;
    
    std::vector<EqkoGUIParam<float> > params;
    std::map<int, EqkoGUIParam<float>*> paramMap;
    
    std::vector<EqkoGUIAuto*> autos;
    
    AbstractApp* app;
    
    int selectedParameter=-1;
    int selectedAuto=-1;
};




#endif
