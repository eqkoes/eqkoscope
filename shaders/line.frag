#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform float WIDTH = 1280;
uniform float HEIGHT = 720;
uniform float da = 0.75; // 0.75:centered : 0.25 : out of phase (cool for cinema)
uniform float dx = 0;
uniform float intensity=1;

void main()
{
    float x = texCoordVarying.x/WIDTH;
    float y = texCoordVarying.y/HEIGHT - 0.5;
    
    float a = x;
    float d = 0.5-y;
    a = (2)*(a+da)*3.1416;
    
    vec2 coords;
    float xx = (WIDTH)/2 + d*cos(a)*(HEIGHT/2 + dx*HEIGHT/2);
    float yy = HEIGHT/2 + d*sin(a)*HEIGHT/2;
    
    coords.x = xx*intensity + texCoordVarying.x*(1-intensity);
    coords.y = yy*intensity + texCoordVarying.y*(1-intensity);
    
    outputColor = texture(tex0, coords);
}