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
#include "targa.h"

#define DEF_SCREEN_W 800
#define DEF_SCREEN_H 800
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
	MESH *dog;
	GLuint dog_texCId, dog_texNId, dog_texSId;
	GLuint dog_program;

	int xform_mode;
	int press_x, press_y;
	float x_angle,y_angle;
	float scale_size;

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
