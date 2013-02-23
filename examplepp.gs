#version 150
#extension GL_ARB_explicit_attrib_location : enable
layout(points) in;
layout(triangle_strip, max_vertices=4) out;

out vec2 sspos;

void main()
{
	gl_Position = vec4(-1, -1,0,1);
	sspos = gl_Position.xy;
	EmitVertex();
	gl_Position = vec4(-1,  1,0,1);
	sspos = gl_Position.xy;
	EmitVertex();
	gl_Position = vec4( 1, -1,0,1);
	sspos = gl_Position.xy;
	EmitVertex();
	gl_Position = vec4( 1,  1,0,1);
	sspos = gl_Position.xy;
	EmitVertex();

}
