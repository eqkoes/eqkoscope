#ifndef eqkoscope_print_h
#define eqkoscope_print_h

#include "scene.h"
#include <ofGraphics.h>

class Print: public Scene{
    
public:
    
    Print(AbstractApp* app)
        :Scene(app, "print"){
            sceneID = "print";

            
            font.loadFont("Arial", app->parameterMap[fontSize]);
            load();
            initParameters();
    }
    
    void load(){
        fonts.push_back("Arial");
        fonts.push_back("Helvetica");
        fonts.push_back("Verdana");
        fonts.push_back("Gill Sans");
        fonts.push_back("Cochin");
        fonts.push_back("Didot");
        fonts.push_back("Copperplate");
        fonts.push_back("Baskerville");
        fonts.push_back("American Typewriter");
        fonts.push_back("Avenir");
        fonts.push_back("Futura");
        //        fonts.push_back("Damascus");
        fonts.push_back("Euphemia UCAS");
        //        fonts.push_back("Farisi");
//        fonts.push_back("Kai");
        //        fonts.push_back("Khmer MN");
        //        fonts.push_back("Kokonor");
        fonts.push_back("Palatino");
        fonts.push_back("Skia");
        //        fonts.push_back("Symbol");
        fonts.push_back("Adobe Garamond Pro");
        fonts.push_back("Charlemagne Std");
        fonts.push_back("Courier");
        //        fonts.push_back("GunSeo");
        
        
        ofDirectory dir("print/");
        dir.listDir();
        vector<string> strs;
        for(int i=0;i<dir.numFiles();i++){
            string d = dir.getPath(i);
            if(d.find(".txt")!=string::npos){
                int count = 0;
                ofFile file(d);
                string text = file.readToBuffer().getText();
                size_t pos1 = 0, pos2=1000000000; //parsing
                string delimiter = " ";
                while (
                       (
                           ((pos1 = text.find(delimiter)) != std::string::npos)
                           || ((pos2 = text.find("\n")) != std::string::npos)
                           )
                       && (app->parameterMap[wordLimit] <1 || app->parameterMap[wordLimit] >count)) {
                    int pos = min(pos1, pos2);
                    string token = text.substr(0, pos);
                    if(token.find("\n") != std::string::npos){
                    }else{
                        strs.push_back(token);
                    }
                    text.erase(0, pos + delimiter.length());
                    count++;
                    pos2=1000000000;
                    
                }
            }
            cout << strs.size() << endl;
            db.push_back(strs);
        }
        //markovAnalysis();
    }
    
    void markovAnalysis(){
        //        markovMap.erase();
        for(int i=0;i<1;i++){
            for(int str=0;str<db[i].size();str++){
                string word = db[i][str];
                if(markovMap.count(word)==0){
                    markovMap[word] = map<string, float>();
                }
                map<string, float> probas;
                for(int str2=0;str2<db[i].size();str2++){
                    markovMap[word][db[i][str2]]=0;
                }
                for(int str2=1;str2<db[i].size();str2++){
                    if(!db[i][str2-1].compare(word))
                        markovMap[word][db[i][str2]]++;
                }
            }
        }
        
    }
    
    void setup(){
    }
    
    void exit(){
        
    }
    void draw(){
        if(ofGetElapsedTimeMillis()-app->parameterMap[txt_lastDate]>=app->parameterMap[txt_rate] || 1==1){
            ofSetColor(ofColor::black);
            ofRect(0,0,WIDTH,HEIGHT2);
            ofSetColor(ofColor::white);
            
            float strw = font.stringWidth(currentText);
            float strh = font.stringHeight(currentText);
            
            if(app->parameterMap[txt_randPosition]){
                for(int i=0;i<app->parameterMap[nWords];i++){
                    
                    int x = WIDTH/2-strw/2;
                    int y = HEIGHT2/2 + 100 - strh/2;
                    x += (ofRandom(WIDTH) - WIDTH/2)*((WIDTH-strw)/(float)WIDTH);
                    y += (ofRandom(HEIGHT) - HEIGHT/2)*((HEIGHT-strh)/(float)(HEIGHT));
                    font.drawString(currentText, x, y);
                }
            }else{
                int x = WIDTH/2-strw/2;
                int y = HEIGHT2/2 + 25;
                float fillw = app->parameterMap[txt_accumulate] ? WIDTH/strw : 1;
                float fillh = app->parameterMap[txt_accumulate] ? HEIGHT/strh : 1;
                for(int xx=0;xx<fillw;xx++)
                    for(int yy=0;yy<fillh;yy++)
                        font.drawString(currentText, x + 1.2*strw*(xx-fillw/2+0.5), y + 1.2*strh*1.1*(yy-fillh/2+0.5));
            }
        }
    }
    void mask(){
        
    }
    
    void capture(ofFbo* fbo){
        
    }
    
    void update(){
        if(ofGetElapsedTimeMillis()-txt_lastDate>=app->parameterMap[txt_rate]){
            txt_lastDate = ofGetElapsedTimeMillis();
            if(app->parameterMap[randomFont]){
                int currentFont = ofRandom(fonts.size()-0.01);
                currentFontFamily = fonts[currentFont];
                font.loadFont(currentFontFamily, app->parameterMap[fontSize] + ofRandom(-app->parameterMap[txt_randSize],app->parameterMap[txt_randSize]));
                font.setLineHeight(app->parameterMap[fontSize]);
            }
            
            if(app->parameterMap[randomText] || app->parameterMap[seqText]){
                if(app->parameterMap[randomText]){
                int tmp;
                do{
                    tmp = (int)ofRandom(db[dbIndex].size() - 0.01);
                }while( tmp==curIndex && db[dbIndex].size()>1
                        // && markovMap[db[dbIndex][curIndex]][db[dbIndex][tmp]]<4
                        );
                curIndex = tmp;
                }else{
                   curIndex = int((++curIndex) % db[dbIndex].size());
                }
                if(app->parameterMap[txt_accumulate]){
                    currentText.append(db[dbIndex][curIndex]);
                    currentText.append(" ");
                    if(ofRandom(1)<0.1)
                        currentText.append("\n");
                    if(currentText.length()>1000)
                        currentText.erase(0, currentText.length()-1000);
                }else{
                    currentText = db[dbIndex][curIndex];
                }
            }
        }
        
        if(app->parameterMap[fontSize]!=font.getSize())
//            font.loadFont(currentFontFamily, app->parameterMap[fontSize]);
            font.setLineHeight(app->parameterMap[fontSize]);
}
    
    void mousePressed(int x, int y, int button){}
  void mouseDragged(int x, int y, int button){
        
    }
    void mouseMoved(int x, int y){}

    void touchMoved(ofTouchEventArgs &touch){
        
    }
    
    void keyPressed(int key){
        
    }
    
    void midiEvent(ofxMidiMessage& eventArgs){
        
    }
    void oscEvent(std::string header, std::string arg){
        
    }
    void oscEvent(std::string header, std::vector<float> args){
        
    }
    
    void saveMacro(stringstream *xml){
        (*xml) << "<print>" << endl;
        
        (*xml) << "<currentText>" << currentText << "</currentText>" << endl;
        (*xml) << "<currentFontFamily>" << currentFontFamily << "</currentFontFamily>" << endl;

        (*xml) << "</print>" << endl;

    }
    
    void loadMacro(ofXml *xml){
        xml->setTo("print");
        
        currentText = xml->getValue("currentText");
        currentFontFamily = xml->getValue("currentFontFamily");
        
        xml->setToParent();
        
        font.loadFont(currentFontFamily, app->parameterMap[fontSize] + ofRandom(-app->parameterMap[txt_randSize],app->parameterMap[txt_randSize]));
        font.setLineHeight(app->parameterMap[fontSize]);
    }
    
    std::string getInfo(){
        return "";
    }
    
    bool isBackground(){return back;}
    
    std::string currentText = "SAMPLE TEXT LEL";
    std::string currentFontFamily = "";
    std::vector<std::vector<std::string> > db;
    std::vector<std::string> fonts;
    int dbIndex = 0;
    int curIndex = 0;
    
    int txt_lastDate = 0;
    
    
    ofTrueTypeFont font;
    
    std::map<std::string, std::map<std::string, float> > markovMap;
};


#endif
