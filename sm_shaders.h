#ifndef SM_SHADERS_H
#define SM_SHADERS_H
#include <stdio.h>//this should be in a c file, we shouldn't pollute the namespace like this
#include <stdlib.h>
#include <string.h>
#include "sm_textures.h"

static struct
{
	GLuint program;
	GLuint vertex, fragment, geometry;
	char name[101];
	struct
	{
		char name[101];
		GLuint texture;
	} textures[20];
	int numtextures;
	GLuint fbo;
	int numfboattachments;
	int fboresx, fboresy;
} shaderdata[50] = {0};//this is actually >101kb
static int numshaders = 0;
static int currentshader = 0;
static char includedata[10001] = {0};
static char *afterincludedata = NULL;
static int numinclstrings = 1;

static int smscreenw=1920, smscreenh=1080;

static int checkshader(GLuint id, char errorname[])
{
	int rval;
	glGetShaderiv(id, GL_COMPILE_STATUS, &rval);
	if(!rval)
	{
		char log[10000];
		glGetShaderInfoLog(id, 10000, NULL, log);
		printf("XXX:shader error in %s: %s", errorname, log);
	}
	return rval;
}

static void parsedata(char *source)
{
	includedata[0] = '\0';
	afterincludedata = NULL;
	numinclstrings = 1;

	char words[100][100];
	int numwords = 0;
	int wordlen = 0;

	char current;
	int index = 0;
	current = source[0];
	while(current != '\0')
	{
		if(current == ' ' && wordlen > 0)
		{
			words[numwords][wordlen] = '\0';
			wordlen = 0;
			numwords ++;
		}
		else if(current == '\n' || wordlen >= 99)
		{
			words[numwords][wordlen] = '\0';
			numwords ++;
			if(strcmp(words[0], "//@TEX")==0)
				if(numwords >=2)
				{
					printf("XXXXXXXXXXXXXXXXXload tex %s\n", words[1]);
					strncpy(shaderdata[numshaders].textures[shaderdata[numshaders].numtextures].name,words[1], 99);
					shaderdata[numshaders].textures[shaderdata[numshaders].numtextures].texture = smt_load(words[1]);//FIXME:this will cause trouble if filenames have a / in them
					shaderdata[numshaders].numtextures ++;
				}
				else
					printf("usage: //@TEX filename\n");
			else if(strcmp(words[0], "//@FBOUT")==0)
			{
				if(numwords >= 3)
				{
					if(strcmp(words[2], "SCREEN") ==0)
					{
						shaderdata[numshaders].fboresx=smscreenw;
						shaderdata[numshaders].fboresy=smscreenh;
					}
					else if(strcmp(words[2], "HALFSCREEN") ==0)
					{
						shaderdata[numshaders].fboresx=smscreenw/2;
						shaderdata[numshaders].fboresy=smscreenh/2;
					}
					else if(strcmp(words[2], "SMALL") ==0)
					{
						shaderdata[numshaders].fboresx=256;
						shaderdata[numshaders].fboresy=256;
					}
					else if(strcmp(words[2], "MEDIUM") ==0)
					{
						shaderdata[numshaders].fboresx=1024;
						shaderdata[numshaders].fboresy=1024;
					}
					else if(strcmp(words[2], "LARGE") ==0)
					{
						shaderdata[numshaders].fboresx=2048;
						shaderdata[numshaders].fboresy=2048;
					}
					else{printf("invalid size %s\n", words[2]);}

					GLint type = GL_RGBA;
					if(numwords >= 4 && strcmp(words[3], "FLOAT") == 0 )
						type = GL_RGBA32F;
					printf("setting up framebuffer output %s attachment %d\n", words[1], shaderdata[numshaders].numfboattachments);
					if(shaderdata[numshaders].numfboattachments == 0)//only one fbo per shader, but possibly multiple attachments
					{
						glGenFramebuffers(1,&shaderdata[numshaders].fbo);
					}
					glBindFramebuffer(GL_FRAMEBUFFER, shaderdata[numshaders].fbo);
					//smt_gen(words[3],shaderdata[numshaders].fboresx,shaderdata[numshaders].fboresy, GL_RGBA);
					smt_gen(words[1],shaderdata[numshaders].fboresx,shaderdata[numshaders].fboresy, type);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+shaderdata[numshaders].numfboattachments++, GL_TEXTURE_2D, smt_gettex(words[1]),0);
					GLenum fbstat = glCheckFramebufferStatus(GL_FRAMEBUFFER);
					if(fbstat != GL_FRAMEBUFFER_COMPLETE)
						printf("error:framebuffer not complete %x\n", fbstat);

				}
				else
					printf("usage: //@FBOUT name resolution [FLOAT], where resolution is one of SCREEN, HALFSCREEN, SMALL, MEDIUM, LARGE\n");

			}
			else if(strcmp(words[0], "//@FBDEP")==0)
			{//FIXME:clean this up, make it right
				if(numwords >= 2 && shaderdata[numshaders].fbo != 0)
				{
					glBindFramebuffer(GL_FRAMEBUFFER, shaderdata[numshaders].fbo);
					smt_gen(words[1],shaderdata[numshaders].fboresx,shaderdata[numshaders].fboresy, GL_DEPTH_COMPONENT24);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, smt_gettex(words[1]),0);
				}
				
			}
			else if(strcmp(words[0], "//@FEEDBACK")==0)
			{
				//printf("not implemented, should specify the names of variables to be used as output (glTransformFeedbackVaryings)");
				char * names[50];
				int worditer;
				for(worditer=1;worditer < numwords; worditer ++)
					names[worditer-1] = &words[worditer][0];
				glTransformFeedbackVaryings(shaderdata[numshaders].program, numwords -1, names, GL_INTERLEAVED_ATTRIBS);//XXX:does this have to go after compileShader?
			}
			else if(strcmp(words[0], "//@INCLUDE")==0 && numwords > 1)//FIXME:parse recursively
			{
				strcat(words[1], ".sl");
				printf("including %s\n", words[1]);
				FILE *incl = fopen(words[1], "r");
				if(incl == NULL)
					printf("WARNING:missing shader library %s", words[1]);
				else
				{
					int includedatalen = fread(includedata, 1, 10000, incl); 
					if(includedatalen >= 10000)
						printf("WARNING:shader library %s too long, truncating", words[1]);
					includedata[includedatalen] = '\0';
					afterincludedata = &source[index];
					source[index-1] = '\0';
					numinclstrings = 3;
				}
			}

			numwords = 0;
			wordlen = 0;
		//	printf("herp %s %d\n", words[0], numwords);
		}
		else
		{
			words[numwords][wordlen++] = current;
		}
		current = source[++index];
	}


}


int sm_load(char *name)//load a shader, return -1 on error
{

	printf("loading shader %s\n", name);
	char filename[500];
	strncpy(filename, name, 490);
	strncat(filename, ".vs", 10);
	FILE *shadersource = fopen(filename, "r");
	shaderdata[numshaders].program = glCreateProgram();//need to initialize this here for transform feedback
	if(shadersource == NULL)
	{
		printf("missing vertex shader %s\n", filename);
		return -1;
	}
	char filedata[20001];
	char *fd[3] = {filedata, includedata, NULL/*this not valid until after parsedata*/};//using &filedata compiles fine but causes segfault (compiler using registers or something?)
	int filelength = fread(filedata, 1, 20000, shadersource);
	filedata[filelength] = '\0';
	if(filelength >= 20000)
		printf("shader over 20000 bytes, truncating\n");
	parsedata(filedata);
	fd[2] = afterincludedata;
	shaderdata[numshaders].vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shaderdata[numshaders].vertex, numinclstrings, &fd[0], NULL);//TODO:add dynamic shit here
	glCompileShader(shaderdata[numshaders].vertex);
	checkshader(shaderdata[numshaders].vertex, "vertex");
	fclose(shadersource);//done with vertex

	

	strncpy(filename, name, 490);
	strncat(filename, ".fs", 10);
	shadersource = fopen(filename, "r");
	if(shadersource == NULL)
	{
		printf("missing fragment shader %s\n", filename);
		return -1;//this will leak memory, open all files at beginning?
	}
	filelength = fread(filedata, 1, 20000, shadersource);
	filedata[filelength] = '\0';
	if(filelength >= 20000)
		printf("shader over 20000 bytes, truncating\n");
	parsedata(filedata);
	fd[2] = afterincludedata;
	shaderdata[numshaders].fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shaderdata[numshaders].fragment, numinclstrings, &fd[0], NULL);//TODO:add dynamic shit here
	glCompileShader(shaderdata[numshaders].fragment);
	checkshader(shaderdata[numshaders].fragment, "fragment");
	fclose(shadersource);//done with fragment


	

	strncpy(filename, name, 490);
	strncat(filename, ".gs", 10);
	shadersource = fopen(filename, "r");
	if(shadersource == NULL)//FIXME:missing geometry shader doesn't work
	{
		printf("missing geometry shader %s\n", filename);
		//return;//this will leak memory, open all files at beginning?
		shaderdata[numshaders].geometry = -1;
	}
	else
	{
		filelength = fread(filedata, 1, 20000, shadersource);
		filedata[filelength] = '\0';
		if(filelength >= 20000)
			printf("shader over 20000 bytes, truncating\n");
		parsedata(filedata);
		fd[2] = afterincludedata;
		shaderdata[numshaders].geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(shaderdata[numshaders].geometry, numinclstrings, &fd[0], NULL);//TODO:add dynamic shit here
		glCompileShader(shaderdata[numshaders].geometry);
		checkshader(shaderdata[numshaders].geometry, "geometry");
		fclose(shadersource);//done with geometry


	}

	
	


	glAttachShader(shaderdata[numshaders].program, shaderdata[numshaders].vertex);
	glAttachShader(shaderdata[numshaders].program, shaderdata[numshaders].fragment);
	if(shaderdata[numshaders].geometry != -1)
		glAttachShader(shaderdata[numshaders].program, shaderdata[numshaders].geometry);
	glLinkProgram(shaderdata[numshaders].program);
	int linkerr;
	glGetProgramiv(shaderdata[numshaders].program, GL_LINK_STATUS, &linkerr);
	char errlog[1000];
	glGetProgramInfoLog(shaderdata[numshaders].program, 1000, NULL, errlog);
	if(!linkerr)
	{
		printf("XXX:failed to link shader\n%s", errlog);
		return -1;
	}
	printf("%s", errlog);


	//setup textures
	int i;
	printf("setting up %d textures for shader %d\n", shaderdata[numshaders].numtextures, numshaders);
	glUseProgram(shaderdata[numshaders].program);
	for(i=0;i<shaderdata[numshaders].numtextures;i++)
	{
		printf("setting uniform %s to %d\n", shaderdata[numshaders].textures[i].name, shaderdata[numshaders].textures[i].texture);
		glUniform1i(glGetUniformLocation(shaderdata[numshaders].program, shaderdata[numshaders].textures[i].name), shaderdata[numshaders].textures[i].texture);
	}


	strncpy(shaderdata[numshaders].name, name, 100);
	numshaders ++;
	return 0;
}
void sm_use(char *name)//sets the active program object to the one specified
{

	int i;
	for(i=0;i<numshaders;i++)
	{
		if(strcmp(shaderdata[i].name, name) ==0)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, shaderdata[i].fbo);
			glUseProgram(shaderdata[i].program);
			if(shaderdata[i].fbo ==0)
			{
				glViewport(0,0,smscreenw,smscreenh);
			//	GLenum target = GL_FRONT_LEFT;
			//	glDrawBuffers(1, target);//FUCK OFF AMD
			}
			else
			{
				glViewport(0,0,shaderdata[i].fboresx,shaderdata[i].fboresy);

				GLenum targets[50];
				int j;
				//targets[0]=GL_DEPTH_ATTACHMENT;
				for(j=0;j<10;j++)
					targets[j] = GL_COLOR_ATTACHMENT0+j;
				glDrawBuffers(shaderdata[i].numfboattachments, targets);
			}
			currentshader = i;
			return;
		}
	}
	printf("no such shader %s\n", name);
}

GLuint sm_uniloc(char *name)//get the location of a uniform in the active program
{
	return glGetUniformLocation(shaderdata[currentshader].program, name);
}

void sm_screensize(int w, int h)//set the screen size (used for texture allocations and viewports, you should probably call this during setup)
{
	smscreenw = w;
	smscreenh = h;
}

#endif
