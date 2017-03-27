#version 150

uniform sampler2DRect tex0;
uniform float threshold=0.5;
uniform float saturation=0.5;
uniform float brightness=1;
uniform float alpha = 1;
uniform float levels = 1;
uniform float invert = 0;
uniform float drywet = 1;
in vec2 texCoordVarying;
out vec4 outputColor;

vec3 rgb2hsv(vec4 c){
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec4 hsv2rgb(vec3 c){
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return vec4(c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y), 1);
}

void main()
{
    vec4 s = texture(tex0, texCoordVarying);
    
    bool i = invert > 0;
    float r = s.r > threshold ? (i?0:1) : (i?1:0);
    float g = s.g > threshold ? (i?0:1) : (i?1:0);
    float b = s.b > threshold ? (i?0:1) : (i?1:0);
    
//    float r = int(levels*s.r)/(levels) + (mod(s.r, levels)<threshold/levels ? 0 : 1/levels);
//    float g = int(levels*s.g)/(levels) + (mod(s.g, levels)<threshold/levels ? 0 : 1/levels);
//    float b = int(levels*s.b)/(levels) + (mod(s.b, levels)<threshold/levels ? 0 : 1/levels);
    
    r = drywet*r + s.r*(1-drywet);
    g = drywet*g + s.g*(1-drywet);
    b = drywet*b + s.b*(1-drywet);
    
    if(saturation>0){
        vec3 hsv = rgb2hsv(vec4(r,g,b,1));
        hsv.y = min(hsv.y, saturation);
        hsv.z *= brightness;
        outputColor = hsv2rgb(hsv);
    }else{
    	outputColor = vec4(r, g, b, 1);
    }
}