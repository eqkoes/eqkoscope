#version 150
precision mediump float;

uniform sampler2DRect tex0;
uniform float tintMap[255];
out vec4 outputColor;
in vec2 texCoordVarying;

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
    vec4 col = texture(tex0, p);
    float a = col.a;
    vec3 hsb = rgb2hsv(col);
    
//    hsb.r = tintMap[int(hsb.r*255)]/255.0;
    hsb.x = tintMap[10];
    hsb.x /= 255.0;
//    hsb.x = 0.1;
    
    vec4 c2 = hsv2rgb(hsb);
    c2.a = 1;
    outputColor = c2;
}

