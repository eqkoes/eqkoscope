#version 150
precision mediump float;

#define LEN 1000

uniform sampler2DRect tex0;
uniform float displace[LEN+1];
uniform float w;
uniform float h;
uniform float x;
uniform float y;
uniform float offx;
uniform float offy;
out vec4 outputColor;
in vec2 texCoordVarying;
uniform float intensity=0;
uniform float vertical=0;

void main(){
    vec2 p = texCoordVarying;
    
    if(abs(p.x-x-offx) < w/2 && abs(p.y-y-offy) < h/2){
        p.x += offx;
        p.y += offy;
    }
    
    outputColor = texture(tex0, p);
}

