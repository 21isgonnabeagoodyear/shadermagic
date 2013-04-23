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


float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

#define NUMSAMPLES 30

vec2 complexsquare(vec2 inc)
{
	return vec2(inc.x*inc.x-inc.y*inc.y, inc.x*inc.y+inc.y*inc.x);
}

void main(void)
{
	//float freakyblend = pow(clamp(length(sspos), 0,1),8);
	//vec2 sspos = (1-freakyblend)*sspos+(freakyblend)*vec2(sspos.x+sin(sspos.y*10)*0.1, sspos.y+sin(sspos.x*10)*0.1);
	//sspos /= 1.2;
	//vec2 sspos = sspos / (1+clamp(length(sspos)-1,0,1));
//	vec2 sspos = sspos -0.5*(pow(clamp(length(sspos)-1,0,5),8))*(sspos);
//	sspos *= 2;
//	vec2 sspos = normalize(sspos)*pow(length(sspos),2);

	color = texture(unprocessedscreen, sspos*0.5+vec2(0.5,0.5));
	float depth = texture(unprocessedscreendepth, sspos*0.5+vec2(0.5,0.5));
	float ssaoaccum = 0;
	for(int i=0;i<NUMSAMPLES;i++)
	{
		vec2 samplepos =  sspos*0.5+vec2(0.5,0.5) + i*0.02/NUMSAMPLES*vec2(sin(float(i)), cos(float(i)));
		ssaoaccum += 3*vec3(clamp(1000*(texture(unprocessedscreendepth, samplepos).r-depth), -1.5*(1.0/NUMSAMPLES), (1.0/NUMSAMPLES)));//SSAO
	}
	color.rgb *= 1 + ssaoaccum;///(depth+1)*2;
	for(int i=0;i<NUMSAMPLES;i++)
	{
		vec2 samplepos =  sspos*0.5+vec2(0.5,0.5) + i*0.05/NUMSAMPLES*vec2(sin(float(i)), cos(float(i)));
		//vec2 samplepos =  sspos*0.5+vec2(0.5,0.5) + i*0.05/NUMSAMPLES*vec2(sin(rand(i*sspos)*100), cos(rand(i*sspos)*100));
		color +=(1.0/NUMSAMPLES)*0.5*texture(unprocessedscreen, samplepos);//BLOOM
	}
//color.rg = sspos-ssspos;
color.a=1;







	vec2 temp = sspos*2;
	for(int i=0;i<20;i++)
	{
		temp = complexsquare(temp)+sspos*2;
		if(length(temp) > 2)
		{
			color = vec4(0,0,1,1)*(1.0/i);
			return;
		}
	}
	//color = vec4(1,0,0,1);


	































}
