#version 150

uniform sampler2DRect tex0;
uniform int xresolution = 10;
uniform int yresolution = 10;
uniform int xfocus = -1;
uniform int yfocus = -1;
uniform int WIDTH = 1;
uniform int HEIGHT = 1;
in vec2 texCoordVarying;
out vec4 outputColor;

void main()
{
    
    vec2 coord = texCoordVarying - vec2(WIDTH/2, HEIGHT/2);
    
    if(xresolution>1){
        if(xfocus==-1)
            coord.x = int(coord.x/xresolution)*xresolution + xresolution/(WIDTH*2);
        else{
            float xres = xresolution * abs(coord.x - xfocus) * 0.1;;
            coord.x = int(coord.x/xres)*xres + xresolution/(WIDTH*2);
        }
    }
    if(yresolution>1){
        if(yfocus==-1)
            coord.y = int(coord.y/yresolution)*yresolution + yresolution/(HEIGHT*2);
        else{
            float yres = yresolution * abs(coord.y - yfocus) * 0.1;
            coord.y = int(coord.y/yres)*yres + yresolution/(HEIGHT*2);
        }
    }
    
    coord = coord + vec2(WIDTH/2, HEIGHT/2);
    outputColor = texture(tex0, coord);
}