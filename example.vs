#version 150
#extension GL_ARB_explicit_attrib_location : enable
layout(location=0) in vec3 position;
layout(location=1) in vec3 vertexcolor;
layout(location=2) in vec3 offset;//used for transform feedback

out vec3 col;


uniform mat4 modelviewproj;

void main(void)
{
	col = vertexcolor;
	gl_Position = modelviewproj*vec4(position+offset,1);
}
