#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform float brightness = 1;
uniform float contrast = 1;

void main()
{
    vec4 col = texture(tex0, texCoordVarying);
    
    vec3 bright = col.rbg + vec3(brightness,brightness,brightness);
    col = vec4(bright[0],bright[1],bright[2], col.a);
    
    vec3 contrasted = (col.rbg - 0.5) * contrast + 0.5;
    col = vec4(contrasted[0], contrasted[1], contrasted[2], col.a);
    
    outputColor = col;
}

