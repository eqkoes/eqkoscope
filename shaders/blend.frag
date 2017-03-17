#version 150

#define ALPHABLACK 0
#define SCREEN 1
#define TRUED 2
#define MULTIPLY 3
#define ADD 4
#define SUBTRACT 5
#define INCRUSTATION 6
#define PROLLAGE 7
#define SUPERPOSE 8
#define NORMAL 9


uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform sampler2DRect tex1;

uniform float thresh = 0.5;
uniform float _mix = 1;
uniform float WIDTH = 1280;
uniform float HEIGHT = 720;
uniform float corr = 0.5;
uniform int mode = 3;

uniform float prollageFreq = 0.01;
uniform float trapeze = 0;

void main()
{
    
    vec2 coords = texCoordVarying;
    coords.y -= HEIGHT/2;
    if(trapeze>=0)
        coords.y /= (1-trapeze) + (coords.x/WIDTH)*(trapeze);
    else
        coords.y /= (1-abs(trapeze)) + (1-coords.x/WIDTH)*(-trapeze);
    coords.y += HEIGHT/2;
    if(coords.y<0 || coords.y>HEIGHT){
        outputColor = vec4(0,0,0,1);
    }else{
        vec4 c = texture(tex0, coords);
        if(mode==MULTIPLY){
            vec4 col1 = texture(tex1, coords);
            //        col0 *= (1-thresh);
            outputColor = vec4(c.r*col1.r, c.g*col1.g, c.b*col1.b, 1);
        }else{
            if(mode==ALPHABLACK){
                float mean = (c.r + c.b + c.g) / 3.0 * 3.0;
                
                if(mean<thresh)
                    outputColor = vec4(0,0,0,0);
                else
                    outputColor = vec4(c.r,c.g,c.b,c.a*(mean-thresh)/(1-thresh)*_mix);
            }else{
                if(mode==INCRUSTATION){
                    vec4 col1 = texture(tex1, coords);
                    c *= (1-thresh);
                    outputColor.r = (col1.r < 0.5) ? 2*col1.r*c.r : 1-2*(1-col1.r)*(1-c.r);
                    outputColor.g = (col1.g < 0.5) ? 2*col1.g*c.g : 1-2*(1-col1.g)*(1-c.g);
                    outputColor.b = (col1.b < 0.5) ? 2*col1.b*c.b : 1-2*(1-col1.b)*(1-c.b);
                    outputColor.a = 1; //overlay
                }else{
                    if(mode==PROLLAGE){
                        if(mod(texCoordVarying.x, prollageFreq*WIDTH) < prollageFreq*WIDTH/2)
                            c.w = 0;
                        outputColor = c;
                    }else{
                        if(mode==SCREEN){
                            vec4 col1 = texture(tex1, coords);
                            outputColor = max(c, col1);
                            
                        }else{
                            if(mode==ADD){
                                vec4 col1 = texture(tex1, coords);
                                outputColor = c + col1;
                            }else{
                                if(mode==SUBTRACT){
                                    vec4 col1 = texture(tex1, coords);
                                    outputColor = c - col1;
                                }else{
                                    if(mode==SUPERPOSE){
                                        vec4 col1 = texture(tex1, coords);
                                        if(c.z==0)
                                            outputColor = col1;
                                        else
                                            outputColor = c;
                                    }else{
                                        if(mode==NORMAL){
                                            c.a = _mix;
                                            outputColor = c;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}