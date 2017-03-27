#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform float WIDTH = 1280;
uniform float HEIGHT = 720;
uniform float intensity = 1;
uniform float remap = 1;
uniform float cx = 0;
uniform float cy = 0;

void main()
{
    float x = (texCoordVarying.x-WIDTH/2)/WIDTH - cx;
    float y = (texCoordVarying.y-HEIGHT/2)/HEIGHT + cy;
    
    float a = atan(y,x);
    float d = sqrt(x*x + y*y);
    
    d = pow(d, 1-intensity);
    
    if(remap==1)
        d /= 1/(1-intensity);
    
    
        vec2 coords;
        coords.x = (d*cos(a)+0.5)*WIDTH;
        coords.y = (d*sin(a)+0.5)*HEIGHT;
    
    if(coords.x>=0 && coords.x<WIDTH && coords.y>=0 && coords.y<HEIGHT){
        vec4 col = texture(tex0, coords);
        
        outputColor = col;
    }else{
        outputColor = vec4(0,0,0,1);
    }
}