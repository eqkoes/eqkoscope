#version 150

#define WIDTH 1024
#define HEIGHT 768

// these are for the programmable pipeline system
uniform mat4 modelViewProjectionMatrix;
uniform mat4 textureMatrix;
in vec4 position;
in vec2 texcoord;
in vec4 normal;
in vec4 color;
out vec2 texCoordVarying;

float distance(float x1, float y1, float x2, float y2){
    return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

void main()
{
#ifdef INTEL_CARD
    color = vec4(1.0); // for intel HD cards
    normal = vec4(1.0); // for intel HD cards
#endif
    texCoordVarying = texcoord;
    
    vec4 pos = position;
//    if(texcoord.x < WIDTH/2)
//        pos.z = texcoord.x;
//    else
//        pos.z = WIDTH - texcoord.x;
    //else
      //  pos.z = -(1 - texCoord.x);
    
//    pos.z = pos.x*pos.y*0.1;
    pos.z = 100;
    pos.z = (texcoord.x-0.5);
    gl_Position = modelViewProjectionMatrix * pos;
}