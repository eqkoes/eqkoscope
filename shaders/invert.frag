#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform float circle = 0;
uniform float WIDTH = 1280;
uniform float HEIGHT = 720;

void main()
{
    vec4 c = texture(tex0, texCoordVarying);
    
    if(circle!=0){
        float x = texCoordVarying.x-WIDTH/2;
        float y = texCoordVarying.y-HEIGHT/2;
        float d = (x*x + y*y)/(HEIGHT/2*HEIGHT/2);
        if((circle>0 && d > circle) || (circle<0 && d < -circle)){
            c.r = 1 -  c.r;
            c.b = 1 -  c.b;
            c.g = 1 -  c.g;
        }
    }else{
        c.r = 1 -  c.r;
        c.b = 1 -  c.b;
        c.g = 1 -  c.g;
    }
    outputColor = c;
}