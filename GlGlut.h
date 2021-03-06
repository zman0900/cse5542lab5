#ifndef __GLGLUT_H__
#define __GLGLUT_H__

#ifdef _WIN32
#include <Windows.h>
#pragma comment(lib, "glew32.lib")
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <cstddef>

#include "MY_GLSL.h"
#include "MESH.h"
#include "PXM_IO.h"
#include "targa.h"
#include "texturedplane.h"

#define DEF_SCREEN_W 800
#define DEF_SCREEN_H 800
#define MIRROR_TEX_W 1024
#define MIRROR_TEX_H 1024
#define CHECK_WIDTH 1024	// Dimensions of checkerboard texture
#define CHECK_HEIGHT 1024
#define WINDOW_TITLE "CSE 5542 Lab 5 - Dan Ziemba"
#define XFORM_NONE 0
#define XFORM_ROTATE 1
#define XFORM_SCALE 2

namespace lab5 {

class GlGlut {
private:

protected:
	static GlGlut *instance;
	int screen_width;
	int screen_height;
	bool multisample, fullscreen;
	MESH *dog, *env;
	GLuint dog_texCId, dog_texSId;
	GLuint dog_program, mirror_program, env_program;

	GLuint fboId, rboId;
	GLuint mirrorTexId, checkTexId, envTexId;
	TexturedPlane *mirror, *floor;
	GLubyte *checkerTexture;

	int xform_mode;
	int press_x, press_y;
	float scale_size;
	int mirrorAngle, dogAngle;
	float dogX, dogZ;

	// Glut callbacks
	void display();
	void idle();
	void keyboard(unsigned char key, int mousex, int mousey);
	void mouseClick(int button, int state, int x, int y);
	void mouseMotion(int x, int y);
	void reshape(int w, int h);

	////  Static wrappers used to register glut callbacks
	static void displayWrapper();
	static void idleWrapper();
	static void keyboardWrapper(unsigned char key, int mousex, int mousey);
	static void mouseClickWrapper(int button, int state, int x, int y);
	static void mouseMotionWrapper(int x, int y);
	static void reshapeWrapper(int w, int h);

	// Others
	void createCheckerboardTexture();
	void loadEnvTexture();
	void loadTexture(const string& filename, GLuint *texId);
	void loadShaders();
	void setViewport();
	void drawScene();
public:
	GlGlut();
	~GlGlut();

	// Called by start to make wrappers work
	void setInstance();
	// Starts scene, does not return
	void start(int *argc, char *argv[]);
};

} // namespace lab5

#endif
