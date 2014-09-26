#ifndef SM_MESH_H
#define SM_MESH_H

static FILE *current=NULL;
static unsigned int numverts=0;
static unsigned int numtris=0;
static unsigned int vertelem=0;
static const int HEADERSIZE=12;

int smm_open(char filename[])//open a .tmd file
{
	if(current != NULL)
		fclose(current);
	current = fopen(filename, "r");
	if(current != NULL)
	{
		fread(&numverts, sizeof(unsigned int), 1, current);
		fread(&numtris, sizeof(unsigned int), 1, current);
		fread(&vertelem, sizeof(unsigned int), 1, current);
		printf("%d verts %d tris %d floats per vertex\n", numverts, numtris, vertelem);
		return 0;
	}
	printf("failed to open %s\n", filename);
	numverts =0;
	return 1;
}
unsigned int smm_numverts()
{
	return numverts;
}
unsigned int smm_vertsize()//size of each vertex in bytes
{
	return vertelem*sizeof(float);
}
void smm_fetchverts(void *dest)//read all the vertices to dest
{
	fseek(current, HEADERSIZE, SEEK_SET);
	fread(dest, numverts*smm_vertsize(), 1, current);
}

unsigned int smm_numtris()
{
	return numtris;
}
unsigned int smm_trisize()
{
	return sizeof(short)*3;
}
void smm_fetchtris(void *dest)//copy the indices into dest
{
	fseek(current, HEADERSIZE+numverts*smm_vertsize(), SEEK_SET);
	fread(dest, numtris*smm_trisize(), 1, current);
}

GLuint smm_upload()//upload the indices and vertices to the gl, using vec3s until out, returns a vao
{
	float *vertexdata = malloc(smm_vertsize()*smm_numverts());
	float *indexdata = malloc(smm_trisize()*smm_numtris());
	smm_fetchverts(vertexdata);
	smm_fetchtris(indexdata);

	GLuint vao, vbo;
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);
printf("bind vao %d", vao);
	glGenBuffers(1,&vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, /*sizeof(vertexdata)*/smm_numverts()*smm_vertsize(), vertexdata, GL_STATIC_DRAW);

	int i;
	for(i=0;i<smm_vertsize()/(3*sizeof(float));i++)
	{
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, smm_vertsize(), i*3*sizeof(float));//associates first attribute with vertexdata, also sets source to vbo
printf("setting %d to %d\n", i, i*3*sizeof(float));
	}
	if(smm_vertsize()/(sizeof(float)) % 3)
	{
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, smm_vertsize()/sizeof(float) % 3, GL_FLOAT, GL_FALSE, smm_vertsize(), i*3*sizeof(float));//use last floats as a 1 or 2 element vector
printf("setting %d to %d only %d\n", i, i*3*sizeof(float), smm_vertsize()/sizeof(float) % 3);
	}


	GLuint ibo;
	glGenBuffers(1,&ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);//this is bound to the vao
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, /*sizeof(indexdata)*/smm_numtris()*smm_trisize(), indexdata, GL_STATIC_DRAW);

/*	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(65535);*/
	return vao;
}


#endif
