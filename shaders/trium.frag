#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform float WIDTH;

void main()
{
    int slice = int((3*texCoordVarying.x) / (WIDTH));
    
    vec2 coords = texCoordVarying;
    coords.x -= (slice-1)*(WIDTH)/3;
    
    outputColor = texture(tex0, coords);
}

