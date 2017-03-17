#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform float gamma = 2.2;
uniform float contrast = 0;

void main()
{
        vec4 col0 = texture(tex0, texCoordVarying);
    
        // Tone mapping (simple photographic exposure)
    vec4 finalCol = col0;
    
    finalCol.r = finalCol.r < 0.5 ? finalCol.r-contrast : finalCol.r+contrast;
    finalCol.g = finalCol.g < 0.5 ? finalCol.g-contrast : finalCol.g+contrast;
    finalCol.b = finalCol.b < 0.5 ? finalCol.b-contrast : finalCol.b+contrast;
    
    finalCol.r = pow(finalCol.r, gamma);
    finalCol.g = pow(finalCol.g, gamma);
    finalCol.b = pow(finalCol.b, gamma);
    
        outputColor = finalCol;
    }