#version 150
precision mediump float;

#define WIDTH 1024
#define PI 3.141692

uniform sampler2DRect tex0;
out vec4 outputColor;
in vec2 texCoordVarying;
uniform float hue=0;
uniform float hue2 = 0.4;
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

float getAdjustedBrightness(vec4 c){
    float perceivedLightness = (0.2126*c.r + 0.7152*c.g + 0.0722*c.b);
    float br = 1.07 - perceivedLightness/3.5;
    return br;
}

void main(){
    vec4 c = texture(tex0, texCoordVarying);
    vec3 h = rgb2hsv(c);
    h.x = mod((hue*texCoordVarying.x+hue2*(WIDTH-texCoordVarying.x))/(WIDTH*2), 1);
    h.y = saturation;
    c = hsv2rgb(h);
    float corr = getAdjustedBrightness(c);
    c *= vec4(corr, corr, corr, 1);
    outputColor = c;
}
