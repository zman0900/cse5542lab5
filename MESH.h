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
#ifndef __WHMIN_MESH_H__
#define __WHMIN_MESH_H__

#ifdef _WIN32
#include <Windows.h>
#pragma comment(lib, "glew32.lib")
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>
#include <cstring>

namespace lab5 {


float Normalize(float *x, float *r=0);

void Cross(float x[], float y[], float r[]);


typedef struct 
{
    float p[4];     //Vertex Position
    float n[3];     //Vertex Color
	float uv[2];
} VERTEX;


class MESH
{
public:
	int max_number;
	int max_verts;
	int max_tris;

	//vertex data
	unsigned int number; //vertex number
	VERTEX* X;


	//triangle data
	unsigned int t_number; //triangle number
	GLuint*	T; //triangle indices

    GLuint vertex_handle;
    GLuint triangle_handle;
    GLuint vao;
    GLint vpos, vnorm, vtex;
    GLint tex_c, tex_s;

	MESH();

	~MESH();

	void Read_OBJ_File(const char *filename);

	void Read_OBJ_File_Advanced(const char *filename);

	void Scale(float s);

	void Centerize();

	void Create_Sphere(float radius, int m, int n);

	void Build_Normal();
	
	void Init();

	void Draw();
};

}

#endif
