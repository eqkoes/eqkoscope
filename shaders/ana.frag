#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform float gamma = 2.2;

uniform int WIDTH 1024
uniform int HEIGHT 768

uniform float x1 0
uniform float y1 0
uniform float x2 1024
uniform float y2 0
uniform float x3 0
uniform float y3 768
uniform float x4 1024
uniform float y4 768

void main()
{
    int x = x1 + (x2-x1)/2.0;
    int y = ;
        vec4 col0 = texture(tex0, texCoordVarying);
    
        outputColor = ;
}