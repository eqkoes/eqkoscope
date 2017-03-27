#version 150
precision mediump float;

uniform sampler2DRect tex0;
uniform sampler2DRect skew;
out vec4 outputColor;
in vec2 texCoordVarying;
uniform float offset;
uniform float intensity=0;

void main(){
    vec2 p = texCoordVarying;
    vec4 c = texture(skew, vec2(mod(p.y+offset,768),0));
    p.x = p.x + (c.r-0.5)*intensity;
    vec4 color = texture(tex0, p);
    outputColor = color;
}

