/*
 * Game.cpp: define class Game
 */

#include "Game.h"
#include <vrs/time.h>
#include <vrs/sg/clock.h>
#include <vrs/sg/key.h>
#include <vrs/sg/keyevent.h>
#include "MouseControls.h"

using namespace BV3D;

Game::Game() {
	m_Canvas = new GlutCanvas("BlobbyVolley3D",600,300);	// create the main window

	m_RootScene = new SceneThing();

	m_Camera = new Camera(Vector(0.0,2.0,-10.0),Vector(0.0,0.0,0.0),30);
	m_RootScene->append(m_Camera);

	m_AmbientLight = new AmbientLight(Color(0.7));
	m_RootScene->append(m_AmbientLight);

	// TODO: setup Arena
	setExtent(Vector(8.0,10.0,2.0));		// obsolete: should be replaced with Arena::setExtent(...)

	// blobbs setup
	m_RootScene->append(m_Blobb[0].getScene());
	m_RootScene->append(m_Blobb[1].getScene());
	m_Blobb[0].setPosition(Vector(-1.0,0.0,1.0));
	m_Blobb[1].setPosition(Vector(1.0,0.0,1.0));
	m_Blobb[1].setControls(new MouseControls());

	m_Canvas->append(m_RootScene);

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

// update is called peridically on timer events to redisplay the whole scene
// this an ad-hoc solution ;-)
void Game::update() {
	VRSTime time = m_Canvas->clock()->time();

	// calculate if frame is over
	if(double(time) - m_dLastUpdateTime >= 0.8/m_FPS) {
		m_dLastUpdateTime = double(time);

		// count frames per second
		m_iFramerate++;
		if(m_dLastUpdateTime - m_dLastSecond >= 1.0) {
			printf("framerate: %d\n",m_iFramerate);
			m_dLastSecond = m_dLastUpdateTime;
			m_iFramerate = 0;
		}

		// update blobbs' positions
		m_Blobb[0].update();
		m_Blobb[1].update();
		m_Canvas->redisplay();
	}
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