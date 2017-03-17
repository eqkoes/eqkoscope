#version 150
precision mediump float;

#define LEN 1000
//#define WIDTH 1024
//#define HEIGHT 768

uniform float WIDTH = 1;
uniform float HEIGHT = 1;

uniform sampler2DRect tex0;
out vec4 outputColor;
in vec2 texCoordVarying;
uniform float skew[LEN];
uniform float offset=0;
uniform float intensity=0;
uniform float vertical=0;
uniform float dist=0;
uniform float angle=0;

void main(){
    vec2 p = texCoordVarying-vec2(WIDTH/2,HEIGHT/2);
    
    if(angle==1){
        float d = length(p);
        float a = atan(p.y, p.x);
        a = mod(a + (skew[int(mod(d+offset, LEN))])*intensity*0.1, HEIGHT);
        p.x = d*cos(a);
        p.y = d*sin(a);
    }else{
        if(dist==1){
            float d = length(p);
            float a = atan(p.y, p.x);
            d = mod(d + (skew[int(mod(d+offset, LEN))])*intensity, HEIGHT);
            p.x = d*cos(a);
            p.y = d*sin(a);
        }else{
            if(vertical==0){
                p.x = mod(WIDTH/2 + p.x + skew[int(mod(p.y+offset, LEN))]*intensity, WIDTH) - WIDTH/2;
            }else{
                p.y = mod(HEIGHT/2 + p.y + skew[int(mod(p.x+offset, LEN))]*intensity, HEIGHT)  - HEIGHT/2;
        }
        }
    }
    
    
    outputColor = texture(tex0, p+vec2(WIDTH/2, HEIGHT/2));
}

