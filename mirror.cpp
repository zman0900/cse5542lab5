#include "mirror.h"

namespace lab5 {

Mirror::Mirror() {
	mirrorVerts = new VERTEX [number];		
	mirrorInds = new GLuint [t_number*3];
	// upper left
	mirrorVerts[0].p[0] = -0.5;
	mirrorVerts[0].p[1] =  1.0;
	mirrorVerts[0].p[2] =  0.0;
	mirrorVerts[0].p[3] =  1.0;
	mirrorVerts[0].n[0] =  0.0;
	mirrorVerts[0].n[1] =  0.0;
	mirrorVerts[0].n[2] = -1.0;
	mirrorVerts[0].uv[0] = 0.0;
	mirrorVerts[0].uv[1] = 1.0;
	// upper right
	mirrorVerts[1].p[0] =  0.5;
	mirrorVerts[1].p[1] =  1.0;
	mirrorVerts[1].p[2] =  0.0;
	mirrorVerts[1].p[3] =  1.0;
	mirrorVerts[1].n[0] =  0.0;
	mirrorVerts[1].n[1] =  0.0;
	mirrorVerts[1].n[2] = -1.0;
	mirrorVerts[1].uv[0] = 1.0;
	mirrorVerts[1].uv[1] = 1.0;
	// lower left
	mirrorVerts[2].p[0] = -0.5;
	mirrorVerts[2].p[1] =  0.0;
	mirrorVerts[2].p[2] =  0.0;
	mirrorVerts[2].p[3] =  1.0;
	mirrorVerts[2].n[0] =  0.0;
	mirrorVerts[2].n[1] =  0.0;
	mirrorVerts[2].n[2] = -1.0;
	mirrorVerts[2].uv[0] = 0.0;
	mirrorVerts[2].uv[1] = 0.0;
	// lower right
	mirrorVerts[3].p[0] =  0.5;
	mirrorVerts[3].p[1] =  0.0;
	mirrorVerts[3].p[2] =  0.0;
	mirrorVerts[3].p[3] =  1.0;
	mirrorVerts[3].n[0] =  0.0;
	mirrorVerts[3].n[1] =  0.0;
	mirrorVerts[3].n[2] = -1.0;
	mirrorVerts[3].uv[0] = 1.0;
	mirrorVerts[3].uv[1] = 0.0;

	// Triangle indices
	mirrorInds[0] = 2;
	mirrorInds[1] = 1;
	mirrorInds[2] = 0;
	mirrorInds[3] = 2;
	mirrorInds[4] = 3;
	mirrorInds[5] = 1;
}

void Mirror::initVBO() {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vertex_handle);
	glGenBuffers(1, &triangle_handle);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_handle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX)*number, mirrorVerts,
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*t_number*3, mirrorInds,
	             GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Mirror::draw() {
	glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, t_number*3, GL_UNSIGNED_INT,
		               (char*) NULL+0);
	glBindVertexArray(0);
}

}
