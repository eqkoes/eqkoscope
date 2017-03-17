#version 150
precision mediump float;

uniform float WIDTH = 1;
uniform float HEIGHT = 1;
uniform sampler2DRect tex0;
out vec4 outputColor;
in vec2 texCoordVarying;
uniform float sides = 3; // 0 : v mirror, 1 : h mirror
uniform float copy = 1; //multiple exposure
uniform float copyAdjust = 0; //adjust brightness
uniform float x=0,y=0;
uniform float copyBorder = 0;
uniform float scale = 2;
uniform float mirror = 0;

void main(){
    
    vec4 color = vec4(0,0,0,1);
    vec2 p = texCoordVarying - vec2(WIDTH/2,HEIGHT/2);
    
    if(sides>1){
        //cart to polar
        float r = length(p);
        float a = atan(p.y, p.x);
        float tau = 2. * 3.1416;
        
        if(copy==1){
            for(float i=0;i<sides;i+=0.5){
                float aa = mod(a - 0.5*3.14, tau/sides);
                aa = abs(aa - tau/sides/2.);
                aa += 2*3.14*i/sides;
                p = r * vec2(cos(aa), sin(aa));
                if(scale>1){
                    p *= scale;
                    p.x = mod(p.x+HEIGHT/2,HEIGHT)-HEIGHT/2;
                    p.y = mod(p.y+HEIGHT/2,HEIGHT)-HEIGHT/2;
                }
                if(copyBorder==1 || (abs(p.x+x)<WIDTH/2 && abs(p.y+y)<HEIGHT/2)){
                    if(copyAdjust>0)
                        color += texture(tex0, p+vec2(WIDTH/2,HEIGHT/2) + vec2(x,y)) / (2*sides*copyAdjust);
                    else
                        color += texture(tex0, p+vec2(WIDTH/2,HEIGHT/2) + vec2(x,y));
                }
            }
        }else{
            float aa = mod(a - 0.5*3.14, tau/sides);
            aa = abs(aa - tau/sides/2.);
            p = r * vec2(cos(aa), sin(aa));
            if(scale>1){
                p *= scale;
                p.x = mod(p.x+HEIGHT/2,HEIGHT)-HEIGHT/2;
                p.y = mod(p.y+HEIGHT/2,HEIGHT)-HEIGHT/2;
            }
            if(copyBorder==1 || (abs(p.x+x)<WIDTH/2 && abs(p.y+y)<HEIGHT/2))
                color = texture(tex0, p+vec2(WIDTH/2,HEIGHT/2) + vec2(x,y));
        }
    }else{
        if(scale>1){
            p *= scale;
        }
        vec2 p2;
        if(sides==0){ //mirror
            if(copy==1){
                p2 = vec2(-p.x, p.y);
                if(mirror==1)
                    p2.y = -p2.y;
                if(scale>1){
                    p.x = mod(p.x+HEIGHT/2,HEIGHT)-HEIGHT/2;
                    p.y = mod(p.y+HEIGHT/2,HEIGHT)-HEIGHT/2;
                    p2.x = mod(p2.x+HEIGHT/2,HEIGHT)-HEIGHT/2;
                    p2.y = mod(p2.y+HEIGHT/2,HEIGHT)-HEIGHT/2;
                }
                color = (texture(tex0, p + vec2(x,y)+vec2(WIDTH/2,HEIGHT/2)) + texture(tex0, p2 + vec2(x,y) + vec2(WIDTH/2,HEIGHT/2)));
                if(copyAdjust==1)
                    color /= 2;
            }else{
                if(p.x >= 0){
                    p.x = - p.x;
                    if(mirror==1)
                        p.y = -p.y;
                }
                if(scale>1){
                    p.x = mod(p.x+HEIGHT/2,HEIGHT)-HEIGHT/2;
                    p.y = mod(p.y+HEIGHT/2,HEIGHT)-HEIGHT/2;
                    p2.x = mod(p2.x+HEIGHT/2,HEIGHT)-HEIGHT/2;
                    p2.y = mod(p2.y+HEIGHT/2,HEIGHT)-HEIGHT/2;
                }
                color = texture(tex0, p + vec2(x,y) + vec2(WIDTH/2,HEIGHT/2));
            }
            
        }else{
        if(sides==1){ //mirror
            if(copy==1){
                p2 = vec2(p.x, - p.y);
                if(mirror==1)
                    p2.x = -p2.x;
                if(scale>1){
                    p.x = mod(p.x+HEIGHT/2,HEIGHT)-HEIGHT/2;
                    p.y = mod(p.y+HEIGHT/2,HEIGHT)-HEIGHT/2;
                    p2.x = mod(p2.x+HEIGHT/2,HEIGHT)-HEIGHT/2;
                    p2.y = mod(p2.y+HEIGHT/2,HEIGHT)-HEIGHT/2;
                }
                color = texture(tex0, p + vec2(x,y) + vec2(WIDTH/2,HEIGHT/2)) + texture(tex0, p2 + vec2(x,y) + vec2(WIDTH/2,HEIGHT/2));
                if(copyAdjust==1)
                    color /= 2;
            }else{
                if(p.y >= 0){
                    p.y = -p.y;
                    if(mirror==1)
                        p.x = -p.x;
                }
                if(scale>1){
                    p.x = mod(p.x+HEIGHT/2,HEIGHT)-HEIGHT/2;
                    p.y = mod(p.y+HEIGHT/2,HEIGHT)-HEIGHT/2;
                    p2.x = mod(p2.x+HEIGHT/2,HEIGHT)-HEIGHT/2;
                    p2.y = mod(p2.y+HEIGHT/2,HEIGHT)-HEIGHT/2;
                }
                color = texture(tex0, p + vec2(x,y) + vec2(WIDTH/2,HEIGHT/2));
            }
        }
        }
    }
    
    outputColor = color;
}
