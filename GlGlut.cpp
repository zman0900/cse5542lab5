#include "GlGlut.h"

using namespace std;

namespace lab5 {

void loadTexture(const string& filename, GLuint *texId, GLenum texNum,
                 GLint location) {
	TargaImage *tex = new TargaImage();
	if (!tex->load(filename))
		cerr << "Failed to load texture!" << endl;
	glGenTextures(1, texId);
	glUniform1i(location, texNum);
	glActiveTexture(texNum);
	glBindTexture(GL_TEXTURE_2D, *texId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
	                GL_LINEAR_MIPMAP_NEAREST);
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

GlGlut *GlGlut::instance = NULL;

//// Glut callbacks /////
void GlGlut::display() {
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

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
			dog_program = Setup_GLSL("dog");
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

	if (w > h) {
		glViewport((w-h)/2, 0, h, h);
	} else {
		glViewport(0, (h-w)/2, w, w);
	}

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
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
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
		exit(1);
	}
#endif

	Check_GPU_Status();

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
	printf("Loading dog shader...\n");
	dog_program = Setup_GLSL("dog");
	glUseProgram(dog_program);
		dog->vpos = glGetAttribLocation(dog_program, "vpos");
		dog->vnorm = glGetAttribLocation(dog_program, "vnorm");
		dog->vtex = glGetAttribLocation(dog_program, "vtex");
		dog->tex_c = glGetUniformLocation(dog_program, "tex_c");
		dog->tex_n = glGetUniformLocation(dog_program, "tex_n");
		dog->tex_s = glGetUniformLocation(dog_program, "tex_s");
	glUseProgram(0);

	// Textures
	printf("Loading dog textures...\n");
	loadTexture("doberman_c.tga", &dog_texCId, GL_TEXTURE0, dog->tex_c);
	loadTexture("doberman_n.tga", &dog_texNId, GL_TEXTURE1, dog->tex_n);
	loadTexture("doberman_s.tga", &dog_texSId, GL_TEXTURE2, dog->tex_s);

	// Initialize VBOs and VAO
	dog->Init();

	// Start
	reshape(screen_width, screen_height);
	glutIdleFunc(idleWrapper);
	glutMainLoop();
}

} // namespace lab5
