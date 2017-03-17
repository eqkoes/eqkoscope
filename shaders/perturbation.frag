#version 150
precision mediump float;

#define M_PI 3.1415

uniform sampler2DRect tex0;
out vec4 outputColor;
in vec2 texCoordVarying;
uniform float WIDTH=1000;
uniform float HEIGHT=1000;
uniform float intensity=0;
uniform float evolution=0;
uniform float freq=7;
uniform float persistance = .5;


float rand(vec2 c){
    return fract(sin(dot(c ,vec2(12.9898,78.233))) * 43758.5453);
}

float noise(vec2 p, float freq ){
    float unit = WIDTH/freq;
    vec2 ij = floor(p/unit);
    vec2 xy = mod(p,unit)/unit;
    xy = .5*(1.-cos(M_PI*xy));
    float a = rand((ij+vec2(0.,0.)));
    float b = rand((ij+vec2(1.,0.)));
    float c = rand((ij+vec2(0.,1.)));
    float d = rand((ij+vec2(1.,1.)));
    float x1 = mix(a, b, xy.x);
    float x2 = mix(c, d, xy.x);
    return mix(x1, x2, xy.y);
}

float pNoise(vec2 p, float res){
    float n = 0.;
    float normK = 0.;
    float f = 4.;
    float amp = 1.;
    int iCount = 0;
    for (int i = 0; i<50; i++){
        n+=amp*noise(p, f);
        f*=2.;
        normK+=amp;
        amp*=persistance;
        if (iCount == int(res)) break;
        iCount++;
    }
    float nf = n/normK;
    return nf*nf*nf*nf;
}

void main(){
    vec2 p = texCoordVarying;
    p.x += intensity * HEIGHT * pNoise(p + (evolution)*vec2(WIDTH,WIDTH), (freq));
    p.y += intensity * HEIGHT * pNoise(p + (evolution+1)*vec2(WIDTH,WIDTH), (freq));
    
    if(p.x>WIDTH)
        p.x -= WIDTH;
    if(p.y>HEIGHT)
        p.y -= HEIGHT;
    outputColor = texture(tex0, p);
}

