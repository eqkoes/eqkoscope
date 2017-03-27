#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;

void main()
{
    vec4 c =texture(tex0, texCoordVarying);
    float xx = texCoordVarying.x-500;
    float yy = texCoordVarying.y-300;
    float alpha = sqrt(xx*xx + yy*yy);
    outputColor = vec4(c.r,c.g,c.b,alpha);
}