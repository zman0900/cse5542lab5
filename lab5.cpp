#include <iostream>

#include "GlGlut.h"

using namespace std;
using namespace lab5;

int main(int argc, char *argv[]) {
	cout << "CSE 5542 - Lab 5" << endl;
	cout << "Dan Ziemba, 2013" << endl << endl;
	cout << "Controls:" << endl;
	cout << "\t'j' - Move dog left" << endl;
	cout << "\t'l' - Move dog right" << endl;
	cout << "\t'i' - Move dog away" << endl;
	cout << "\t'k' - Move dog closer" << endl;
	cout << "\t'u' - Rotate dog left" << endl;
	cout << "\t'o' - Rotate dog right" << endl;
	cout << "\t',' - Rotate mirror left" << endl;
	cout << "\t'.' - Rotate mirror right" << endl;
	cout << "\t'm' - Toggle multisample" << endl;
	cout << "\t'r' - Reload shaders" << endl;
	cout << "\t'f' - Toggle fullscreen" << endl;
	cout << endl;

	GlGlut g;
	g.start(&argc, argv);

	return 0;
}
