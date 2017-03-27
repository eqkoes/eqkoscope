#version 150
precision mediump float;

#define HEIGHT 720
#define PI 3.141692

uniform sampler2DRect tex0;
out vec4 outputColor;
in vec2 texCoordVarying;
uniform float hue=0;
uniform float saturation = 0.8;

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

void main(){
    vec2 p = texCoordVarying;
    
    vec4 c = texture(tex0, p);

        vec3 h = rgb2hsv(c);
    
    //T1
//    float d = h.x;
//    float a = h.y;
//    p.x += d*cos(a)*100;
//    p.y += d*sin(a)*100;
    
    //FIRE
//    float d = h.x;
//    float a = h.z>0?0:PI;
//    p.x += d*cos(a)*100;
//    p.y += d*sin(a)*100;
    
    //reflow
//    float d = 5;
//    float a = h.z>0?PI/2:3*PI/2;
//    p.x += d*cos(a);
//    p.y += d*sin(a);
    
    float d = (h.z-0.5)*5;
    float a = (h.x-0.5)*PI*2;
    d = 15;
    a = atan((p.y-640)/(p.x-360));
//    p.x += d*cos(a);
//    p.y += d*sin(a);
    if(p.x <640)
    p.x += 10;
    else
        p.x -= 10;
    
    outputColor = texture(tex0, p);
}
