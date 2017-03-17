#version 150
// these are for the programmable pipeline system
uniform mat4 modelViewProjectionMatrix;
uniform mat4 textureMatrix;
in vec4 position;
in vec2 texcoord;
in vec4 normal;
in vec4 color;
out vec2 texCoordVarying;
void main()
{
#ifdef INTEL_CARD
    color = vec4(1.0); // for intel HD cards
    normal = vec4(1.0); // for intel HD cards
#endif
    texCoordVarying = texcoord;
    gl_Position = modelViewProjectionMatrix * position;
}

varying vec4 posPos;
uniform float FXAA_SUBPIX_SHIFT = 1.0/4.0;
uniform float rt_w; // GeeXLab built-in
uniform float rt_h; // GeeXLab built-in

void main(void)
{
    gl_Position = ftransform();
    gl_TexCoord[0] = gl_MultiTexCoord0;
    vec2 rcpFrame = vec2(1.0/rt_w, 1.0/rt_h);
    posPos.xy = gl_MultiTexCoord0.xy;
    posPos.zw = gl_MultiTexCoord0.xy -
    (rcpFrame * (0.5 + FXAA_SUBPIX_SHIFT));
}
