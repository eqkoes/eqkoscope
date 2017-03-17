#version 150
precision mediump float;


uniform sampler2DRect tex0;
out vec4 outputColor;
in vec2 texCoordVarying;
uniform float sharpen = 1;
uniform float nPixels = 1;
uniform int mode = 0; //0:both, 1:hor, -1: vert

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

vec4 hsv2rgb2(vec3 c){
    return vec4(0.5*(c.z + c.y*cos(c.x)), 0.5*(c.z + c.y*cos(c.y)), 0.5*(c.z + c.y*cos(c.z)), 1);
}


void main(){
    vec4 Color = texture(tex0, texCoordVarying);
    float sharpenfactor = 10 + sharpen * 40; //Make this between 10 and 50;
    
    vec2 p, m;
    p = texCoordVarying.xy;
    m = texCoordVarying.xy;
    switch(mode){
        case 0:
            p += nPixels;
            m -= nPixels;
            break;
        case 1:
            p.x += nPixels;
            m.x -= nPixels;
            break;
        case -1:
            p.y += nPixels;
            m.y -= nPixels;
            break;
    }

    Color -= texture( tex0, p)*sharpenfactor;
    Color += texture( tex0, m)*sharpenfactor;
    

    Color.a = 1.0;
    
    outputColor = Color;
    
    //foulon's enhancement
    vec3 orig = rgb2hsv(texture(tex0, texCoordVarying));
    vec3 dest = rgb2hsv(outputColor);
    outputColor = hsv2rgb(vec3(orig[0], orig[1], dest[2]));
}

