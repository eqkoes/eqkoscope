#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform float WIDTH = 1280;
uniform float HEIGHT = 720;
uniform float freq = 0.2;
uniform float yShift = 0.2;
uniform float xShift = 0.2;

void main()
{
    vec2 pos = texCoordVarying - vec2(WIDTH/2 , HEIGHT/2);
    
    if(mod(pos.x, WIDTH*freq) < WIDTH*(freq/2)){
        pos.x += xShift*HEIGHT/2;
        pos.y += yShift*HEIGHT/2;
    }else{
        pos.x -= xShift*HEIGHT/2;
        pos.y -= yShift*HEIGHT/2;
    }
    
    pos *= vec2(1-xShift, 1-yShift);
    
    outputColor = texture(tex0, pos + vec2(WIDTH/2 , HEIGHT/2));
}