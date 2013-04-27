#ifndef __MIRROR_H__
#define __MIRROR_H__

#ifdef _WIN32
#include <Windows.h>
#pragma comment(lib, "glew32.lib")
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>

namespace lab5 {

class Mirror {
	private:
		typedef struct {
			float p[4];     //Vertex Position
			float n[3];     //Vertex Normal
			float uv[2];    //Vertex Texture Coordinate
		} VERTEX;

		VERTEX *mirrorVerts;
		GLuint *mirrorInds;
		GLuint vertex_handle;
		GLuint triangle_handle;
		static const int number = 4;    // Number of vertices
		static const int t_number = 2;  // Number of triangles
		GLuint vao;
	public:
		Mirror();
		void initVBO();
		void draw();

		// Shader attributes and uniforms
		GLint vpos, vnorm, vtex;
		GLint tex;
};

}

#endif
