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
#include <vrs/lookat.h>
#include <vrs/perspective.h>
#include <vrs/opengl/texgengl.h>
#include <vrs/color.h>
#include <vrs/sg/selector.h>
#include <gl/glut.h>
#include <vrs/facestyle.h>

#include "MouseControls.h"
#include "KeyboardControls.h"
#include "Ball.h"
#include "Arena.h"
#include "Blobb.h"
#include "ClassicReferee.h"
#include "TieBreakReferee.h"
#include "SceneLoader.h"
#include "Menu.h"
#include "Hud.h"
#include "Ai.h"


BV3D::Game::Game() {
	mDLastSecond = 0;
	mDelayedActionStart = 0;
	mScheduleNewServe = false;
	mPrevPosX = 0; mPrevPosY = 0; mPrevWidth = 0; mPrevHeight = 0;
	mFrameCount = 0;
	mIsCameraDistant = false;
	mUseMovieStyleCamera = false;
	mCurrentCameraPosition = CLASSIC_CAMERA;
	mReferee = 0;

	mCanvas = new GlutCanvas("BlobbyVolley3D",600,300);	// create the main window

	mSceneLoader = new SceneLoader(mCanvas);

	mScene = new SceneThing();	// create absolute root scene for bv3d

	mScene->append(new VRS::FaceStyle(FaceStyle::Filled, FaceStyle::Culled));

	// add transparency and shadow support to scene
	mTransparencyTechnique = new TransparencyTechniqueGL();
	mScene->append(mTransparencyTechnique);

	mPerspective = new Perspective(45, 1.0, 400.0);
	mLookAt = new LookAt(BV3D::LOOK_FROM, BV3D::LOOK_TO);
	mCamera = new Camera(mPerspective, mLookAt);
	mScene->append(mCamera);

	// do some global lighting
	mAmbientLight = new AmbientLight(Color(0.7));
	mScene->append(mAmbientLight);

	//// add PointLight for shadows and reflection
	mPointLight = new PointLight(Vector(-BV3D::ARENA_EXTENT[0]/2, BV3D::ARENA_EXTENT[1], BV3D::ARENA_EXTENT[2]), VRS::Color(0.3));
	mScene->append(mPointLight);

	mArena = new BV3D::Arena(this);
	mArena->setupMaterials();
	mScene->append(mArena->getScene());

	// init Blobbs and add them to the scene
	mBlobbArray = new Array<SO<Blobb> >();
	mBlobbScenesArray = new Array<SO<SceneThing>>();
	SO<Blobb> blobb = new BV3D::Blobb(mArena, BV3D::TEAM1, mLookAt);
	blobb->setPosition(Vector(-BV3D::ARENA_EXTENT[0],0.0,0.0));
	mBlobbScenesArray->append(new SceneThing());
	mBlobbScenesArray->getElement(BV3D::TEAM1)->append(blobb->getScene());
	mScene->append(mBlobbScenesArray->getElement(BV3D::TEAM1));
	mBlobbArray->append(blobb);

	blobb = new BV3D::Blobb(mArena, BV3D::TEAM2, mLookAt);
	blobb->setPosition(Vector(BV3D::ARENA_EXTENT[0],0.0,0.0));
	blobb->setControls(new BV3D::MouseControls());
	mBlobbScenesArray->append(new SceneThing());
	mBlobbScenesArray->getElement(BV3D::TEAM1)->append(blobb->getScene());
	mScene->append(mBlobbScenesArray->getElement(BV3D::TEAM2));
	mBlobbArray->append(blobb);

	mBall = new BV3D::Ball(mArena);
	mScene->append(mBall->getScene());

	mAI = new AI(this);

	mHud = new HUD();
	mScene->append(VRS_Cast(VRS::SceneThing, mHud->getScene()));
	mCanvas->append(mScene);

	mMenu = new Menu(this);
	mCanvas->append(mMenu->getScene());
	mCanvas->append(mMenu->getSelector());

	// Init sound
	setupSound();

	// init update callback
	mFPS = 30.0;	// assuming 30 fps are desired
	mCbUpdate = new BehaviorCallback();
	mCbUpdate->setTimeCallback(new MethodCallback<Game>(this,&Game::update));
	//mCbUpdate->setTimeRequirement(TimeRequirement::infinite);
	//mCbUpdate->activate();
	mCanvas->append(mCbUpdate);
	//mCanvas->switchOn(mCbUpdate);
	//mCanvas->engine()->addPreRenderCallback();

	// init input callback
	mCbInput = new BehaviorCallback();
	mCbInput->setCanvasCallback(new MethodCallback<Game>(this,&Game::processInput));
	mCanvas->append(mCbInput);

	// init JumpNavigation
	mNavigation = new JumpNavigation(Vector(0.0, 1.0, 0.0), mLookAt, 3.0);
	mCanvas->append(mNavigation);//m_InteractionConcept);

	switchToMenu(false);	// start showing menu
}

BV3D::Game::~Game() {
	delete[] mBlobbArray;
}

void BV3D::Game::applyMenuSettings() {
	mBlobbArray->getElement(TEAM1)->setColor(mMenu->getPlayer1Color());
	mBlobbArray->getElement(TEAM2)->setColor(mMenu->getPlayer2Color());

	VRS::SO<Controls> controls = NULL;
	switch(mMenu->getPlayer1Controls()) {
		case Menu::KB_ARROWS:	controls = new KeyboardControls(); break;
		case Menu::KB_WASDQ:	controls = new KeyboardControls(119,115,97,100,113); break;
		case Menu::MOUSE:		controls = new MouseControls(); break;
		case Menu::AI:			controls = 0; mAI->enableAI(BV3D::TEAM1); break;
		default:				controls = 0; break;
	}
	mBlobbArray->getElement(TEAM1)->setControls(controls);
	if (controls != NULL && mAI->isAiControlled(BV3D::TEAM1))
		mAI->disableAI(BV3D::TEAM1);

	controls = NULL;
	switch(mMenu->getPlayer2Controls()) {
		case Menu::KB_ARROWS:	controls = new KeyboardControls(); break;
		case Menu::KB_WASDQ:	controls = new KeyboardControls(119,115,97,100,113); break;
		case Menu::MOUSE:		controls = new MouseControls(); break;
		case Menu::AI:			controls = 0; mAI->enableAI(BV3D::TEAM2); break;
		default:				controls = 0; break;
	}
	mBlobbArray->getElement(TEAM2)->setControls(controls);
	if (controls != NULL && mAI->isAiControlled(BV3D::TEAM2))
		mAI->disableAI(BV3D::TEAM2);

	if (mScene->contains(mBackground))
		mScene->remove(mBackground);
	if(mMenu->getPlace()==Menu::BEACH)
		mBackground = mSceneLoader->loadBeach();
	else if(mMenu->getPlace()==Menu::HEAVEN)
		mBackground = mSceneLoader->loadHeaven();
	else
		mBackground = mSceneLoader->loadArena();

	mScene->append(mBackground);

	VRS::SO<BV3D::Referee> newReferee;
	if(mMenu->getRules()==Menu::CLASSIC)
		newReferee = new BV3D::ClassicReferee(this);
	else
		newReferee = new BV3D::TieBreakReferee(this);

	// copy values of current referee to the new referee
	if(mReferee) {
		newReferee->setCurrentScore(BV3D::TEAM1, mReferee->getCurrentScore(BV3D::TEAM1));
		newReferee->setCurrentScore(BV3D::TEAM2, mReferee->getCurrentScore(BV3D::TEAM2));
		newReferee->setCurrentContacts(BV3D::TEAM1, mReferee->getCurrentContacts(BV3D::TEAM1));
		newReferee->setCurrentContacts(BV3D::TEAM2, mReferee->getCurrentContacts(BV3D::TEAM2));
		newReferee->setServingTeam(mReferee->getServingTeam());
	}
	mReferee = newReferee;
	mReferee->setHUD(mHud);

	// always put HUD at the very end of the scene graph to prevent it from getting covered!
	if(mScene->contains(mHud->getScene()))
		mScene->remove(mHud->getScene());
	mScene->append(mHud->getScene());

	mArena->setupMaterials();
}

/**
 * called periodically on timer events to update the physics world and redisplay the whole scene - main callback
 */
void BV3D::Game::update() {
	VRSTime time = mCanvas->clock()->time();
	// test if current frame's time is over (0.8/mFPS seems to be a good approximation)
	if(double(time) - mDLastUpdateTime >= 0.7/mFPS) {
		float timestep = (double)time - mDLastUpdateTime;
		mDLastUpdateTime = double(time);

		// count frames per second
		mIFramerate++;
		mFrameCount++;
		if(mDLastUpdateTime - mDLastSecond >= 1.0) {
			printf("framerate: %d\n",mIFramerate);
			mDLastSecond = mDLastUpdateTime;
			mIFramerate = 0;
		}

		if(mCanvas->isSwitchedOn(mScene))	// simulate world only if root scene is visible
			mArena->updateWorld(timestep);

		// used to delay a new serve for a few seconds when one player scores
		if (mDelayedActionStart != 0) {
			if (double(time) - mDelayedActionStart >= 3.0) {
				newServe();
				mDelayedActionStart = 0;
			}
		}
		if (mScheduleNewServe) {
			mDelayedActionStart = double(time);
			mScheduleNewServe = false;
		}

		//animate blobbs if they're moving
		mBlobbArray->getElement(BV3D::TEAM1)->updateShape(mCanvas);
		mBlobbArray->getElement(BV3D::TEAM2)->updateShape(mCanvas);

		//mCanvas->redisplay();
		mCbUpdate->nextRedraw(BehaviorNode::RedrawWindow);

		mFmodSystem->update();
	}
}

/**
 * evaluates incoming InputEvents from canvas, processes and dispatches them
 */
void BV3D::Game::processInput() {
	static bool warpingMousePointer = false;	// indicates whether mouse pointer was warped (see below)
	InputEvent* ie = VRS_Cast(InputEvent, mCbInput->currentCanvasEvent());
	if(ie==NULL)
		return;

	// process general controls (pausing, camera positioning,...)
	KeyEvent* ke = VRS_Cast(VRS::KeyEvent, ie);
	if(ke != NULL)
		if(ke->pressed()) {
			switch (ke->keyCode())
			{
				case Key::Escape:
					switchToMenu(!mReferee->isGameOver());
					return;
				case Key::F2:	//change to standard camera position
					switchCameraposition(BV3D::CLASSIC_CAMERA);
					break;
				case Key::F3:	//view field from opposite the standard position
					switchCameraposition(BV3D::REVERSE_CAMERA);
					break;
				case Key::F4:	//view field from baseline of Team1
					switchCameraposition(BV3D::TEAM1_BASECAMERA);
					break;
				case Key::F5:	//view field from baseline of Team2
					switchCameraposition(BV3D::TEAM2_BASECAMERA);
					break;
				case Key::F8:
					mIsCameraDistant = !mIsCameraDistant;
					switchCameraposition(mCurrentCameraPosition);
					break;
				case Key::F9:
					mUseMovieStyleCamera = !mUseMovieStyleCamera;
					break;
			}
		}

	VRS::SO<VRS::MotionEvent> me = VRS_Cast(MotionEvent, ie);

	// mouse pointer warping is finished after motion event with center coordinates is processed
	// that is, if...
	if (me!=NULL &&					// ...the current event is a MotionEvent that is not NULL...
		warpingMousePointer &&						// ...warping was issued...
		(me->x() == mCanvas->getWidth() / 2) &&		// ...the event position is the horizontal...
		(me->y() == mCanvas->getHeight() / 2))		// ...and vertical center of the canvas
	{
		warpingMousePointer = false;
	} else {	// otherwise, the event should be processed regularly by the blobbs
		mBlobbArray->getElement(0)->processInput(ie);
		mBlobbArray->getElement(1)->processInput(ie);
	}

	// warp pointer back to canvas center if pointer leaves some rectangle inside the canvas
	if(me!=NULL &&
		((me->x() > mCanvas->getWidth() * 0.75f) ||
		(me->x() < mCanvas->getWidth() * 0.25f) ||
		(me->y() > mCanvas->getHeight() * 0.75f) ||
		(me->y() < mCanvas->getWidth() * 0.25f))) {
			glutWarpPointer(mCanvas->getWidth() / 2, mCanvas->getHeight() / 2);
			warpingMousePointer = true;
	}
}

/**
 * causes a new serve in about 3 seconds (called by Referee)
 */
void BV3D::Game::scheduleNewServe() {
	mScheduleNewServe = true;
}

void BV3D::Game::newServe() {
	BV3D::TEAM team = mReferee->getServingTeam();

	dFloat nullForce[3] = {0.0f, 0.0f, 0.0f};
	NewtonWorldFreezeBody(mArena->getWorld(), mBall->getBody());
	NewtonBodySetForce(mBall->getBody(), nullForce);
	NewtonBodySetTorque(mBall->getBody(), nullForce);
	Vector pos(mArena->getTeamBounds(team).center());
	pos[1] = 5;
	mBall->resetPosition(pos);

	mReferee->setActive(true);
	if (mAI->isAiControlled(team))
		mAI->aiServe(team);
}

/**
 * called by menu
 */
void BV3D::Game::switchToGame(bool restart) {
	// deactivate menu
	if(mCanvas->contains(mMenu->getScene())) mCanvas->switchOff(mMenu->getScene());
	if(mCanvas->contains(mMenu->getSelector())) mCanvas->switchOff(mMenu->getSelector());

	// adjust settings
	applyMenuSettings();
	if(restart) {
		mReferee->startNewGame();
		getBlobb(BV3D::TEAM1)->setPosition(VRS::Vector(-BV3D::ARENA_EXTENT[0]/4, 0.0f, 0.0f));
		getBlobb(BV3D::TEAM2)->setPosition(VRS::Vector( BV3D::ARENA_EXTENT[0]/4, 0.0f, 0.0f));
		newServe();
	}

	// activate game
	if(mCanvas->contains(mScene)) mCanvas->switchOn(mScene);
	if(mCanvas->contains(mCbInput)) mCanvas->switchOn(mCbInput);
	mCanvas->setCursor(VRS::Cursor::Blank);
}

void BV3D::Game::switchToMenu(bool allowResume) {
	// deactivate game
	if(mCanvas->contains(mScene)) mCanvas->switchOff(mScene);
	if(mCanvas->contains(mCbInput)) mCanvas->switchOff(mCbInput);

	// activate menu (allow resume game if game is not yet over)
	mMenu->showMainMenu(allowResume);
	if(mCanvas->contains(mMenu->getScene())) mCanvas->switchOn(mMenu->getScene());
	if(mCanvas->contains(mMenu->getSelector())) mCanvas->switchOn(mMenu->getSelector());
	mCanvas->setCursor(VRS::Cursor::Arrow);
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

void BV3D::Game::setupSound() {
	FMOD::System_Create(&mFmodSystem);
	mFmodSystem->init(32, FMOD_INIT_NORMAL, 0);
	mFmodSystem->createSound("../Sounds/bums.wav", FMOD_DEFAULT, 0, &soundTouch);
	mFmodSystem->createSound("../Sounds/pfiff.wav", FMOD_DEFAULT, 0, &soundWhistle);
	soundWhistle->setDefaults(44100, 0.5, 1.0, 128);
}

void BV3D::Game::playSoundTouch() {
	mFmodSystem->playSound(FMOD_CHANNEL_FREE, soundTouch, false, NULL);
}

void BV3D::Game::playSoundWhistle() {
	mFmodSystem->playSound(FMOD_CHANNEL_FREE, soundWhistle, false, NULL);
}

void BV3D::Game::switchCameraposition(BV3D::CAMERAPOSITION position)
{
	if (mUseMovieStyleCamera)
	{
		//TODO: pause game
		VRS::SO<VRS::Array<VRS::Vector> > positions = new VRS::Array<VRS::Vector>();
		VRS::SO<VRS::Array<VRS::Vector>	> directions = new VRS::Array<VRS::Vector>();
		positions->clear();
		directions->clear();
		bool moveLeft = true;
		if ( ((mCurrentCameraPosition == CLASSIC_CAMERA) && (position == TEAM1_BASECAMERA)) ||
			 ((mCurrentCameraPosition == TEAM1_BASECAMERA) && (position == REVERSE_CAMERA)) ||
			 ((mCurrentCameraPosition == REVERSE_CAMERA) && (position == TEAM2_BASECAMERA)) ||
			 ((mCurrentCameraPosition == TEAM2_BASECAMERA) && (position == CLASSIC_CAMERA)) )
			moveLeft = false;
		do
		{
			if (moveLeft)
				mCurrentCameraPosition = (CAMERAPOSITION) ((mCurrentCameraPosition + 1) % MAX_CAMERAS);
			else
				mCurrentCameraPosition = (CAMERAPOSITION) ((mCurrentCameraPosition + ((int)MAX_CAMERAS - 1)) % MAX_CAMERAS);
			positions->append(getPositionVector(mCurrentCameraPosition));
			directions->append(getDirectionVector(mCurrentCameraPosition));
		}while(mCurrentCameraPosition != position);

		mNavigation->initMultiStepPath(positions, directions);
	}
	else
	{
		mNavigation->initPath(getPositionVector(position), getDirectionVector(position));
		mCurrentCameraPosition = position;
	}
}

VRS::Vector BV3D::Game::getPositionVector(CAMERAPOSITION position)
{
	double offset = 0.0;
	if (mIsCameraDistant)
		offset = 5.0;
	switch (position)
		{
			case BV3D::CLASSIC_CAMERA:
				return(BV3D::LOOK_FROM + VRS::Vector(0.0, 0.0, -offset));
			case BV3D::REVERSE_CAMERA:
				return(Vector(0.0, BV3D::LOOK_FROM[1], -(BV3D::LOOK_FROM[2] - offset) ));
			case BV3D::TEAM1_BASECAMERA:
				return(Vector(BV3D::LOOK_FROM[2] - offset, BV3D::LOOK_FROM[1], 0.0));
			case BV3D::TEAM2_BASECAMERA:
				return(Vector(-(BV3D::LOOK_FROM[2] - offset), BV3D::LOOK_FROM[1], 0.0));
			default:
				return(BV3D::LOOK_FROM + VRS::Vector(0.0, 0.0, -offset));
	}
}


VRS::Vector BV3D::Game::getDirectionVector(CAMERAPOSITION position)
{
	switch (position)
		{
			case BV3D::CLASSIC_CAMERA:
				return (-BV3D::LOOK_FROM + BV3D::LOOK_TO);
			case BV3D::REVERSE_CAMERA:
				return (VRS::Vector(0.0, -BV3D::LOOK_FROM[1], BV3D::LOOK_FROM[2]) + BV3D::LOOK_TO);
			case BV3D::TEAM1_BASECAMERA:
				return (VRS::Vector(-BV3D::LOOK_FROM[2], -BV3D::LOOK_FROM[1], 0.0) + BV3D::LOOK_TO);
			case BV3D::TEAM2_BASECAMERA:
				return (Vector(BV3D::LOOK_FROM[2], -BV3D::LOOK_FROM[1], 0.0) + BV3D::LOOK_TO);
			default:
				return (-BV3D::LOOK_FROM + BV3D::LOOK_TO);
	}
}
