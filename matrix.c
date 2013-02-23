

#ifndef MATRIX_C
#define MATRIX_C

#include <math.h>
#include <string.h>
#include <stdio.h>

void mat_identity(float *in)
{
	const float identity[] =  {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
	memcpy(in, identity, sizeof(float)*16);
}
void mat_mul(float *in, float *tomulby)//this *= in
{
	int i,j,k;
	float backup[4][4];
	memcpy(backup, in, sizeof(float)*16);
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			in[i*4+j] = 0;
			for(k=0;k<4;k++)
			{
				in[i*4+j] += backup[i][k]*tomulby[k*4 + j];
			}
		}
	}
}

void mat_mulvec(float *mat, float x, float y, float z, float w, float *result)
{
	float *d = mat;
	result[0] = d[0]*x + d[1]*y + d[2]*z + d[3]*w;
	result[1] = d[4]*x + d[5]*y + d[6]*z + d[7]*w;
	result[2] = d[8]*x + d[9]*y + d[10]*z+ d[11]*w;
	result[3] = d[12]*x+ d[13]*y+ d[14]*z+ d[15]*w;
}

void mat_rot(float *mat, float x, float y, float z, float amount)
{
	float camt = cos(amount);
	float samt = sin(amount);
	float multmat[16];//http://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
	multmat[0] = camt + x*x*(1-camt);
	multmat[1] = x*y*(1-camt)-z*samt;
	multmat[2] = x*z*(1-camt)+y*samt;
	multmat[3] = 0;

	multmat[4] = y*x*(1-camt) + z*samt;
	multmat[5] = camt + y*y*(1-camt);
	multmat[6] = y*z*(1-camt) -x*samt;
	multmat[7] = 0;

	multmat[8] = z*x*(1-camt)-y*samt;
	multmat[9] = z*y*(1-camt)+x*samt;
	multmat[10]= camt + z*z*(1-camt);
	multmat[11]= 0;

	multmat[12]=0;
	multmat[13]=0;
	multmat[14]=0;
	multmat[15]=1;

	mat_mul(mat, multmat);
}
void mat_scale(float *mat, float x, float y, float z)
{
	float matr[] = {0,0,0,0,   0,0,0,0,   0,0,0,0,   0,0,0,1};
	matr[0] = x;
	matr[5] = y;
	matr[10]= z;
	mat_mul(mat, matr);
}
void mat_trans(float *mat, float x, float y, float z)
{
	float matr[] = {1,0,0,0  ,0,1,0,0  ,0,0,1,0  ,0,0,0,1};
	matr[3] = x;
	matr[7] = y;
	matr[11]= z;
	mat_mul(mat, matr);
}
void mat_proj(float *mat, float d)//mingw doesn't like "near"
{
	float matr[] = {1,0,0,0  ,0,1,0,0  ,0,0,1,0  ,0,0,0,1};
	matr[11] = 1 / d;
	matr[14] = - d;

	mat_mul(mat, matr);
}
void mat_proj2(float *mat, float vfov, float aspect, float far, float near)//mingw doesn't like "near"
{
/*
0 1 2 3
4 5 6 7
8 9 1011
12131415
*/
	float matr[] = {1,0,0,0  ,0,1,0,0  ,0,0,1,0  ,0,0,0,1};
	matr[0] = (1/tan(vfov/2))/aspect;
	matr[5] = 1/tan(vfov/2);
	matr[10] = (far+near)/(far-near);
	matr[11] = -2*(far*near)/(far-near);
	matr[14] = 1;
/*
	matr[0] = (1/tan(vfov/2))/aspect;
	matr[5] = 1/tan(vfov/2);
	matr[10] = (far+near)/(far-near);
	matr[14] = -2*(far*near)/(far-near);
	matr[11] = 1;
*/
	mat_mul(mat, matr);
}
void mat_print(float *mat)
{
	int i, j;
	for(i=0;i<4;i ++)
	{
		for(j=0;j<4;j++)
			printf("%f ",mat[i*4+j]);
		printf("\n");
	}
}

#endif
