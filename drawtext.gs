#version 150
#extension GL_ARB_explicit_attrib_location : enable
layout(points) in;
layout(triangle_strip, max_vertices=4) out;

in vec2 uv[];
in vec2 pos[];
out vec2 texuv;

#define LETTERSPERROW 16.0
#define LETTERSIZE 0.05

void main()
{
	gl_Position = vec4(pos[0], 0,1);
	texuv = uv[0] + vec2(0,-1/LETTERSPERROW);
	//texuv = uv[0];
	EmitVertex();
	gl_Position = vec4(pos[0] + vec2(0,LETTERSIZE), 0,1);
	texuv = uv[0];
	//texuv = uv[0] + vec2(0,1/LETTERSPERROW);
	EmitVertex();
	gl_Position = vec4(pos[0] + vec2(LETTERSIZE,0), 0,1);
	texuv = uv[0] + vec2(1/LETTERSPERROW,-1/LETTERSPERROW);
	//texuv = uv[0] + vec2(1/LETTERSPERROW,0);
	EmitVertex();
	gl_Position = vec4(pos[0] + vec2(LETTERSIZE,LETTERSIZE), 0,1);
	texuv = uv[0] + vec2(1/LETTERSPERROW,0);
	//texuv = uv[0] + vec2(1/LETTERSPERROW,1/LETTERSPERROW);
	EmitVertex();

}
