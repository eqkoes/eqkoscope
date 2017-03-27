#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;

uniform float alphaOffset = 1;
uniform float invert = 0;

void main()
{
    vec4 c =texture(tex0, texCoordVarying);
    float sum = c.r + c.b + c.g;
    if(sum>0)
        outputColor = c;
    else
        outputColor = vec4(0,0,0,0);
    
    if(invert==0)
    outputColor = vec4(c.r,c.g,c.b, sum*alphaOffset);
    else
        outputColor = vec4(1-c.r,1-c.g,1-c.b, sum*alphaOffset);
    }