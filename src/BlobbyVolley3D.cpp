/*
 * BlobbyVolley3d.cpp: program entry
 */


#include "Game.h"
#include <gl/glut.h>

int main(int argc, char** argv) {
	glutInit(&argc,argv);	// init glut
	Game game;				// create the game
	glutMainLoop();			// start the window event loop
}
