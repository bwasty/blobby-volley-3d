/**
 * The Game class represents an instance of the BlobbyVolley3D game.
 * It is responsible for:
 * - creating and managing:
 *   - the obvious game objects: the Arena, the Blobbs, the Ball and the Net
 *   - the canvas(window) to which the scene is rendered
 *   - the root scene node
 *   - the scene camera
 * - hooking in/out the sub-scenes of the game objects
 * - processing and dispatching of input events from the canvas
 * - periodically issue rendering
 */

#include <vrs/time.h>
#include <vrs/sg/clock.h>
#include <vrs/sg/key.h>
#include <vrs/sg/keyevent.h>
#include <vrs/color.h>
#include "Game.h"
#include "MouseControls.h"	// TODO: delete if no longer needed

using namespace BV3D;

Game::Game() {
	m_Canvas = new GlutCanvas("BlobbyVolley3D",600,300);	// create the main window

	m_RootScene = new SceneThing();

	// add transparency support to scene 
	m_TransparencyTechnique = new TransparencyTechniqueGL();
	m_RootScene->append(m_TransparencyTechnique);

	m_Camera = new Camera(Vector(0.0,10.0,-10.0),Vector(0.0,0.0,0.0),30);
	m_RootScene->append(m_Camera);

	// do some global lighting
	m_AmbientLight = new AmbientLight(Color(0.7));
	m_RootScene->append(m_AmbientLight);

	m_Arena = new Arena();
	m_RootScene->append(m_Arena->getScene());

	// init Blobbs and add them to the scene
	m_BlobbArray = new Array<SO<Blobb>>(2);
	SO<Blobb> blobb = new Blobb();
	blobb->setPosition(Vector(-1.0,0.0,1.0));
	m_RootScene->append(blobb->getScene());
	m_BlobbArray->append(blobb);

	blobb = new Blobb();
	blobb->setPosition(Vector(1.0,0.0,1.0));
	blobb->setControls(new MouseControls());
	blobb->setColor(Color(0.0,0.0,1.0,0.4));
	m_RootScene->append(blobb->getScene());
	m_BlobbArray->append(blobb);

	setArenaExtent(Vector(8.0,10.0,4.0));

	m_Canvas->append(m_RootScene);

	// init update callback
	m_FPS = 30.0;	// assuming 30 fps are desired
	m_cbUpdate = new BehaviorCallback();
	m_cbUpdate->setTimeCallback(new MethodCallback<Game>(this,&Game::update));
	m_Canvas->append(m_cbUpdate);

	// init input callback
	m_cbInput = new BehaviorCallback();
	m_cbInput->setCanvasCallback(new MethodCallback<Game>(this,&Game::processInput));
	m_Canvas->append(m_cbInput);
}

Game::~Game() {
	delete[] m_BlobbArray;
}

/**
 * update() is called peridically on timer events to redisplay the whole scene
 */
void Game::update() {
	VRSTime time = m_Canvas->clock()->time();

	// test if current frame's time is over (0.8/m_FPS seems to be a good approximation)
	if(double(time) - m_dLastUpdateTime >= 0.8/m_FPS) {
		m_dLastUpdateTime = double(time);

		// count frames per second
		m_iFramerate++;
		if(m_dLastUpdateTime - m_dLastSecond >= 1.0) {
			printf("framerate: %d\n",m_iFramerate);
			m_dLastSecond = m_dLastUpdateTime;
			m_iFramerate = 0;
		}

		// update blobbs'
		//Iterator it = m_BlobbArray->
		m_BlobbArray->getElement(0)->update();
		m_BlobbArray->getElement(1)->update();
		m_Canvas->redisplay();
	}
}

/**
 * evaluates incoming InputEvents from canvas, processes and dispatches them
 */
void Game::processInput() {
	InputEvent* ie = VRS_Cast(InputEvent, m_cbInput->currentCanvasEvent());

	// process general controls (pausing, camera positioning,...)
	KeyEvent* ke = VRS_Cast(KeyEvent, ie);
	if(ke != NULL)
		if(ke->pressed()) {
			if(ke->keyCode() == Key::Escape)
				exit(0);
			if(ke->keyCode() == Key::Home)
					setArenaExtent(Vector(6.0,2.0,2.0));
		}

	// pass input to blobbs
		m_BlobbArray->getElement(0)->processInput(ie);
	m_BlobbArray->getElement(1)->processInput(ie);
}

/**
 * sets Arena bounds and notifies Blobbs etc
 */
void Game::setArenaExtent(Vector extent) {
	m_Arena->setExtent(extent);
	m_BlobbArray->getElement(0)->setBounds(m_Arena->getTeamBounds(0));
	m_BlobbArray->getElement(1)->setBounds(m_Arena->getTeamBounds(1));
}
