#version 150


uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;

uniform int resolution = 25;
uniform float blurDistance = 10;
uniform float originalMix = 1;
uniform float vertical = 0;
uniform float intensity = 1;
uniform float totalMix = 1;
uniform float brightness = 1;


void main()
{
    vec4 c = originalMix * texture(tex0, texCoordVarying);
    vec4 bColor = vec4(0,0,0,1);
    int denom = 0;
    
    for(int i=1;i<resolution;i++){
        if(vertical!=0){
            bColor += i * texture(tex0, texCoordVarying + vec2(0.0, blurDistance * (resolution-1-i)));
            bColor += i * texture(tex0, texCoordVarying + vec2(0.0, blurDistance * -(resolution-1-i)));
        }else{
            bColor += i * texture(tex0, texCoordVarying + vec2(blurDistance * (resolution-1-i), 0.0));
            bColor += i * texture(tex0, texCoordVarying + vec2(blurDistance * -(resolution-1-i), 0.0));
        }
        denom += 2*i;
    }
    
     if(vertical!=0)
         bColor += resolution * texture(tex0, texCoordVarying + vec2(0.0, blurDistance));
     else
         bColor += resolution * texture(tex0, texCoordVarying + vec2(blurDistance, 0.0));
    
    bColor *= vec4(brightness,brightness,brightness,1);
    
    denom += resolution;

    bColor /= (denom + originalMix);
    outputColor = bColor*vec4(1,1,1,intensity)*totalMix + (1-totalMix)*c;
}