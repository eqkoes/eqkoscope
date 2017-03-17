#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform sampler2DRect tex1;
uniform float thresh = 0.5;
uniform float WIDTH = 1280;
uniform float corr = 0.5;
uniform float xRatio = 1;
uniform float yRatio = 1;
uniform float multiply = 0;


void main()
{
    vec4 col0 = texture(tex0, texCoordVarying);
    float x = texCoordVarying.x/xRatio;
    float y = texCoordVarying.y/yRatio;
    vec4 col1 = texture(tex1, vec2(x, y));
    
    
    if(multiply==1){
        col0 *= (1-thresh);
        outputColor = vec4(col0.r*col1.r, col0.g*col1.g,col0.b*col1.b, 1);
//        outputColor = vec4(col0.r, col0.g,0, 1);
//        outputColor = col0;
    }else{
        col0 *= (1-thresh);
        outputColor.r = (col1.r < 0.5) ? 2*col1.r*col0.r : 1-2*(1-col1.r)*(1-col0.r);
        outputColor.g = (col1.g < 0.5) ? 2*col1.g*col0.g : 1-2*(1-col1.g)*(1-col0.g);
        outputColor.b = (col1.b < 0.5) ? 2*col1.b*col0.b : 1-2*(1-col1.b)*(1-col0.b);
        outputColor.a = 1; //overlay
    }
}