#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform int fast = 0;
uniform float mix_ = 0;
uniform float intensity = 1;
uniform vec2 kern1[9] = vec2[9](vec2(-1,-1), //0
                                vec2(0,-1), //1
                                vec2(1,-1), //2
                                vec2(-1,0), //3
                                vec2(0,0), //4
                                vec2(1,0), //5
                                vec2(-1,1), //6
                                vec2(0,1), //7
                                vec2(1,1)); //8

uniform vec2 kern2[5] = vec2[5](vec2(-1,-1),
                                vec2(1,-1),
                                vec2(0,0),
                                vec2(-1,1),
                                vec2(1,1));
uniform vec2 kern3[3] = vec2[3](vec2(-1,0),
                                vec2(-1,1),
                                vec2(1,1));

uniform int hueLevels = 0;
uniform int lumLevels = 0;

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

vec4 hsv2rgb2(vec3 c){
    return vec4(0.5*(c.z + c.y*cos(c.x)), 0.5*(c.z + c.y*cos(c.y)), 0.5*(c.z + c.y*cos(c.z)), 1);
}

void main(void)
{
    vec4 sample[9];
    
    switch(fast){
        case 0:
            for (int i = 0; i < 9; i++)
                sample[i] = texture(tex0, texCoordVarying + kern1[i]);
        break;
        case 1:
            for (int i = 0; i < 5; i++)
                sample[i] = texture(tex0, texCoordVarying + kern2[i]);
        break;
        case 2:
            for (int i = 0; i < 3; i++)
                sample[i] = texture(tex0, texCoordVarying + kern3[i]);
        break;
    }
    
    vec4 horizEdge;
    vec4 vertEdge;
    switch(fast){
        case 0:
                horizEdge = sample[2] + (2.0*sample[5]) + sample[8] -
                (sample[0] + (2.0*sample[3]) + sample[6]);
            
             vertEdge = sample[0] + (2.0*sample[1]) + sample[2] -
                (sample[6] + (2.0*sample[7]) + sample[8]);
            break;
        case 1:
             horizEdge = sample[1] + sample[4] -
            (sample[0] + sample[3]);
            
             vertEdge = sample[0] + sample[1] -
            (sample[3] + sample[4]);
            break;
    }
    
    outputColor.rgb = sqrt((horizEdge.rgb * horizEdge.rgb) +
                            (vertEdge.rgb * vertEdge.rgb));
    
    //foulon's enhancement
    vec3 orig = rgb2hsv(texture(tex0, texCoordVarying));
    vec3 dest = rgb2hsv(outputColor);
    
    if(hueLevels>0)
        orig[0] = int(orig[0]*hueLevels) / float(hueLevels);
    if(lumLevels>0)
        orig[2] = int(orig[2]*lumLevels) / float(lumLevels);
    
    outputColor = hsv2rgb(vec3(orig[0], orig[1], dest[2]))*intensity;
    
    if(intensity<1)
        outputColor += texture(tex0, texCoordVarying)*(1-intensity);
    
    if(mix_>0){
        if(hueLevels>0 || lumLevels>0){
            outputColor += hsv2rgb(vec3(orig[0], orig[1], orig[2]))*mix_;
        }else{
            outputColor += texture(tex0, texCoordVarying)*mix_;
        }
    }

    outputColor.a = 1.0;
}

