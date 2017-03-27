#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;

float getAdjustedBrightness(vec4 c){
    float perceivedLightness = (0.2126*c.r + 0.7152*c.g + 0.0722*c.b);
    float br = 1.05 - perceivedLightness/4.0;
    return br;
}

vec4 graytoHSL(float g)
{
    return vec4( hue, sat, lum, col.a );
}

void main()
{
    vec4 c = texture(tex0, texCoordVarying);
    float g = (c.r + c.g + c.b)/3.0;
    c = convertRGBtoHSL(g);
    c *= getAdjustedBrightness(c);
    outputColor = vec4(c, c.a);
}