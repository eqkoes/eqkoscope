#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;

uniform float WIDTH = 1280;
uniform float HEIGHT = 720;
uniform int iter = 60;
uniform float param1 = 0;
uniform float param2 = 0;

uniform float mode = 0;
uniform float scale = 1;


void main()
{
    float d1, d2;
    float i;
    vec2 c;
    vec2 z;
    
    float p1 = (param1-0.5)/scale;
//    float p1 = (param1/scale-0.5);
//    float p2 = (param2/scale-0.5);
    float p2 = (param2-0.5)/scale;

    float n = max(iter, min(int(iter + (scale-1)*(50)), 300));
    
    if(mode==0) //julia
    {
         c = vec2(0.5+p1,0.55);
        
        z.x = 3.0 * (texCoordVarying.x/WIDTH - 0.5)/scale;
        z.y = 2.0 * (texCoordVarying.y/HEIGHT - 0.5)/scale;
        
        for(i=0; i<n; i++) {
            float x = (z.x * z.x - z.y * z.y) + c.x;
            float y = (z.y * z.x + z.x * z.y) + c.y;
            
            if((x * x + y * y) > 4.0) break;
            z.x = x;
            z.y = y;
            z.x = x;
            z.x += p1;
            z.y = y;
            z.y += p2;
        }
    
        outputColor = vec4(1, 0, 0, (i == n) ? 0.0 : i/n);
//        d1 = (i == n) ? 0.0 : i/60.0;
    }
    else
    {
        if(mode==1){ //mandelbruh
        
        vec2 center = vec2(1.42,0);
        
        c.x = 1.3333 * (texCoordVarying.y/HEIGHT - 0.5) / scale - center.x;
        c.y = (texCoordVarying.x/WIDTH - 0.5) / scale - center.y;
            
        z = c;
        for(i=0; i<n; i++) {
            float x = (z.x * z.x - z.y * z.y) + c.x;
            float y = (z.y * z.x + z.x * z.y) + c.y;
            
            if((x * x + y * y) > 4.0) break;
            z.x = x;
            z.x += p1;
            z.y = y;
            z.y += p2;
        }
        
            outputColor = vec4(1, 0, 0, (i == n) ? 0.0 : i/n);
//        d2 = (i == n) ? 0.0 : i/n;

//        }else{
//            if(mode==2){
//                float r;
//                int n = 0;
//                vec3 z;
//                z.x = texCoordVarying.x/WIDTH;
//                z.y = texCoordVarying.y/HEIGHT;
//                z.xy = mod((z.xy),0.1) - vec2(0.5,0); // instance on xy-plane
//                outputColor = vec4(length(z)-0.3,0,0,1);
//            }
        }
    }
    
//    outputColor = vec4(d1-d2, d2+d1, 0, 1);

}