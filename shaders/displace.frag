#version 150
precision mediump float;

#define LEN 1000

uniform sampler2DRect tex0;
uniform float displace[LEN+1];
out vec4 outputColor;
in vec2 texCoordVarying;
uniform float intensity=0;
uniform float vertical=0;

void main(){
    vec2 p = texCoordVarying;
    if(vertical==0)
        p.x = p.x + int(displace[int(mod(int(p.y)+1, LEN))]*intensity);
    else
        p.y = p.y + int(displace[int(mod(int(p.x)+1, LEN))]*intensity);
    outputColor = texture(tex0, p);
}

