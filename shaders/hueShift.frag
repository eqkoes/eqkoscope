#version 150
precision mediump float;

#define HEIGHT 720
#define PI 3.141692

uniform sampler2DRect tex0;
out vec4 outputColor;
in vec2 texCoordVarying;
uniform float intensity=0.5;
uniform float center=0;
uniform float range=0.2;
uniform float crazyMode=0;

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
        vec3 hsv = rgb2hsv(c);
    
    float diff = abs(hsv.x - center);
    if(diff < range){
        //        hsv.x = mod(hsv.x + intensity*(1-(diff*diff)/(range*range)), 1);
 if(crazyMode==1)
        hsv.x = mod(hsv.x + intensity/(1-diff/range), 1);
 else
        hsv.x = mod(hsv.x + intensity, 1);
    }
    
        c = hsv2rgb(hsv);
        outputColor = c;
}
