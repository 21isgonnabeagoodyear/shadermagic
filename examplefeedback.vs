#version 150
#extension GL_ARB_explicit_attrib_location : enable
out vec3 voffset;

//@FEEDBACK voffset


void main(void)
{
	voffset =vec3(cos(gl_VertexID*0.1)*gl_VertexID*0.1,0,sin(gl_VertexID*0.1)*gl_VertexID*0.1);// 30*vec3(mod(gl_VertexID, 10), sin(gl_VertexID*0.1), mod(gl_VertexID, 100));
}
