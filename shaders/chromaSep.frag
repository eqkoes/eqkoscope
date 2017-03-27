#version 150
precision mediump float;

//#define HEIGHT 768
#define PI 3.141692

uniform sampler2DRect tex0;
out vec4 outputColor;
in vec2 texCoordVarying;
uniform float intensity=0;
uniform float sepAlpha = 1;
uniform float chromaOffset=0;
uniform float chromasepAngle=0;
uniform float chromasepHue=0;
uniform bool chromaSens = true;
uniform float reTint = 0;
uniform float reSaturate = 0;
uniform float chromaSepMode = 0;


uniform float WIDTH = 1;
uniform float HEIGHT = 1;

uniform float brightOffset = 0.1; //offset for detection
uniform float v = 0.6;
uniform float saturation = 0.8;
uniform float sidesSaturation = 1;

uniform float hue=0;

vec3 rgb2hsv(vec4 c){
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec4 hsv2rgb(vec3 c){
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return vec4(c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y), 1);
}

float getAdjustedBrightness(vec4 c){
    float perceivedLightness = (0.2126*c.r + 0.7152*c.g + 0.0722*c.b);
    float br = 1.07 - perceivedLightness/3.5;
    return br;
}

void main(){
    float saturationOffset = max(0, sidesSaturation-1);
    vec2 p = texCoordVarying;
    
    vec2 p1,p2;
    vec2 off1, off2;
    switch(int(chromaSepMode)){
        case 0:{ //classic mode
            off1.x = intensity*HEIGHT*cos(chromasepAngle*2*PI);
            off1.y = intensity*HEIGHT*sin(chromasepAngle*2*PI);
            if(chromaSens){
                off2.x = -intensity*HEIGHT*cos(chromasepAngle*2*PI);
                off2.y = -intensity*HEIGHT*sin(chromasepAngle*2*PI);
            }else{
                off2.x = intensity/2.*HEIGHT*cos(chromasepAngle*2*PI);
                off2.y = intensity/2.*HEIGHT*sin(chromasepAngle*2*PI);
            }
            p1.x = p.x + off1.x;
            p1.y = p.y + off1.y;
            p2.x = p.x + off2.x;
            p2.y = p.y + off2.y;
        }break;
        case 1:{
            p1.x = p.x + (p.x - WIDTH/2) * intensity;
            p1.y = p.y + (p.y - HEIGHT/2) * intensity;
            p2.x = p.x - (p.x - WIDTH/2) * intensity;
            p2.y = p.y - (p.y - HEIGHT/2) * intensity;
        }break;
        case 2:{
            float r = length(p - vec2(WIDTH/2, HEIGHT/2));
            float a = atan(p.y - HEIGHT/2, p.x - WIDTH/2);
            off1.x = (WIDTH/2*intensity*cos(a));
            off1.y = (HEIGHT/2*intensity*sin(a));
            off2.x = -(WIDTH/2*intensity*cos(a));
            off2.y = -(HEIGHT/2*intensity*sin(a));
            p1.x = p.x + off1.x;
            p1.y = p.y + off1.y;
            p2.x = p.x + off2.x;
            p2.y = p.y + off2.y;
        }break;
    }
    
    vec4 color = texture(tex0, p);
    vec4 outCol;
    vec3 h = rgb2hsv(color);
    
    if(reTint!=0){
        h.x = hue;
        if(reSaturate!=0)
            h.y = reSaturate; //B4
    }else{
        if(reSaturate!=0)
            h.y += reSaturate;
    }
    
    outCol = hsv2rgb(h);
    
    if(reTint<1)
        outCol = outCol*reTint + color*(1-reTint);
    
    if(saturation>0 && reTint!=0){
        float correction = getAdjustedBrightness(outCol);
        outCol *= vec4(correction,correction,correction,1);
    }
    
    if(intensity<0.001){
        outputColor = outCol;
        return;
    }
    
    vec4 c1 = texture(tex0, p1);
    vec4 c2 = texture(tex0, p2);
    vec4 mx = vec4(0,0,0, 1);
    float alphaT = 30;
    if((c1.r > brightOffset || c1.g > brightOffset || c1.b > brightOffset) && p1.x>=0 && p1.x < WIDTH && p1.y>=0 && p1.y<HEIGHT){
        vec3 h1 = rgb2hsv(c1);
        h1.x = mod(h1.x*255 + hue*255 + chromasepHue + chromaOffset/2, 255)/255.;
        h1.y = h1.y*sidesSaturation + saturationOffset;
        c1 = hsv2rgb(h1);
        float correction = getAdjustedBrightness(c1);
        c1 *= vec4(correction,correction,correction,1);
        c1 *= sepAlpha;
        mx = vec4((c1.r), (c1.g), (c1.b), 1);
    }
    if((c2.r > brightOffset || c2.g > brightOffset || c2.b > brightOffset) && p2.x>=0 && p2.x < WIDTH && p2.y>=0 && p2.y<HEIGHT){
        vec3 h2 = rgb2hsv(c2);
        h2.x = mod(h2.x*255 + hue*255 + chromasepHue - chromaOffset/2, 255)/255.;
        h2.y = h2.y*sidesSaturation + saturationOffset;
        c2 = hsv2rgb(h2);
        float correction = getAdjustedBrightness(c2);
        c2 *= vec4(correction,correction,correction,1);
        c2 *= sepAlpha;
        mx = vec4(max(c2.r, mx.r), max(c2.g, mx.g), max(c2.b, mx.b), 1);
    }
    
    mx = vec4(max(outCol.r, mx.r), max(outCol.g, mx.g), max(outCol.b, mx.b), 1); //if black keep black of not keep white
    
    outputColor = mx;
}
