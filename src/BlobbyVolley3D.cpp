/*
 * BlobbyVolley3D.cpp: program entry
 */

#include <stdio.h>
#include "Game.h"
#include <gl/glut.h>

int main(int argc, char** argv) {
	printf("WARNING: blobb1.3ds MUST be in the project or Debug/Release directory!\n");
	printf("move Blobbs with cursor keys and mouse movement!\n");
	printf("press Home to switch Arena extents.\n");
	printf("press ESC to exit!\n");
	glutInit(&argc,argv);	// init glut
	BV3D::Game game;		// create the main game object
	glutMainLoop();			// start the window event loop
}
