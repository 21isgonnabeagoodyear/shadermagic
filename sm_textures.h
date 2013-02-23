#ifndef SM_TEXTURES_H
#define SM_TEXTURES_H
#include <stdio.h>

static struct
{
	GLuint id;
	char name[101];
	GLuint textureslot;
} texinfo[50];
static int numtex=0;
static GLuint lasttextureunit = 0;

struct smt_sgi_rgb
{
	short magic;
	char storage;
	char bpc;
	unsigned short dimensions;
	unsigned short xsize;
	unsigned short ysize;
	unsigned short zsize;
	int pixmin;
	int pixmax;
	int dummy;
	char imagename[80];
	int colormap;
	char dummy2[404];
};


GLuint smt_load(char filename[])//load a texture into a texture unit, returns the id of the texture unit/slot (this is what you pass to the shaders)
{
	int i;
	for(i=0;i<numtex;i++)
	{
		if(strncmp(texinfo[i].name, filename, 100) ==0)
		{
			printf("warning:tried to load texture twice\n");
			return texinfo[i].textureslot;
		}
		else
			printf("%s is not the same\n", texinfo[i].name);
	}

	struct smt_sgi_rgb header;
	FILE *input = fopen(filename, "r");
	if(input == NULL)
	{
		printf("no such file %s\n", filename);
		return 0;
	}
	fread(&header,512, 1, input);

	//XXX:put rgb header into little endian
	header.magic = header.magic >>8 | header.magic <<8;
	header.xsize = header.xsize >>8 | header.xsize <<8;
	header.ysize = header.ysize >>8 | header.ysize <<8;
	header.zsize = header.zsize >>8 | header.zsize <<8;





	printf("rgb header width %d height %d magic %d sizeof %d\n", header.xsize, header.ysize, header.magic, sizeof(struct smt_sgi_rgb));
	if(header.zsize != 4 || header.storage ==1)
		printf("image should have 4 channels (rgba) with no rle compression\n");
	char *data = malloc(header.xsize*header.ysize*4);

	//data is not stored interleaved, so we need to manually interleave it
	//fread(data, header.xsize*header.ysize*4, 1, input);
	int j,k;
	for(i=0;i<4;i++)
		for(j=0;j<header.ysize;j++)
			for(k=0;k<header.xsize;k++)
				data[(k+j*header.xsize)*4+i] = fgetc(input);
	
	
/*
	char test[256][256][4];
	int j;
	for(i=0;i<256;i++)
		for(j=0;j<256;j++)
		{
			test[i][j][0] = i&j;
			test[i][j][1] = i&j;
			test[i][j][2] = i&j;
			test[i][j][3] = 255;
		}*/
	glGenTextures(1, &texinfo[numtex].id);
	glActiveTexture(GL_TEXTURE0+lasttextureunit);//bind the texture to a unit
	texinfo[numtex].textureslot = lasttextureunit++;
	glBindTexture(GL_TEXTURE_2D, texinfo[numtex].id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//these are required
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, header.xsize, header.ysize, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

printf("using slot %d for texture %s\n", texinfo[numtex].textureslot, filename);
	free(data);
	fclose(input);
	strncpy(texinfo[numtex].name, filename, 99);
//	numtex ++;
	return texinfo[numtex++].textureslot;
}
GLuint smt_gen(char name[], int w, int h, GLenum type)//creates a new, blank texture 
{
	int i;
	for(i=0;i<numtex;i++)
	{
		if(strncmp(texinfo[i].name, name, 100) ==0)
		{
			printf("warning:tried to gen texture twice\n");
			return texinfo[i].textureslot;
		}
	}
	printf("generating texture %s\n", name);

	glGenTextures(1, &texinfo[numtex].id);
	glActiveTexture(GL_TEXTURE0+lasttextureunit);//bind the texture to a unit
	texinfo[numtex].textureslot = lasttextureunit++;
	glBindTexture(GL_TEXTURE_2D, texinfo[numtex].id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//these are required
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if(type == GL_DEPTH_COMPONENT24)
		glTexImage2D(GL_TEXTURE_2D, 0, type, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, type, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	strncpy(texinfo[numtex].name, name, 99);
	numtex ++;
	return texinfo[numtex].textureslot;
}
GLuint smt_gettex(char name[])//gets the id of the texture buffer (not its texture unit)
{
	int i;
	for(i=0;i<numtex;i++)
	{
		if(strncmp(texinfo[i].name, name, 100) ==0)
		{
			return texinfo[i].id;
		}
	}


}


#endif
