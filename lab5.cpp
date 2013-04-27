#include <iostream>

#include "GlGlut.h"

using namespace std;

int main(int argc, char *argv[]) {
	cout << "CSE 5542 - Lab 5" << endl;
	cout << "Dan Ziemba, 2013" << endl << endl;
	cout << "Controls:" << endl;
	cout << "\t'm' - Toggle multisample" << endl;
	cout << "\t'r' - Reload shaders" << endl;
	cout << endl;

	lab5::GlGlut g;
	g.start(&argc, argv);

	return 0;
}
