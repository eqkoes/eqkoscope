//
//  genetic.h
//  eqkoscope
//
//  Created by Raphaël on 12/07/2017.
//
//

#ifndef eqkoscope_genetic_h
#define eqkoscope_genetic_h

#include "AbstractApp.h"

class Genetic{
  
public:
    Genetic(AbstractApp* app){
        this->app = app;
    }
    
    //eval : pool de n random (de base)
    //selection : pool de weights, garder les n/2 meilleurs
    //croisement / mutation -> nouveau pool croisé
    
    void init(){
    }
    
    
    void eval(float weight){
        cout << "evaluated @ " << weight << endl;
        if(nEval<N){//first evals
        vector<float> map;
        for(int i = 0;i<N_PARAM;i++)
            map.push_back(app->parameterMap[i]);
        maps.push_back(map);
        weights.push_back(weight);
        }else{
            weights[nEval] = weight;
        }
        
        nEval ++;
        if(nEval < N)
            app->randomParameters();
        else{
            if(nEval%N == 0)
                breed();
            exec(nEval);
        }
    }
    
    void breed(){

//        std::sort(weights.begin(), weights.end());
//        for(int x=0;x<weights.size();x++){
//            for(int y=0;y<weights.size();y++){
//                if(weights[x]==){
//                    break;
//                }
//            }
//        }

        
        for(int x=0;x<N;x++){
        int i1 = 0;
        int i2 = 0;
        do{
            i1 = ofRandom(N) + N*int(weights.size()/N - 1);
            i2 = ofRandom(N) + N*int(weights.size()/N - 1);
        }while((weights[i1]<0.5 || weights[i2]<0.5) || (i1==i2));
            cout << "breeding... " << i1 << " and " << i2 << endl;

            vector<float> map;
            for(int i = 0;i<N_PARAM;i++)
                map.push_back(ofLerp(maps[i1][i], maps[i2][i], weights[i2]/weights[i1]));
//                map.push_back(ofLerp(maps[i1][i], maps[i2][i], ofRandom(1)));
//                map.push_back(maps[i1][i]);
            maps.push_back(map);
            weights.push_back(-1);
        }
        
        for(int i=nEval+N-1;i<=nEval;i--)
            mutate(i);
    }
    
    void mutate(int index){
        for(int i = 0;i<N_PARAM;i++)
            if(ofRandom(1)<0.01)
            maps[index][i] = maps[index][i] + ofRandom(0,1);
    }
    
    void exec(int index){
        if(index>=maps.size())
            return;
        cout << "executing " << index << endl;
//        float max = 0;
//        int index = 0;
//        for(int i = 0;i<maps.size();i++)
//            if(max < weights[i]){
//                index = i;
//                max = weights[i];
//            }
        for(int i = 0;i<N_PARAM;i++)
             app->deltaMap[i] = app->parameterMap[i] = maps[index][i];
    }
    
private:
    AbstractApp* app;
    
    int N = 6;
    int nEval = 0;
    
    vector<vector<float> > maps;
    vector<float*> tmpMaps;
    vector<float> weights;
    
};

#endif
