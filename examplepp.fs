#version 150
#extension GL_ARB_explicit_attrib_location : enable

/*
=========NOTE=========
The implementations of bloom and ssao below are slow and low quality, and are intended for demonstration only.  To do these effects properly you'll want to use a two pass blur filter, probably using lower resolution textures.

*/
out vec4 color;
in vec2 sspos;
uniform sampler2D unprocessedscreen;
uniform sampler2D unprocessedscreendepth;
//@TEX unprocessedscreen
//@TEX unprocessedscreendepth

#define NUMSAMPLES 30
void main(void)
{
	color = vec4(sspos, 1,0.5);
	color = texture(unprocessedscreen, sspos*0.5+vec2(0.5,0.5));
	float depth = texture(unprocessedscreendepth, sspos*0.5+vec2(0.5,0.5));
	for(int i=0;i<NUMSAMPLES;i++)
	{
		vec2 samplepos =  sspos*0.5+vec2(0.5,0.5) + i*0.05/NUMSAMPLES*vec2(sin(float(i)), cos(float(i)));
		color +=(1.0/NUMSAMPLES)*0.5*texture(unprocessedscreen, samplepos);//BLOOM
	}
	float ssaoaccum = 0;
	for(int i=0;i<NUMSAMPLES;i++)
	{
		vec2 samplepos =  sspos*0.5+vec2(0.5,0.5) + i*0.02/NUMSAMPLES*vec2(sin(float(i)), cos(float(i)));
		ssaoaccum += vec3(clamp(1000*(texture(unprocessedscreendepth, samplepos).r-depth), -1.5*(1.0/NUMSAMPLES), 0));//SSAO
	}
	color.rgb *= 1 + ssaoaccum;///(depth+1)*2;
color.a=1;
}
