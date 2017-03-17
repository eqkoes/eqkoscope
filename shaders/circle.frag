#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform float WIDTH = 1280;
uniform float HEIGHT = 720;
uniform float da = 0;
uniform float alpha = 1;
uniform float amount=1;

uniform float minRad = 0.25;
uniform float maxRad = 0.5;

void main()
{
    float x = (-(WIDTH-HEIGHT)/2 + texCoordVarying.x)/HEIGHT - 0.5;
    float y = texCoordVarying.y/HEIGHT - 0.5;
    
    float a = atan(y,x);

    a = mod(da + a + 3.1416*4.5, 3.1416*2)/(3.1416*2);
    a= (a*2) - 1;
    
    if(a < 0)
        a = -a;
    
    float d = sqrt(x*x + y*y);
    
    if(d<=maxRad && d>=minRad){
        vec2 coords;
        coords.x = (1-amount)*texCoordVarying.x + amount*(WIDTH/2 + WIDTH/2 * a);
        coords.y = (1-amount)*texCoordVarying.y + amount*(HEIGHT - (HEIGHT/2*0 + 1*HEIGHT * d));
        
        vec4 col = texture(tex0, coords);
    
        col.a *= 2*(d);
        col.a *= alpha;
        
        outputColor = col;
    }else{
        outputColor = vec4(0,0,0,0);
    }
}