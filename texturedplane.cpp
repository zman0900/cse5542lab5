#include "texturedplane.h"
#include <cstddef>

namespace lab5 {

TexturedPlane::TexturedPlane() {
	verts = new VERTEX [number];		
	inds = new GLuint [t_number*3];
	// upper left
	verts[0].p[0] = -0.5;
	verts[0].p[1] =  1.0;
	verts[0].p[2] =  0.0;
	verts[0].p[3] =  1.0;
	verts[0].n[0] =  0.0;
	verts[0].n[1] =  0.0;
	verts[0].n[2] = -1.0;
	verts[0].uv[0] = 0.0;
	verts[0].uv[1] = 1.0;
	// upper right
	verts[1].p[0] =  0.5;
	verts[1].p[1] =  1.0;
	verts[1].p[2] =  0.0;
	verts[1].p[3] =  1.0;
	verts[1].n[0] =  0.0;
	verts[1].n[1] =  0.0;
	verts[1].n[2] = -1.0;
	verts[1].uv[0] = 1.0;
	verts[1].uv[1] = 1.0;
	// lower left
	verts[2].p[0] = -0.5;
	verts[2].p[1] =  0.0;
	verts[2].p[2] =  0.0;
	verts[2].p[3] =  1.0;
	verts[2].n[0] =  0.0;
	verts[2].n[1] =  0.0;
	verts[2].n[2] = -1.0;
	verts[2].uv[0] = 0.0;
	verts[2].uv[1] = 0.0;
	// lower right
	verts[3].p[0] =  0.5;
	verts[3].p[1] =  0.0;
	verts[3].p[2] =  0.0;
	verts[3].p[3] =  1.0;
	verts[3].n[0] =  0.0;
	verts[3].n[1] =  0.0;
	verts[3].n[2] = -1.0;
	verts[3].uv[0] = 1.0;
	verts[3].uv[1] = 0.0;

	// Triangle indices
	inds[0] = 2;
	inds[1] = 1;
	inds[2] = 0;
	inds[3] = 2;
	inds[4] = 3;
	inds[5] = 1;
}

void TexturedPlane::initVBO() {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vertex_handle);
	glGenBuffers(1, &triangle_handle);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_handle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX)*number, verts,
	             GL_STATIC_DRAW);

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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*t_number*3, inds,
	             GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void TexturedPlane::draw() {
	glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, t_number*3, GL_UNSIGNED_INT,
		               (char*) NULL+0);
	glBindVertexArray(0);
}

}
