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

#include "Game.h"

#include <Newton.h>
#include <fmod.hpp>

#include <vrs/time.h>
#include <vrs/sg/clock.h>
#include <vrs/sg/key.h>
#include <vrs/sg/keyevent.h>
#include <vrs/sg/jumpnavigation.h>
#include <vrs/glut/glutcanvas.h>
#include <vrs/camera.h>
#include <vrs/sg/scenething.h>
#include <vrs/ambientlight.h>
#include <vrs/pointlight.h>
#include <vrs/distantlight.h>
#include <vrs/sg/behaviorcallback.h>
#include <vrs/opengl/transparencytechniquegl.h>
#include <vrs/sg/interactionmode.h>
#include <vrs/sg/interactionconcept.h>
#include <vrs/lookat.h>
#include <vrs/perspective.h>
#include <vrs/opengl/texgengl.h>
#include <vrs/color.h>
#include <vrs/opengl/shadowtechniquegl.h>
#include <vrs/shadowcaster.h>
#include <vrs/shadowed.h>
#include <vrs/sg/selector.h>
#include <gl/glut.h>

// temporarily used for floor
#include <vrs/box.h>




//#include <fmod_errors.h>

#include "MouseControls.h"
#include "KeyboardControls.h"
#include "Ball.h"
//#include "Constants.h"
#include "Arena.h"
#include "Blobb.h"
#include "ClassicReferee.h"
#include "TieBreakReferee.h"
#include "SceneLoader.h"
#include "Menu.h"
#include "Hud.h"


BV3D::Game::Game() {
	m_dLastSecond = 0;
	m_DelayedActionStart = 0;
	m_ScheduleNewServe = false;
	mPrevPosX = 0; mPrevPosY = 0; mPrevWidth = 0; mPrevHeight = 0;
	m_FrameCount = 0;
	isCameraDistant = false;


	m_Canvas = new GlutCanvas("BlobbyVolley3D",600,300);	// create the main window

	m_SceneLoader = new SceneLoader(m_Canvas);

	mScene = new SceneThing();	// create absolute root scene for bv3d

	// add transparency and shadow support to scene
	m_TransparencyTechnique = new TransparencyTechniqueGL();
	mScene->append(m_TransparencyTechnique);
	mScene->append(new VRS::ShadowTechniqueGL());

	m_perspective = new Perspective(45, 1.0, 100.0);
	m_lookAt = new LookAt(BV3D::lookFrom, BV3D::lookTo);
	m_Camera = new Camera(m_perspective, m_lookAt);
	mScene->append(m_Camera);

	// do some global lighting
	m_AmbientLight = new AmbientLight(Color(0.7));
	mScene->append(m_AmbientLight);

	//// add PointLight for shadows and reflection
	/*m_PointLight = new PointLight(Vector(-BV3D::arenaExtent[0]/2, BV3D::arenaExtent[1], BV3D::arenaExtent[2]), VRS::Color(0.3));
	mScene->append(m_PointLight);*/

	// create directional light for shadows
	mTopLight = new VRS::DistantLight(VRS::Vector(0.0,1.0,0.0), VRS::Color(0.5));
	mScene->append(mTopLight);
	
	m_Arena = new BV3D::Arena(this);
	mScene->append(m_Arena->getScene());

	// init Blobbs and add them to the scene
	m_BlobbArray = new Array<SO<Blobb> >();
	mBlobbScenesArray = new Array<SO<SceneThing>>();
	SO<Blobb> blobb = new BV3D::Blobb(m_Arena, BV3D::TEAM1, m_lookAt, false);
	blobb->setPosition(Vector(-2.0,0.0,0.0));
	blobb->getScene()->prepend(new VRS::ShadowCaster(mTopLight));
	mBlobbScenesArray->append(new SceneThing());
	mBlobbScenesArray->getElement(BV3D::TEAM1)->append(blobb->getScene());
	mScene->append(mBlobbScenesArray->getElement(BV3D::TEAM1));
	m_BlobbArray->append(blobb);

	blobb = new BV3D::Blobb(m_Arena, BV3D::TEAM2, m_lookAt, true);
	blobb->setPosition(Vector(2.0,0.0,0.0));
	blobb->setControls(new BV3D::MouseControls());
	blobb->getScene()->prepend(new VRS::ShadowCaster(mTopLight));
	mBlobbScenesArray->append(new SceneThing());
	mBlobbScenesArray->getElement(BV3D::TEAM1)->append(blobb->getScene());
	mScene->append(mBlobbScenesArray->getElement(BV3D::TEAM2));
	m_BlobbArray->append(blobb);

	m_Ball = new BV3D::Ball(m_Arena);
	m_Ball->getScene()->prepend(new VRS::ShadowCaster(mTopLight));
	mScene->append(m_Ball->getScene());

	m_Arena->setupMaterials();
	m_Arena->createAItrigger();

	mHud = new HUD();
	mScene->append(VRS_Cast(VRS::SceneThing, mHud->getScene()));
	m_Canvas->append(mScene);

	mMenu = new Menu(this);
	m_Canvas->append(mMenu->getScene());
	m_Canvas->append(mMenu->getSelector());

	// Init sound
	setupSound();

	// init update callback
	m_FPS = 30.0;	// assuming 30 fps are desired
	m_cbUpdate = new BehaviorCallback();
	m_cbUpdate->setTimeCallback(new MethodCallback<Game>(this,&Game::update));
	//m_cbUpdate->setTimeRequirement(TimeRequirement::infinite);
	//m_cbUpdate->activate();
	m_Canvas->append(m_cbUpdate);
	//m_Canvas->switchOn(m_cbUpdate);
	//m_Canvas->engine()->addPreRenderCallback();

	// init input callback
	m_cbInput = new BehaviorCallback();
	m_cbInput->setCanvasCallback(new MethodCallback<Game>(this,&Game::processInput));
	m_Canvas->append(m_cbInput);

	// init JumpNavigation
	m_Navigation = new JumpNavigation(Vector(0.0, 1.0, 0.0), m_lookAt, 3.0);
	m_Canvas->append(m_Navigation);//m_InteractionConcept);

	//switchToGame(true);
	applyMenuSettings();
	switchToMenu();	// start showing menu
}

BV3D::Game::~Game() {
	delete[] m_BlobbArray;
}

void BV3D::Game::applyMenuSettings() {
	m_BlobbArray->getElement(TEAM1)->setColor(mMenu->getPlayer1Color());
	m_BlobbArray->getElement(TEAM2)->setColor(mMenu->getPlayer2Color());

	VRS::SO<Controls> controls = 0;
	switch(mMenu->getPlayer1Controls()) {
		case Menu::KB_ARROWS:	controls = new KeyboardControls(); break;
		case Menu::KB_WASDQ:	controls = new KeyboardControls(119,115,97,100,113); break;
		case Menu::MOUSE:		controls = new MouseControls(); break;
		case Menu::AI:			controls = 0; break;
		default:				controls = 0; break;
	}
	m_BlobbArray->getElement(TEAM1)->setControls(controls);
	// TODO: [un]install ai here!!!

	controls = 0;
	switch(mMenu->getPlayer2Controls()) {
		case Menu::KB_ARROWS:	controls = new KeyboardControls(); break;
		case Menu::KB_WASDQ:	controls = new KeyboardControls(119,115,97,100,113); break;
		case Menu::MOUSE:		controls = new MouseControls(); break;
		case Menu::AI:			controls = 0; break;
		default:				controls = 0; break;
	}
	m_BlobbArray->getElement(TEAM2)->setControls(controls);
	// TODO: [un]install ai here!!!

	if (mScene->contains(mBackground))
		mScene->remove(mBackground);
	if(mMenu->getPlace()==Menu::BEACH)
		mBackground = m_SceneLoader->loadBeach();
	else if(mMenu->getPlace()==Menu::HEAVEN)
		mBackground = m_SceneLoader->loadHeaven();
	else
		mBackground = m_SceneLoader->loadArena();

	mBackground->prepend(new VRS::Shadowed(mTopLight));
	mScene->append(mBackground);

	if(mMenu->getRules()==Menu::CLASSIC)
		m_Referee = new BV3D::ClassicReferee(this);
	else
		m_Referee = new BV3D::TieBreakReferee(this);
	m_Referee->setHUD(mHud);
	m_Referee->startNewGame();
	m_Arena->setupMaterials();
	newServe();
}

/**
 * update() is called periodically on timer events to redisplay the whole scene
 */
void BV3D::Game::update() {

	//while (true) {

		VRSTime time = m_Canvas->clock()->time();
		//m_iFramerate++;
		// test if current frame's time is over (0.8/m_FPS seems to be a good approximation)
		if(double(time) - m_dLastUpdateTime >= 0.7/m_FPS) {
			float timestep = (double)time - m_dLastUpdateTime;
			m_dLastUpdateTime = double(time);

			// count frames per second
			m_iFramerate++;
			m_FrameCount++;
			if(m_dLastUpdateTime - m_dLastSecond >= 1.0) {
				printf("framerate: %d\n",m_iFramerate);
				m_dLastSecond = m_dLastUpdateTime;
				m_iFramerate = 0;
			}

			if(m_Canvas->isSwitchedOn(mScene))	// simulate world only if root scene is visible
				m_Arena->updateWorld(timestep);


			if (m_DelayedActionStart != 0) {
				if (double(time) - m_DelayedActionStart >= 3.0) {
					newServe();
					m_DelayedActionStart = 0;
				}
			}
			if (m_ScheduleNewServe) {
				m_DelayedActionStart = double(time);
				m_ScheduleNewServe = false;
			}

			//animate blobbs if they're moving
			m_BlobbArray->getElement(BV3D::TEAM1)->updateShape(m_Canvas);
			m_BlobbArray->getElement(BV3D::TEAM2)->updateShape(m_Canvas);

			/*	team = BV3D::TEAM1;
				if (m_BlobbArray->getElement(team)->isMoving())
				{
					mBlobbScenesArray->getElement(team)->clear();
					mBlobbScenesArray->getElement(team)->append(m_BlobbArray->getElement(team)->getNextScene());
				}
				team = BV3D::TEAM2;
				if (m_BlobbArray->getElement(team)->isMoving())
				{
					mBlobbScenesArray->getElement(team)->clear();
					mBlobbScenesArray->getElement(team)->append(m_BlobbArray->getElement(team)->getNextScene());
				}*/

			//m_Canvas->redisplay();
			//m_Canvas->postForRedisplay();
			m_cbUpdate->nextRedraw(BehaviorNode::RedrawWindow);

			m_fmodSystem->update();
			//printf("update\n");
			//m_cbUpdate->deactivate();
		}
		else;
			//printf("frame discarded\n");
	//}
}

/**
 * evaluates incoming InputEvents from canvas, processes and dispatches them
 */
void BV3D::Game::processInput() {
	InputEvent* ie = VRS_Cast(InputEvent, m_cbInput->currentCanvasEvent());
	if(ie==NULL)
		return;

	// process general controls (pausing, camera positioning,...)
	KeyEvent* ke = VRS_Cast(VRS::KeyEvent, ie);
	if(ke != NULL)
		if(ke->pressed()) {
			switch (ke->keyCode())
			{
				case Key::Escape:
					switchToMenu();
					return;
				/*case Key::F1:
					initBackgroundCubeMap();
					break;*/
				case Key::F2:	//view field from the side
					m_Navigation->initPath(BV3D::lookFrom, -BV3D::lookFrom + BV3D::lookTo);
					break;
				case Key::F3:	//view field from above
					m_Navigation->initPath(Vector(0.0, BV3D::lookFrom[1] + 10.0, -0.1), Vector(0.0, -BV3D::lookFrom[1], 0.1) + BV3D::lookTo);
					//printf("Key F2 pressed\n");
					break;
				case Key::F4:	//view field from the front(from baseline of one blobb's field)
					m_Navigation->initPath(Vector(-(BV3D::lookFrom[2] - 2.0), BV3D::lookFrom[1], 0.0), Vector(BV3D::lookFrom[2] - 10.0, -BV3D::lookFrom[1], 0.0) + BV3D::lookTo);
					break;
				case Key::F5:	//view field from the side "lying on the ground"
					m_Navigation->initPath(Vector(0.0, 0.0, BV3D::lookFrom[2]), Vector(0.0, 3.0, -BV3D::lookFrom[2]));
					break;
				case Key::F6:	//view field from +y-axis
					m_Navigation->initPath(Vector(0.0, BV3D::lookFrom[1], -BV3D::lookFrom[2]), Vector(0.0, -BV3D::lookFrom[1], BV3D::lookFrom[2]) + BV3D::lookTo);
					break;
				case Key::F7:	//view field from -x-axis
					m_Navigation->initPath(Vector(BV3D::lookFrom[2] - 5.0, BV3D::lookFrom[1], 0.0), Vector(-(BV3D::lookFrom[2] - 5.0), -BV3D::lookFrom[1], 0.0) + BV3D::lookTo);
					break;
				case Key::F8:	//view field from center in -x direction
					m_Navigation->initPath(Vector(0.0, 0.0, 0.0), Vector(-1.0, 0.5, 0.0));
					break;
				case Key::F9:	//view field from center in -z direction
					m_Navigation->initPath(Vector(0.0, 0.0, 0.0), Vector(0.0, 0.5, -1.0));
					break;
				case Key::F10:	//view field from center in x direction
					m_Navigation->initPath(Vector(0.0, 0.0, 0.0), Vector(1.0, 0.5, 0.0));
					break;
				case Key::F11:	//view field from center in z direction
					m_Navigation->initPath(Vector(0.0, 0.0, 0.0), Vector(0.0, 0.5, 1.0));
					break;
				//case Key::Insert:	//load surroundings
				//	printf("Loading beach...\n");
				//	mScene->append(m_SceneLoader->loadBeach());
			}
			//printf("Key %i pressed\n", ke->keyCode());
		}

	// pass input to blobbs
	m_BlobbArray->getElement(0)->processInput(ie);
	m_BlobbArray->getElement(1)->processInput(ie);

	// warping the pointer freezes the simulation
	/*MotionEvent* me = VRS_Cast(MotionEvent, ie);
	if(me!=NULL)
		glutWarpPointer(m_Canvas->getWidth()/2,m_Canvas->getHeight()/2);*/
}

void BV3D::Game::initBackgroundCubeMap()
{
	/*int selector = 4;
	if (mBackground)
		mScene->remove(mBackground);
	switch (selector)
	{
		case 0:
			mBackground = m_SceneLoader->loadDesertSkybox();
			break;
		case 1:
			mBackground = m_SceneLoader->loadWaterscapeSkybox();
			break;
		case 2:
			mBackground = m_SceneLoader->loadIslandsSkybox();
			break;
		case 3:
			mBackground = m_SceneLoader->loadHotDesertSkybox();
			break;
		case 4:
			mBackground = m_SceneLoader->loadBrightDaySkybox();
			break;
		case 5:
			mBackground = m_SceneLoader->loadZolsky6Skybox();
			break;
	}
	mScene->append(mBackground);*/
}

void BV3D::Game::scheduleNewServe() {
	//m_DelayedActionStart = new VRSTime(VRSTime::now());
	//printf("scheduleNewServe: %f\n", double(*m_DelayedActionStart));
	m_ScheduleNewServe = true;
}

void BV3D::Game::newServe() {
	BV3D::TEAM team = m_Referee->getServingTeam();

	// Reset ball -> setForce into applyForceAndTorque-Callback?
	dFloat nullForce[3] = {0.0f, 0.0f, 0.0f};
	NewtonWorldFreezeBody(m_Arena->getWorld(), m_Ball->getBody());
	NewtonBodySetForce(m_Ball->getBody(), nullForce);
	//NewtonWorldUnfreezeBody(m_Arena->getWorld(), m_Ball->getBody());
	NewtonBodySetTorque(m_Ball->getBody(), nullForce);
	//collData->ball->setLocked(true);
	Vector pos(m_Arena->getTeamBounds(team).center());
	pos[1] = 5;
	m_Ball->resetPosition(pos);

	m_Referee->setActive(true);
	if (getBlobb(team)->isAIcontrolled())
		aiServe(team);
}

void BV3D::Game::aiServe(BV3D::TEAM team) {
	int teamModifier = team==BV3D::TEAM1 ? -1 : 1;
	float random = ((rand() % 13) - 6.0) / 10; // from -0.6 to 0.6
	getBlobb(team)->setPosition(VRS::Vector(teamModifier*(BV3D::arenaExtent[0]/4+1.3+random/6), 0.0,random));
	getBlobb(team)->maxJump();
}

// called by menu
void BV3D::Game::switchToGame(bool restart) {
	applyMenuSettings();

	// deactivate menu
	if(m_Canvas->contains(mMenu->getScene())) m_Canvas->switchOff(mMenu->getScene());
	if(m_Canvas->contains(mMenu->getSelector())) m_Canvas->switchOff(mMenu->getSelector());

	// activate game
	if(m_Canvas->contains(mScene)) m_Canvas->switchOn(mScene);
	if(m_Canvas->contains(m_cbInput)) m_Canvas->switchOn(m_cbInput);
	m_Canvas->setCursor(VRS::Cursor::Blank);
}

void BV3D::Game::switchToMenu() {
	// deactivate game
	if(m_Canvas->contains(mScene)) m_Canvas->switchOff(mScene);
	if(m_Canvas->contains(m_cbInput)) m_Canvas->switchOff(m_cbInput);

	// activate menu (allow resume game if game is not yet over)
	mMenu->showMainMenu(!m_Referee->isGameOver());
	if(m_Canvas->contains(mMenu->getScene())) m_Canvas->switchOn(mMenu->getScene());
	if(m_Canvas->contains(mMenu->getSelector())) m_Canvas->switchOn(mMenu->getSelector());
	m_Canvas->setCursor(VRS::Cursor::Arrow);
}

void BV3D::Game::toggleFullscreen() {
	if(mPrevWidth) {	// reset to windowed mode
		glutPositionWindow(mPrevPosX, mPrevPosY);
		glutReshapeWindow(mPrevWidth, mPrevHeight);
		mPrevPosX = 0; mPrevPosY = 0;
		mPrevWidth = 0; mPrevHeight = 0;
	} else {	// switch to fullscreen mode
		mPrevPosX = glutGet(GLUT_WINDOW_X);
		mPrevPosY = glutGet(GLUT_WINDOW_Y);
		mPrevWidth = glutGet(GLUT_WINDOW_WIDTH);
		mPrevHeight = glutGet(GLUT_WINDOW_HEIGHT);
		glutFullScreen();
	}
}

//void ERRCHECK(FMOD_RESULT result)
//{
//    if (result != FMOD_OK)
//    {
//        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
//        //exit(-1);
//    }
//}


void BV3D::Game::setupSound() {
	FMOD::System_Create(&m_fmodSystem);
	m_fmodSystem->init(32, FMOD_INIT_NORMAL, 0);
	m_fmodSystem->createSound("../Sounds/bums.wav", FMOD_DEFAULT, 0, &soundTouch);
	m_fmodSystem->createSound("../Sounds/pfiff.wav", FMOD_DEFAULT, 0, &soundWhistle);
	soundWhistle->setDefaults(44100, 0.5, 1.0, 128);
}

void BV3D::Game::playSoundTouch() {
	m_fmodSystem->playSound(FMOD_CHANNEL_FREE, soundTouch, false, NULL);
}

void BV3D::Game::playSoundWhistle() {
	m_fmodSystem->playSound(FMOD_CHANNEL_FREE, soundWhistle, false, NULL);
}

void BV3D::Game::switchCameraposition(BV3D::CAMERAPOSITION position, bool distant, bool cinemaMode)
{
	double offset = 0.0;
	if (distant)
		offset = 5.0;
	if (cinemaMode)
	{
		//TODO: stop game and fly camera the long way to destination position
	}
	else
	{
		switch (position)
		{
			case BV3D::CLASSIC:
				m_Navigation->initPath(BV3D::lookFrom + VRS::Vector(0.0, 0.0, -offset), -BV3D::lookFrom + BV3D::lookTo);
				break;
			case BV3D::REVERSE:
				m_Navigation->initPath(Vector(0.0, BV3D::lookFrom[1], -(BV3D::lookFrom[2] - offset) ), Vector(0.0, -BV3D::lookFrom[1], BV3D::lookFrom[2]) + BV3D::lookTo);
				break;
			case BV3D::TEAM1_BASE:
				m_Navigation->initPath(Vector(BV3D::lookFrom[2] - offset, BV3D::lookFrom[1], 0.0), Vector(-BV3D::lookFrom[2], -BV3D::lookFrom[1], 0.0) + BV3D::lookTo);
				break;
			case BV3D::TEAM2_BASE:
				m_Navigation->initPath(Vector(-(BV3D::lookFrom[2] - offset), BV3D::lookFrom[1], 0.0), Vector(BV3D::lookFrom[2], -BV3D::lookFrom[1], 0.0) + BV3D::lookTo);
				break;
		}
	}
}

	//case Key::F2:	//view field from the side
	//				m_Navigation->initPath(BV3D::lookFrom, -BV3D::lookFrom + BV3D::lookTo);
	//				break;
	//			case Key::F3:	//view field from above
	//				m_Navigation->initPath(Vector(0.0, BV3D::lookFrom[1] + 10.0, -0.1), Vector(0.0, -BV3D::lookFrom[1], 0.1) + BV3D::lookTo);
	//				//printf("Key F2 pressed\n");
	//				break;
	//			case Key::F4:	//view field from the front(from baseline of one blobb's field)
	//				m_Navigation->initPath(Vector(-(BV3D::lookFrom[2] - 2.0), BV3D::lookFrom[1], 0.0), Vector(BV3D::lookFrom[2] - 10.0, -BV3D::lookFrom[1], 0.0) + BV3D::lookTo);
	//				break;
	//			case Key::F5:	//view field from the side "lying on the ground"
	//				m_Navigation->initPath(Vector(0.0, 0.0, BV3D::lookFrom[2]), Vector(0.0, 3.0, -BV3D::lookFrom[2]));
	//				break;
	//			case Key::F6:	//view field from +y-axis
	//				m_Navigation->initPath(Vector(0.0, BV3D::lookFrom[1], -BV3D::lookFrom[2]), Vector(0.0, -BV3D::lookFrom[1], BV3D::lookFrom[2]) + BV3D::lookTo);
	//				break;
	//			case Key::F7:	//view field from -x-axis
	//				m_Navigation->initPath(Vector(BV3D::lookFrom[2] - 5.0, BV3D::lookFrom[1], 0.0), Vector(-(BV3D::lookFrom[2] - 5.0), -BV3D::lookFrom[1], 0.0) + BV3D::lookTo);
	//				break;
