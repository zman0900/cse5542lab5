#include "GlGlut.h"

using namespace std;

namespace lab5 {

GlGlut *GlGlut::instance = NULL;

void GlGlut::loadTexture(const string& filename, GLuint *texId) {
	TargaImage *tex = new TargaImage();
	if (!tex->load(filename))
		cerr << "Failed to load texture!" << endl;
	glGenTextures(1, texId);
	glBindTexture(GL_TEXTURE_2D, *texId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
	                GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
	int bpp = tex->getBitsPerPixel();
	GLenum format = 0;
	GLenum internal_format = 0;
	switch (bpp) {
		case 8:
			format = GL_LUMINANCE;
			internal_format = GL_INTENSITY;
			break;
		case 24:
			format = GL_RGB;
			internal_format = GL_RGB8;
			break;
		case 32:
			format = GL_RGBA;
			internal_format = GL_RGBA;
			break;
		default:
			cerr << "Unknown testure format!" << endl;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, tex->getWidth(),
	             tex->getHeight(), 0, format, GL_UNSIGNED_BYTE,
	             tex->getImageData());
	glGenerateMipmap(GL_TEXTURE_2D);
	delete tex;
}

void GlGlut::loadShaders() {
	printf("Loading dog shader...\n");
	dog_program = Setup_GLSL("dog");
	glUseProgram(dog_program);
		dog->vpos = glGetAttribLocation(dog_program, "vpos");
		dog->vnorm = glGetAttribLocation(dog_program, "vnorm");
		dog->vtex = glGetAttribLocation(dog_program, "vtex");
		dog->tex_c = glGetUniformLocation(dog_program, "tex_c");
		dog->tex_s = glGetUniformLocation(dog_program, "tex_s");
	glUseProgram(0);
}

void GlGlut::setViewport() {
	if (screen_width > screen_height) {
		glViewport((screen_width-screen_height)/2, 0, screen_height,
		           screen_height);
	} else {
		glViewport(0, (screen_height-screen_width)/2, screen_width,
		           screen_width);
	}
}

//// Glut callbacks /////
void GlGlut::display() {
	// adjust viewport and projection matrix to texture dimension
	glViewport(0, 0, MIRROR_TEX_W, MIRROR_TEX_H);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (float)(MIRROR_TEX_W)/MIRROR_TEX_H, 1.0f, 100.0f);
	glMatrixMode(GL_MODELVIEW);

	// Switch to mirror FBO
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);

	// Clear FBO
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw mirrored objects here

	// Back to main framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Bind mirror texture and generate mipmaps
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, mirrorTexId);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Clear main framebuffer
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// Back to normal viewport
	setViewport();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1, .1, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0,0,5,0,0,0,0,1,0);

	glRotated(x_angle, 0.0, 1.0, 0.0);
	glRotated(y_angle, 1.0, 0.0, 0.0);
	glScaled(scale_size, scale_size, scale_size);

	// Draw dog using vertex array object
	glUseProgram(dog_program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, dog_texCId);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, dog_texSId);

	glBindVertexArray(dog->vao);
		glDrawElements(GL_TRIANGLES, dog->t_number*3, GL_UNSIGNED_INT,
		               (char*) NULL+0);
	glBindVertexArray(0);
	glUseProgram(0);

	glutSwapBuffers();
}

void GlGlut::idle() {

}

void GlGlut::keyboard(unsigned char key, int mousex, int mousey) {
	switch(key) {
		case 3:  // Ctrl-c
		case 27: // Esc
			exit(EXIT_SUCCESS);
			break;
		case 'r':
			cout << "Reloading shaders" << endl;
			loadShaders();
			break;
		case 'm':
			if (multisample) {
				glDisable(GL_MULTISAMPLE);
				cout << "Multisample off" << endl;
			} else {
				glEnable(GL_MULTISAMPLE);
				cout << "Multisample on" << endl;
			}
			multisample = !multisample;
			break;
		default:
			//cout << "unused key: " << (int) key << endl;
			return;
	}
	glutPostRedisplay();
}

void GlGlut::mouseClick(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		press_x = x;
		press_y = y;
		if (button == GLUT_LEFT_BUTTON)
			xform_mode = XFORM_ROTATE;
		else if (button == GLUT_RIGHT_BUTTON)
			xform_mode = XFORM_SCALE;
	} else if (state == GLUT_UP)
		xform_mode = XFORM_NONE;
}

void GlGlut::mouseMotion(int x, int y) {
	if (xform_mode==XFORM_ROTATE) {
		x_angle += (x - press_x)/5.0;
		if (x_angle > 180) x_angle -= 360;
		else if (x_angle <-180) x_angle += 360;
		press_x = x;

		y_angle += (y - press_y)/5.0;
		if (y_angle > 180) y_angle -= 360;
		else if (y_angle <-180) y_angle += 360;
		press_y = y;
    } else if (xform_mode == XFORM_SCALE) {
		float old_size = scale_size;
		scale_size *= (1+ (y - press_y)/60.0);
		if (scale_size <0) scale_size = old_size;
		press_y = y;
    }
    glutPostRedisplay();
}

void GlGlut::reshape(int w, int h) {
	screen_width = w;
	screen_height = h;

	setViewport();

	glutPostRedisplay();
}
/////////////////////////

//// Static wrappers ////
void GlGlut::displayWrapper() {
	instance->display();
}

void GlGlut::idleWrapper() {
	instance->idle();
}

void GlGlut::keyboardWrapper(unsigned char key, int mousex, int mousey) {
	instance->keyboard(key, mousex, mousey);
}

void GlGlut::mouseClickWrapper(int button, int state, int x, int y) {
	instance->mouseClick(button, state, x, y);
}

void GlGlut::mouseMotionWrapper(int x, int y) {
	instance->mouseMotion(x, y);
}

void GlGlut::reshapeWrapper(int w, int h) {
	instance->reshape(w, h);
}
/////////////////////////

GlGlut::GlGlut() {
	screen_width = DEF_SCREEN_W;
	screen_height = DEF_SCREEN_H;
	xform_mode = XFORM_NONE;
	x_angle = 0.0f;
	y_angle = 0.0f;
	scale_size = 8.0f;
}

GlGlut::~GlGlut() {
	// Nothing to do here
}

void GlGlut::setInstance() {
	instance = this;
}

void GlGlut::start(int *argc, char *argv[]) {
	setInstance();

	// Init glut
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH|GLUT_MULTISAMPLE);
	glutInitWindowSize(screen_width, screen_height);
	glutCreateWindow(WINDOW_TITLE);

	// Register callbacks
	glutDisplayFunc(displayWrapper);
	glutKeyboardFunc(keyboardWrapper);
	glutMouseFunc(mouseClickWrapper);
	glutMotionFunc(mouseMotionWrapper);
	glutReshapeFunc(reshapeWrapper);

	// glew?
#ifdef _WIN32
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		cerr << "Fatal Error: " << glewGetErrorString(err) << endl;
		exit(EXIT_FAILURE);
	}
#endif

	Check_GPU_Status();

	// Print multisample info
	multisample = true;
	GLint buf, sbuf;
	glGetIntegerv(GL_SAMPLE_BUFFERS, &buf);
	printf("number of sample buffers is %d\n", buf);
	glGetIntegerv(GL_SAMPLES, &sbuf);
	printf("number of samples is %d\n", sbuf);

	// Turn on depth testing
	glEnable(GL_DEPTH_TEST);

	// Turn on culling
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	// Meshes
	printf("Loading dog mesh...\n");
	dog = new MESH();
	//dog->Read_OBJ_File_Advanced("vase_02.obj");
	dog->Read_OBJ_File_Advanced("doberman.obj");
	//dog->Read_OBJ_File("bunny.obj");
	dog->Scale(.25);
	dog->Centerize();
	cout << "\tVerts:" << dog->number << " Tri:" << dog->t_number << endl;

	// Shaders
	loadShaders();

	// Textures
	printf("Loading dog textures...\n");
	loadTexture("doberman_c.tga", &dog_texCId);
	glUniform1i(dog->tex_c, 0);
	loadTexture("doberman_s.tga", &dog_texSId);
	glUniform1i(dog->tex_s, 1);

	// Initialize VBOs and VAO
	dog->Init();

	// Texture for mirror
	glGenTextures(1, &mirrorTexId);
	glBindTexture(GL_TEXTURE_2D, mirrorTexId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
	                GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, MIRROR_TEX_W, MIRROR_TEX_H, 0,
	             GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// FBO for mirror
	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	// Need render buffer for depth so depth test will work in FBO
	glGenRenderbuffers(1, &rboId);
	glBindRenderbuffer(GL_RENDERBUFFER, rboId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, MIRROR_TEX_W,
	                      MIRROR_TEX_H);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	// attach a texture to FBO color attachement point
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
	                       mirrorTexId, 0);
	// attach a renderbuffer to depth attachment point
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
	                          GL_RENDERBUFFER, rboId);
	// Check status
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cerr << "Framebuffer setup failed!" << endl;
		exit(EXIT_FAILURE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Start
	reshape(screen_width, screen_height);
	glutIdleFunc(idleWrapper);
	glutMainLoop();
}

} // namespace lab5
