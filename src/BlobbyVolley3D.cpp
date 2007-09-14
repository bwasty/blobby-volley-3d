#include "Game.h"
#include <gl/glut.h>

/**
 * Program entry
 */
int main(int argc, char** argv) {
	glutInit(&argc,argv);	// init glut
	BV3D::Game game;		// create the main game object
	glutMainLoop();			// start the window event loop
}
