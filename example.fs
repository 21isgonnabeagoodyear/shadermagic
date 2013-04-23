#version 150
#extension GL_ARB_explicit_attrib_location : enable
//@FBOUT unprocessedscreen SCREEN
//@FBDEP unprocessedscreendepth
out vec4 color;
uniform mat4 modelviewproj;
in vec3 col;
void main(void)
{
	color = vec4(col,1);
//	color = vec4(1,1,1,1)*(int(gl_FragCoord.x) & int(gl_FragCoord.y));
}
