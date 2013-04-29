#include "GlGlut.h"

using namespace std;

namespace lab5 {

GlGlut *GlGlut::instance = NULL;

void GlGlut::createCheckerboardTexture() {
  for (int irow = 0; irow < CHECK_HEIGHT; irow++)
    for (int jcol = 0; jcol < CHECK_WIDTH; jcol++) {
      int bit = (irow & 4) ^ (jcol & 4);
      GLubyte c = 0;
      if (bit) { c = 255; };
      checkerTexture[irow][jcol][0] = c;
      checkerTexture[irow][jcol][1] = c;
      checkerTexture[irow][jcol][2] = c;
    }
}

void GlGlut::loadEnvTexture() {
	int width, height;
	bool useFloat = hasFloatTextures();
	float *dataf=0;
	unsigned char *datai=0;
	if (useFloat)
		PXM_Read("stpeters_probe.pfm", &dataf, width, height);
	else {
		cout << "\tFloating point textures not supported! "
		     << "Falling back to integer."
		     << endl;
		PXM_Read("stpeters_probe.pfm", &datai, width, height);
	}
	glGenTextures(1, &envTexId);
	glBindTexture(GL_TEXTURE_2D, envTexId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
	                GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
	                GL_LINEAR);
	if (useFloat)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height, 0, GL_RGBA,
		         GL_FLOAT, dataf);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB,
		         GL_UNSIGNED_BYTE, datai);
	delete[] dataf;
	delete[] datai;
}

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
	cout << "Loading dog shader..." << endl;
	dog_program = Setup_GLSL("dog");
	glUseProgram(dog_program);
		dog->vpos = glGetAttribLocation(dog_program, "vpos");
		dog->vnorm = glGetAttribLocation(dog_program, "vnorm");
		dog->vtex = glGetAttribLocation(dog_program, "vtex");
		dog->tex_c = glGetUniformLocation(dog_program, "tex_c");
		dog->tex_s = glGetUniformLocation(dog_program, "tex_s");
	glUseProgram(0);
	cout << "Loading texturedplane shader..." << endl;
	mirror_program = Setup_GLSL("texturedplane");
	glUseProgram(mirror_program);
		mirror->vpos = glGetAttribLocation(mirror_program, "vpos");
		mirror->vnorm = glGetAttribLocation(mirror_program, "vnorm");
		mirror->vtex = glGetAttribLocation(mirror_program, "vtex");
		mirror->tex = glGetUniformLocation(mirror_program, "tex");
		floor->vpos = glGetAttribLocation(mirror_program, "vpos");
		floor->vnorm = glGetAttribLocation(mirror_program, "vnorm");
		floor->vtex = glGetAttribLocation(mirror_program, "vtex");
		floor->tex = glGetUniformLocation(mirror_program, "tex");
	glUseProgram(0);
	cout << "Loading environment shader..." << endl;
	env_program = Setup_GLSL("env");
	glUseProgram(env_program);
		env->vpos = glGetAttribLocation(env_program, "vertex_position");
		env->vnorm = glGetAttribLocation(env_program, "vertex_normal");
		env->tex_c = glGetUniformLocation(env_program, "env_texture");
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

void GlGlut::drawScene() {
	// Draw environment
	glUseProgram(env_program);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, envTexId);
		glUniform1i(env->tex_c, 4);
		env->Draw();
	glUseProgram(0);

	// Draw floor
	glPushMatrix();
	glRotated(-90.0, 1.0, 0.0, 0.0);
	glScaled(100.0, 100.0, 100.0);  // Huge floor
	glTranslated(0.0, -0.5, 0.0);
	glUseProgram(mirror_program);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, checkTexId);
		glUniform1i(floor->tex, 3);
		floor->draw();
	glUseProgram(0);
	glPopMatrix();

	// Draw dog using vertex array object
	glPushMatrix();
	glTranslated(dogX, 0.0, dogZ);
	glRotated(dogAngle, 0.0, 1.0, 0.0);
	glUseProgram(dog_program);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, dog_texCId);
		glUniform1i(dog->tex_c, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, dog_texSId);
		glUniform1i(dog->tex_s, 1);
		dog->Draw();
	glUseProgram(0);
	glPopMatrix();
}

//// Glut callbacks /////
void GlGlut::display() {
	// Switch to mirror FBO
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);

	// adjust viewport and projection matrix to texture dimension
	glViewport(0, 0, MIRROR_TEX_W, MIRROR_TEX_H);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, (double)(MIRROR_TEX_W)/MIRROR_TEX_H, 0.1, 10000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0., -0.5, 1.);

	// Clear FBO
	glClearColor(1., 1., 1., 1.);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glTranslated(0.0, -0.5, 0.0);
	glPushMatrix();
	// Transform to reflector's local coordinate system
	glTranslated(0.0, 0.0, -1.0);
	glRotated(mirrorAngle, 0.0, 1.0, 0.0);
	// Reflect object across -z
	glScaled(1.0, 1.0, -1.0);
	// Transform back to world coordinates
	glRotated(-1*mirrorAngle, 0.0, 1.0, 0.0);
	glTranslated(0.0, 0.0, 1.0);
	// Draw mirrored objects here
	drawScene();
	glPopMatrix();

	// Back to main framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Bind mirror texture and generate mipmaps
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, mirrorTexId);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Clear main framebuffer
	glClearColor(1., 1., 1., 1.);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// Back to normal viewport
	setViewport();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 1., 0.1, 10000.);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.,7.,10.,
	          0.,0.5,-1.,
	          0.,1.,0.);

	glRotated(x_angle, 0.0, 1.0, 0.0);
	glRotated(y_angle, 1.0, 0.0, 0.0);
	glScaled(scale_size, scale_size, scale_size);

	// Draw mirror
	glPushMatrix();
	glTranslated(0.0, 0.0, -1.0);
	glRotated(mirrorAngle, 0.0, 1.0, 0.0);
	glUseProgram(mirror_program);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mirrorTexId);
		glUniform1i(mirror->tex, 2);
		mirror->draw();
	glUseProgram(0);
	glPopMatrix();

	drawScene();

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
		case ',':
			mirrorAngle -= 5;
			if (mirrorAngle < -90) mirrorAngle = -90;
			break;
		case '.':
			mirrorAngle += 5;
			if (mirrorAngle > 90) mirrorAngle = 90;
			break;
		case 'j':
			dogX -= 0.1;
			break;
		case 'l':
			dogX += 0.1;
			break;
		case 'i':
			dogZ -= 0.1;
			break;
		case 'k':
			dogZ += 0.1;
			break;
		case 'u':
			dogAngle = (dogAngle - 5) % 360;
			break;
		case 'o':
			dogAngle = (dogAngle + 5) % 360;
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
	multisample = true;
	mirrorAngle = -15;
	dogAngle = -45;
	dogX = dogZ = 0.0;
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
	GLint buf, sbuf;
	glGetIntegerv(GL_SAMPLE_BUFFERS, &buf);
	printf("number of sample buffers is %d\n", buf);
	glGetIntegerv(GL_SAMPLES, &sbuf);
	printf("number of samples is %d\n", sbuf);

	// Turn on depth testing
	glEnable(GL_DEPTH_TEST);

	// Turn on culling (prevents floor and env textures from working in mirror)
	//glEnable(GL_CULL_FACE);

	// Meshes
	printf("Loading dog mesh...\n");
	dog = new MESH();
	dog->Read_OBJ_File_Advanced("doberman.obj");
	dog->Scale(.25);
	cout << "\tVerts:" << dog->number << " Tri:" << dog->t_number << endl;
	mirror = new TexturedPlane();
	floor = new TexturedPlane();
	env = new MESH();
	env->Create_Sphere(50, 80, 80);

	// Shaders
	loadShaders();

	// Textures
	printf("Loading dog textures...\n");
	loadTexture("doberman_c.tga", &dog_texCId);
	glUniform1i(dog->tex_c, 0);
	loadTexture("doberman_s.tga", &dog_texSId);
	glUniform1i(dog->tex_s, 1);
	printf("Loading environment texture...\n");
	loadEnvTexture();

	// Texture for floor
	printf("Generating floor texture...\n");
	createCheckerboardTexture();
	glGenTextures(1, &checkTexId);
	glBindTexture(GL_TEXTURE_2D, checkTexId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
	                GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, CHECK_WIDTH, CHECK_HEIGHT, 0,
	             GL_RGB, GL_UNSIGNED_BYTE, checkerTexture);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(floor->tex, 3);

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
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(mirror->tex, 2);

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

	// Initialize VBOs and VAO
	dog->Init();
	mirror->initVBO();
	floor->initVBO();
	env->Init();

	// Start
	reshape(screen_width, screen_height);
	//glutIdleFunc(idleWrapper);
	glutMainLoop();
}

} // namespace lab5
