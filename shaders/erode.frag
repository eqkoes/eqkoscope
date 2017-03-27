#version 150

//different kernels for different speeds : 0: classic 9 points, down to 3 pts
uniform vec2 kern1[9] = vec2[9](
    vec2(-1,-1),
    vec2(0,-1),
    vec2(1,-1),
    vec2(-1,0),
    vec2(0,0),
    vec2(1,0),
    vec2(-1,1),
    vec2(0,1),
    vec2(1,1) );
uniform vec2 kern2[5] = vec2[5](
                                vec2(-1,-1),
                                vec2(1,-1),
                                vec2(0,0),
                                vec2(-1,1),
                                vec2(1,1) );
uniform vec2 kern3[3] = vec2[3](
                                vec2(-1,0),
                                vec2(-1,1),
                                vec2(1,1) );
uniform sampler2DRect tex0;
uniform int fast = 1;
in vec2 texCoordVarying;
out vec4 outputColor;

void main(){
        vec4 sample;
        vec4 minValue = vec4(1.0);
    
    switch(fast){
        case 0:{
            for (int i = 0; i < 9; i++){
                sample = texture(tex0, texCoordVarying + kern1[i]);
                minValue = min(sample, minValue);
            }
        }break;
        case 1:{
            for (int i = 0; i < 5; i++){
                sample = texture(tex0, texCoordVarying + kern2[i]);
                minValue = min(sample, minValue);
            }
        }break;
        case 2:{
            for (int i = 0; i < 3; i++){
                sample = texture(tex0, texCoordVarying + kern3[i]);
                minValue = min(sample, minValue);
            }
        }break;
    }
        
    outputColor = minValue;
}