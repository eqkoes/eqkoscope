#version 150

uniform sampler2DRect tex0;
uniform float threshold=0.5;
uniform float saturation=0.5;
uniform float alpha = 1;
uniform float levels = 1;
in vec2 texCoordVarying;
out vec4 outputColor;

void main()
{
    vec4 s = texture(tex0, texCoordVarying);
    
    float r = texture(tex0, texCoordVarying).r > threshold ? 0 : 1;
    float g = texture(tex0, texCoordVarying).g > threshold ? 0 : 1;
    float b = texture(tex0, texCoordVarying).b > threshold ? 0 : 1;
    
//    float r = 1 - int(levels*s.r)/(levels) + (mod(s.r, levels)<threshold/levels ? 0 : 1/levels);
//    float g = 1 - int(levels*s.g)/(levels) + (mod(s.g, levels)<threshold/levels ? 0 : 1/levels);
//    float b = 1 - int(levels*s.b)/(levels) + (mod(s.b, levels)<threshold/levels ? 0 : 1/levels);
    
    float P = sqrt(r*r*0.299+g*g*0.587+b*b*0.114) ;
    r=P+(r-P)*saturation;
    g=P+(g-P)*saturation;
    b=P+(b-P)*saturation;
    
    outputColor = vec4(r,g,b,alpha);
}