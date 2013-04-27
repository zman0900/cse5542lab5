//**************************************************************************************
//  Copyright (C) 2002 - 2011, Huamin Wang
//  All rights reserved.
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//     1. Redistributions of source code must retain the above copyright
//        notice, this list of conditions and the following disclaimer.
//     2. Redistributions in binary form must reproduce the above copyright
//        notice, this list of conditions and the following disclaimer in the
//        documentation and/or other materials provided with the distribution.
//     3. The names of its contributors may not be used to endorse or promote
//        products derived from this software without specific prior written
//        permission.
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//	NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//**************************************************************************************
// MESH.h
//**************************************************************************************
#include "MESH.h"

namespace lab5 {

float Normalize(float *x, float *r)
{
	if(r==0) r=x;
	float m=sqrtf(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]);
	if(m<1e-14f) return m;
	float inv_m=1/m;
	r[0]=x[0]*inv_m;
	r[1]=x[1]*inv_m;
	r[2]=x[2]*inv_m;
	return m;
}

void Cross(float x[], float y[], float r[])
{
	r[0]= x[1]*y[2]-y[1]*x[2];
	r[1]=-x[0]*y[2]+y[0]*x[2];
	r[2]= x[0]*y[1]-y[0]*x[1];
}


MESH::MESH()
{
	max_number	= 65536;
	// vase
	//max_verts = 8896;
	//max_tris = 17784;
	// doberman
	//max_verts = 11313;
	//max_tris = 18280;
	// bunny
	max_verts = 29416;
	max_tris = 58828;
	number		= 0;
	t_number	= 0;
	X			= new VERTEX	[max_verts];		
	T			= new GLuint	[max_tris*3];
}

MESH::~MESH()
{
	if(X)	delete []X;
	if(T)	delete []T;
}

void MESH::Read_OBJ_File(const char *filename)
{
	number=0;
	t_number=0;
	int vertex_normal_number=0;
	FILE *fp=fopen(filename, "r+");		
	while(feof(fp)==0)
	{
		char token[1024];
		fscanf(fp, "%s", &token);
		if(token[0]=='#' && token[1]=='\0')
		{
			int c;
			while(feof(fp)==0)
				if((c=fgetc(fp))=='\r' || c=='\n')	break;
		}
		else if(token[0]=='v' && token[1]=='\0')	//vertex
		{
			fscanf(fp, "%f %f %f\n", &X[number].p[0], &X[number].p[1], &X[number].p[2]); 
			X[number].p[3]=1;
			number++;
		}
		else if(token[0]=='v' && token[1]=='t')
		{
			float temp[2];
			fscanf(fp, "%f %f\n", &temp[0], &temp[1]);
		}
		else if(token[0]=='v' && token[1]=='n')
		{				
			vertex_normal_number++;
		}
		else if(token[0]=='f' && token[1]=='\0')
		{
			int data[64];
			int data_number=0;
			memset(data, 0, sizeof(int)*64);
			
			while(feof(fp)==0)
			{
				fscanf(fp, "%d", &data[data_number++]);
				bool end=false;
				while(feof(fp)==0)
				{
					char token=fgetc(fp);
					if(token==' ' || token=='\t')	break;
					if(token=='\n'|| token=='\r')	{end=true; break;}
				}
				if(end)	break;
			}
			for(int i=1; i<data_number-1; i++)
			{
				T[t_number*3+0]=data[0  ]-1;
				T[t_number*3+1]=data[i  ]-1;
				T[t_number*3+2]=data[i+1]-1;
				t_number++;
			}
		}			
	}
	fclose(fp);


	float* TN=new float[max_number*3];

	memset(TN, 0, sizeof(float)*number*3);
	for(int i=0; i<t_number; i++)
	{
		float *p0=X[T[i*3+0]].p;
		float *p1=X[T[i*3+1]].p;
		float *p2=X[T[i*3+2]].p;

		float e0[3], e1[3];
		for(int i=0; i<3; i++)
		{
			e0[i]=p1[i]-p0[i];
			e1[i]=p2[i]-p0[i];
		}		
		Cross(e0, e1, &TN[i*3]);
		Normalize(&TN[i*3]);
	}

	for(int i=0; i<number; i++)
		X[i].n[0]=X[i].n[1]=X[i].n[2]=0;

	for(int i=0; i<t_number; i++)
	{
		int v0=T[i*3+0];
		int v1=T[i*3+1];
		int v2=T[i*3+2];
		X[v0].n[0]+=TN[i*3+0];
		X[v0].n[1]+=TN[i*3+1];
		X[v0].n[2]+=TN[i*3+2];
		X[v1].n[0]+=TN[i*3+0];
		X[v1].n[1]+=TN[i*3+1];
		X[v1].n[2]+=TN[i*3+2];
		X[v2].n[0]+=TN[i*3+0];
		X[v2].n[1]+=TN[i*3+1];
		X[v2].n[2]+=TN[i*3+2];
	}
	for(int i=0; i<number; i++)	Normalize(X[i].n);

	delete[] TN;
}

void MESH::Read_OBJ_File_Advanced(const char *filename)
{
	float *v_list =new float[max_verts*3+3];
	float *vt_list=new float[max_verts*2+2];
	vt_list[0] = vt_list[1] = 0.0;
	float *vn_list=new float[max_verts*3+3];
	int v_number=0;
	int vt_number=0;
	int vn_number=0;

	int *t_list=new int[max_tris*9+9];
	t_number=0;



	FILE *fp=fopen(filename, "r+");		
	while(feof(fp)==0)
	{
		char token[1024];
		fscanf(fp, "%s", &token);
		if((token[0]=='#' || token[0]=='g') && token[1]=='\0')
		{
			int c;
			while(feof(fp)==0)
				if((c=fgetc(fp))=='\r' || c=='\n')	break;
		}
		else if(token[0]=='v' && token[1]=='\0')	//vertex
		{

			fscanf(fp, "%f %f %f\n", &v_list[v_number*3], &v_list[v_number*3+1], &v_list[v_number*3+2]); 
			v_number++;
		}
		else if(token[0]=='v' && token[1]=='t')
		{
			fscanf(fp, "%f %f\n", &vt_list[vt_number*2], &vt_list[vt_number*2+1]);
			vt_number++;
		}
		else if(token[0]=='v' && token[1]=='n')
		{	
			fscanf(fp, "%f %f %f\n", &vn_list[vn_number*3], &vn_list[vn_number*3+1], &vn_list[vn_number*3+2]); 
			vn_number++;
		}
		else if(token[0]=='s' && token[1]=='\0')
		{
			float temp[1];
			fscanf(fp, "%f\n", &temp[0]);
		}
		else if(token[0]=='f' && token[1]=='\0')
		{
			int data[512];
			int data_number=0;
			memset(data, 0, sizeof(int)*512);

			while(feof(fp)==0)
			{
				fscanf(fp, "%d/%d/%d", &data[data_number*3], &data[data_number*3+1], &data[data_number*3+2]);
				data_number++;
				bool end=false;
				while(feof(fp)==0)
				{
					char token=fgetc(fp);
					if(token==' ' || token=='\t')	break;
					if(token=='\n'|| token=='\r')	{end=true; break;}
				}
				if(end)	break;
			}
			for(int i=1; i<data_number-1; i++)
			{					
				t_list[t_number*9+0]=data[0*3+0]-1;
				t_list[t_number*9+1]=data[0*3+1]-1;
				t_list[t_number*9+2]=data[0*3+2]-1;

				t_list[t_number*9+3]=data[i*3+0]-1;
				t_list[t_number*9+4]=data[i*3+1]-1;
				t_list[t_number*9+5]=data[i*3+2]-1;

				t_list[t_number*9+6]=data[i*3+3]-1;
				t_list[t_number*9+7]=data[i*3+4]-1;
				t_list[t_number*9+8]=data[i*3+5]-1;
				t_number++;
			}
		}			
	}
	fclose(fp);


	for(int t=0; t<t_number; t++)
	{
		// WRONG! these are uv
		//T[t*3+0]=t_list[t*9+1];
		//T[t*3+1]=t_list[t*9+4];
		//T[t*3+2]=t_list[t*9+7];
		T[t*3+0]=t_list[t*9+0];
		T[t*3+1]=t_list[t*9+3];
		T[t*3+2]=t_list[t*9+6];

		X[T[t*3+0]].p[0]=v_list[t_list[t*9+0]*3+0];
		X[T[t*3+0]].p[1]=v_list[t_list[t*9+0]*3+1];
		X[T[t*3+0]].p[2]=v_list[t_list[t*9+0]*3+2];
		X[T[t*3+0]].p[3]=1;

		X[T[t*3+1]].p[0]=v_list[t_list[t*9+3]*3+0];
		X[T[t*3+1]].p[1]=v_list[t_list[t*9+3]*3+1];
		X[T[t*3+1]].p[2]=v_list[t_list[t*9+3]*3+2];
		X[T[t*3+1]].p[3]=1;

		X[T[t*3+2]].p[0]=v_list[t_list[t*9+6]*3+0];
		X[T[t*3+2]].p[1]=v_list[t_list[t*9+6]*3+1];
		X[T[t*3+2]].p[2]=v_list[t_list[t*9+6]*3+2];
		X[T[t*3+2]].p[3]=1;

		X[T[t*3+0]].n[0]=vn_list[t_list[t*9+2]*3+0];
		X[T[t*3+0]].n[1]=vn_list[t_list[t*9+2]*3+1];
		X[T[t*3+0]].n[2]=vn_list[t_list[t*9+2]*3+2];
		//X[T[t*3+0]].n[3]=0;

		X[T[t*3+1]].n[0]=vn_list[t_list[t*9+5]*3+0];
		X[T[t*3+1]].n[1]=vn_list[t_list[t*9+5]*3+1];
		X[T[t*3+1]].n[2]=vn_list[t_list[t*9+5]*3+2];
		//X[T[t*3+1]].n[3]=0;

		X[T[t*3+2]].n[0]=vn_list[t_list[t*9+8]*3+0];
		X[T[t*3+2]].n[1]=vn_list[t_list[t*9+8]*3+1];
		X[T[t*3+2]].n[2]=vn_list[t_list[t*9+8]*3+2];
		//X[T[t*3+2]].n[3]=0;

		X[T[t*3+0]].uv[0]=vt_list[t_list[t*9+1]*2+0];
		X[T[t*3+0]].uv[1]=vt_list[t_list[t*9+1]*2+1];

		X[T[t*3+1]].uv[0]=vt_list[t_list[t*9+4]*2+0];
		X[T[t*3+1]].uv[1]=vt_list[t_list[t*9+4]*2+1];

		X[T[t*3+2]].uv[0]=vt_list[t_list[t*9+7]*2+0];
		X[T[t*3+2]].uv[1]=vt_list[t_list[t*9+7]*2+1];
	}

	// WRONG!
	//number=t_number*3;
	for (int i = 0; i <= t_number*3+2; ++i) {
		if (T[i] > number) number = T[i];
	}
	++number;

	delete[] v_list;
	delete[] vt_list;
	delete[] vn_list;
	delete[] t_list;
}

void MESH::Scale(float s)
{
	for(int i=0; i<number; i++)
	{
		X[i].p[0]*=s;
		X[i].p[1]*=s;
		X[i].p[2]*=s;
		//printf("I %d: %f, %f, %f\n", i, X[i].p[0], X[i].p[1], X[i].p[2]);
	}
}

void MESH::Centerize()
{
	//Make sure that the bunny is centered at the origin.
	float center[3];
	center[0]=center[1]=center[2]=0;
	for(int i=0; i<number; i++)
	{
		center[0]+=X[i].p[0];
		center[1]+=X[i].p[1];
		center[2]+=X[i].p[2];
	}
	center[0]/=number;
	center[1]/=number;
	center[2]/=number;
	for(int i=0; i<number; i++)
	{
		X[i].p[0]-=center[0];
		X[i].p[1]-=center[1];
		X[i].p[2]-=center[2];
	}
}

void MESH::Init()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vertex_handle);
	glGenBuffers(1, &triangle_handle);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_handle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX)*number, X, GL_STATIC_DRAW);

	glEnableVertexAttribArray(vpos);
	glVertexAttribPointer(vpos, 4, GL_FLOAT, GL_FALSE, sizeof(VERTEX),
	                      reinterpret_cast<void*>(offsetof(VERTEX, p)));

	glEnableVertexAttribArray(vnorm);
	glVertexAttribPointer(vnorm, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX),
	                      reinterpret_cast<void*>(offsetof(VERTEX, n)));

	glEnableVertexAttribArray(vtex);
	glVertexAttribPointer(vtex, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX),
	                      reinterpret_cast<void*>(offsetof(VERTEX, uv)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangle_handle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*t_number*3, T,
	             GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void MESH::Draw() {
	glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, t_number*3, GL_UNSIGNED_INT,
		               (char*) NULL+0);
	glBindVertexArray(0);
}

}
