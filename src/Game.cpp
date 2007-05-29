/*
 * Game.cpp: define class Game
 */

#include "Game.h"
#include <vrs/time.h>
#include <vrs/sg/clock.h>
#include <vrs/sg/key.h>
#include <vrs/sg/keyevent.h>

using namespace BV3D;

Game::Game() {
	m_Canvas = new GlutCanvas("BlobbyVolley3d",0,0);	// create the main window

	// TODO: setup arena
	setExtent(Vector(0.6,0.3,0.4));		// obsolete: should be replaced with m_Arena.setExtent()...

	// blobbs setup
	m_Blobb[0].setPosition(Vector(-0.2,0.0,0.3));
	m_Blobb[1].setPosition(Vector(0.2,0.0,0.3));

	// init update callback
	m_FPS = 30.0;
	m_cbUpdate = new BehaviorCallback();
	m_cbUpdate->setTimeCallback(new MethodCallback<Game>(this,&Game::update));
	m_Canvas->append(m_cbUpdate);

	// init input callback
	m_cbInput = new BehaviorCallback();
	m_cbInput->setCanvasCallback(new MethodCallback<Game>(this,&Game::processInput));
	m_Canvas->append(m_cbInput);
}

Game::~Game() {
}

/*
 * obsolete: should be defined in class Arena
 */
void Game::setExtent(VRS::Vector vtrExtent) {
	Vector llf = Vector(-vtrExtent[0]/2, 0.0,-vtrExtent[2]/2);
	Vector urb = Vector( vtrExtent[0]/2, vtrExtent[1], vtrExtent[2]/2);

	m_Bounds = Bounds(llf,urb);

	m_Blobb[0].setBounds(Bounds(llf,Vector(0.0,urb[1],urb[2])));
	m_Blobb[1].setBounds(Bounds(Vector(0.0,0.0,llf[2]),urb));
}

void Game::update() {
	// count frames per second
	/*VRSTime time = m_Canvas->clock()->time();
	if(double(time) - m_dLastUpdateTime >= 1/m_FPS) {
		m_dLastUpdateTime = double(time);
		
		m_iFramerate++;
		if(m_dLastUpdateTime - m_dLastSecond >= 1.0) {
			printf("framerate: %d\n",m_iFramerate);
			m_dLastSecond = m_dLastUpdateTime;
			m_iFramerate = 0;
		}
	}*/

	// update blobbs' positions
	m_Blobb[0].update();
	m_Blobb[1].update();
}

void Game::processInput() {
	InputEvent* ie = VRS_Cast(InputEvent, m_cbInput->currentCanvasEvent());

	// TODO: add generell controls (camera positioning,...)
	KeyEvent* ke = VRS_Cast(KeyEvent, ie);
	if(ke != NULL)
		if(ke->pressed())
			if(ke->keyCode() == Key::Escape)
				exit(0);

	// pass input to blobbs
	m_Blobb[0].getControls()->processInput(ie);
	m_Blobb[1].getControls()->processInput(ie);
}