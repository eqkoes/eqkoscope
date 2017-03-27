#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;

void main()
{
    vec4 c =texture(tex0, texCoordVarying);
    float sum = c.r + c.b + c.g;
    if(sum>0)
        outputColor = c;
    else
        outputColor = vec4(0,0,0,0);
}