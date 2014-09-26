#version 150
#extension GL_ARB_explicit_attrib_location : enable
layout(location=0) in float vletter;

out vec2 uv;
out vec2 pos;


#define LETTERSPERROW 16.0
#define LETTERSIZE 0.05

void main(void)
{
	int xpos = int(vletter+0.5)%int(LETTERSPERROW);
	int ypos = int(vletter+0.5)/int(LETTERSPERROW);

	uv = vec2(mod(vletter/LETTERSPERROW, 1.0), ceil(vletter/LETTERSPERROW)/LETTERSPERROW);
	uv = vec2(xpos,ypos)/LETTERSPERROW;
	uv.y = 1-uv.y;
//uv.y=0;
	pos = vec2(LETTERSIZE*gl_VertexID,0);
	pos.y = -(int(pos.x/1.8))*LETTERSIZE;
	pos.x = mod(pos.x, 1.8);
	pos += vec2(-0.95,0.95);
}
