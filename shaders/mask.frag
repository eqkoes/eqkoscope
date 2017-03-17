#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform float thresh = 0.1;

void main()
{
    vec4 c =texture(tex0, texCoordVarying);
    float mean = (c.r + c.b + c.g) / 3.0 * 3.0;
    
    //    if(mean>thresh)
    if(mean > thresh){
        outputColor = vec4(1,0,0,0);
    }else{
        //        outputColor = vec4(c.r,c.g,c.b,c.a*(mean-thresh)/(thresh));
        outputColor = vec4(c.r,c.g,c.b,c.a);
    }
}