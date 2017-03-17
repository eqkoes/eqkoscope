#version 150
precision mediump float;

#define WIDTH 1024
#define HEIGHT 768

uniform sampler2DRect tex0;
uniform sampler2DRect map;
out vec4 outputColor;
in vec2 texCoordVarying;
uniform float vertical;

void main(){
    vec2 p = texCoordVarying;
    vec4 m = texture(map, p);
    if(vertical==0)
        p.x = m.r*WIDTH;
    else
        p.y = m.r*HEIGHT;
    outputColor = texture(tex0, p);
}

