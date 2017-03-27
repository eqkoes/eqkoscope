#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform float trapeze = 0;

uniform float alphaOffset = 1;
uniform float HEIGHT = 720;
uniform float WIDTH = 1280;

void main()
{
    vec2 coords = texCoordVarying;
    coords.y -= HEIGHT/2;
    if(trapeze>=0)
        coords.y /= (1-trapeze) + (coords.x/WIDTH)*(trapeze);
    else
        coords.y /= (1-abs(trapeze)) + (1-coords.x/WIDTH)*(-trapeze);
    coords.y += HEIGHT/2;
    if(coords.y<0 || coords.y>HEIGHT)
        outputColor = vec4(0,0,0,1);
    else
        outputColor = texture(tex0, coords);
}