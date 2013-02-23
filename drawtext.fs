#version 150
#extension GL_ARB_explicit_attrib_location : enable
in vec2 texuv;
layout(location=0) out vec4 outputcolor;
//@TEX letters
uniform sampler2D letters;
void main(void)
{
	outputcolor =texture(letters, texuv); 
//	if(outputcolor.a < 0.5)
//		discard;
//outputcolor = texture(letters, gl_FragCoord.xy/1000.0);
}
