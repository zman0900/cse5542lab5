#include <iostream>

#include "GlGlut.h"

using namespace std;

int main(int argc, char *argv[]) {
	cout << "CSE 5542 - Lab 5" << endl;
	cout << "Dan Ziemba, 2013" << endl << endl;
	cout << "Controls:" << endl;
	/*cout << "\t'1' - Phong shader (point light behind camera)" << endl;
	cout << "\t'2' - Checkerboard shader" << endl;
	cout << "\t'3' - Wood shader" << endl;
	cout << "\t'4' - Phong shader (fixed point light)" << endl;
	cout << "\t'5' - Phong shader (directional light infinity behind camera)"
	     << endl << endl;*/

	lab5::GlGlut g;
	g.start(&argc, argv);

	return 0;
}
