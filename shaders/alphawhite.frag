#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform sampler2DRect tex1;
uniform float thresh = 0.5;
uniform float thresh_pred = 1;
uniform float WIDTH = 1280;
uniform float corr = 0.5;
uniform float xRatio = 1;
uniform float yRatio = 1;


void main()
{
    vec4 col0 = texture(tex0, texCoordVarying);
    float x = texCoordVarying.x/xRatio;
    float y = texCoordVarying.y/yRatio;
    vec4 col1 = texture(tex1, vec2(x, y));
    
    col0 *= thresh;
    
//    float mix2 = ((thresh-thresh_pred)+1)/2;
//    float mix = (thresh-thresh_pred);
    
    outputColor = pow(col1 * col0, vec4(corr,corr,corr,corr));
    
//    outputColor = pow(col1 * col0 * abs(1-mix) + 0*mix*col0 + 0*(1-mix)*col1 , vec4(corr,corr,corr,corr));
}