#version 150
#extension GL_ARB_explicit_attrib_location : enable
out vec3 voffset;

//@FEEDBACK voffset

uniform float wavetime;


void main(void)
{
	voffset =vec3(cos(pow(gl_VertexID,0.25)*10)*gl_VertexID*0.1,/*-clamp(abs(wavetime-gl_VertexID),0,5)*/sin(wavetime*0.2-gl_VertexID*0.5)-5,sin(pow(gl_VertexID,0.25)*10)*gl_VertexID*0.1);// 30*vec3(mod(gl_VertexID, 10), sin(gl_VertexID*0.1), mod(gl_VertexID, 100));
}
