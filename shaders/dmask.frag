#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform float thresh = 0.1;
//uniform float range = 0.12;
uniform float WIDTH = 1280;
uniform float HEIGHT = 720;

void main()
{
        vec4 c = texture(tex0, texCoordVarying);
        float x = texCoordVarying.x/WIDTH - 0.5;
        float y = texCoordVarying.y/HEIGHT - 0.5;
        float d = sqrt(x*x + y*y);
    
    //entre 0. et 0.1
    //si d == 1-thresh t=1
    //si d == 1-thresh+range t=0
    
    float tt = thresh * 0.5;
    float range = (1-thresh)/2;
    float t = 1;
    if(d>tt)
    t = min(1, range/((d-tt+range)));
    
    c*=t;
//    c.a = 0;
        outputColor = vec4(c.r,c.g,c.b,1);
}
