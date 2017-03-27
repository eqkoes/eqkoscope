#version 150
precision mediump float;

#define WIDTH 1024
#define HEIGHT 768
#define PI 3.141692

uniform sampler2DRect tex0;
out vec4 outputColor;
in vec2 texCoordVarying;
uniform float factor = 0.05;


uniform float brightOffset = 0.1; //offset for detection
uniform float v = 0.8;
uniform float saturation = 1;

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
    vec2 p = texCoordVarying / vec2(WIDTH, HEIGHT) - 0.5;
    
    float r = length(p);
    float a = atan(p.y,p.x);
    
    vec2 px = (pow(r,1+factor*4) * vec2(cos(a), sin(a)) + 0.5) * vec2(WIDTH, HEIGHT);
    
    float off1 = pow(r,1+factor*4)*100;
    float off2 = -off1;
    vec2 p1,p2;
    p1.x = px.x + off1;
    p1.y = px.y;
    p2.x = px.x + off2;
    p2.y = px.y;
    
    vec4 color = texture(tex0, px);
    vec4 c1 = texture(tex0, p1);
    vec4 c2 = texture(tex0, p2);
    
    outputColor = color;
    
    if(c1.r > brightOffset || c1.g > brightOffset || c1.b > brightOffset){
        vec3 h1 = rgb2hsv(c1);
        h1.x = 0;
        h1.y = saturation;
        h1.z = min(v, h1.z);
        c1 = hsv2rgb(h1);
        outputColor += c1;
    }
    if(c2.r > brightOffset || c2.g > brightOffset || c2.b > brightOffset){
        vec3 h2 = rgb2hsv(c2);
        h2.x = 0.5;
        h2.y = saturation;
        h2.z = min(v, h2.z);
        c2 = hsv2rgb(h2);
        outputColor += c2;
    }
}
