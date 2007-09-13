#include "Game.h"
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

#include <Newton.h>
#include <fmod.hpp>

#include <vrs/time.h>
#include <vrs/sg/clock.h>
#include <vrs/sg/key.h>
#include <vrs/sg/keyevent.h>
#include <vrs/sg/jumpnavigation.h>
#include <vrs/glut/glutcanvas.h>
#include <vrs/camera.h>
#include <vrs/ambientlight.h>
#include <vrs/pointlight.h>
#include <vrs/sg/behaviorcallback.h>
#include <vrs/opengl/transparencytechniquegl.h>
#include <vrs/lookat.h>
#include <vrs/perspective.h>
#include <vrs/facestyle.h>
#include <vrs/sg/pickingcallback.h>




BV3D::Game::Game() {
	mDLastSecond = 0;
	mDelayedActionStart = 0;
	mScheduleNewServe = false;
	mPrevPosX = 0; mPrevPosY = 0; mPrevWidth = 0; mPrevHeight = 0;
	mFrameCount = 0;
	mIsCameraDistant = false;
	mIsCameraHigher = false;
	mUseMovieStyleCamera = false;
	mIsPaused = false;
	mIsCameraAnimating = false;
	mCurrentCameraPosition = BV3D::CLASSIC_CAMERA;
	mReferee = 0;

	mCanvas = new VRS::GlutCanvas("BlobbyVolley3D",600,300);	// create the main window

	mSceneLoader = new SceneLoader(mCanvas);

	mScene = new VRS::SceneThing();	// create absolute root scene for bv3d

	mScene->append(new VRS::FaceStyle(VRS::FaceStyle::Filled, VRS::FaceStyle::Culled));

	// add transparency support to scene
	mTransparencyTechnique = new VRS::TransparencyTechniqueGL();
	mScene->append(mTransparencyTechnique);

	mPerspective = new VRS::Perspective(45, 1.0, 400.0);
	mLookAt = new VRS::LookAt(BV3D::LOOK_FROM, BV3D::LOOK_TO);
	mCamera = new VRS::Camera(mPerspective, mLookAt);
	mScene->append(mCamera);

	// do some global lighting
	mAmbientLight = new VRS::AmbientLight(VRS::Color(0.7));
	mScene->append(mAmbientLight);

	// add PointLight for shadows and reflection
	mPointLight = new VRS::PointLight(VRS::Vector(-BV3D::ARENA_EXTENT[0]/2, BV3D::ARENA_EXTENT[1]/2, -BV3D::ARENA_EXTENT[2]/2), VRS::Color(0.7));
	mScene->append(mPointLight);

	mPointLight = new VRS::PointLight(VRS::Vector(BV3D::ARENA_EXTENT[0]/2, BV3D::ARENA_EXTENT[1]/4, -BV3D::ARENA_EXTENT[2]/2), VRS::Color(0.45));
	mScene->append(mPointLight);

	mArena = new BV3D::Arena(this);
	mArena->setupMaterials();
	mScene->append(mArena->getScene());

	// init Blobbs and add them to the scene
	mBlobbArray = new VRS::Array<VRS::SO<BV3D::Blobb> >();
	VRS::SO<BV3D::Blobb> blobb = new BV3D::Blobb(mArena, BV3D::TEAM1, mLookAt);
	blobb->setPosition(VRS::Vector(-BV3D::ARENA_EXTENT[0],0.0,0.0));
	mScene->append(blobb->getScene());
	mBlobbArray->append(blobb);

	blobb = new BV3D::Blobb(mArena, BV3D::TEAM2, mLookAt);
	blobb->setPosition(VRS::Vector(BV3D::ARENA_EXTENT[0],0.0,0.0));
	blobb->setControls(new BV3D::MouseControls());
	mScene->append(blobb->getScene());
	mBlobbArray->append(blobb);

	mBall = new BV3D::Ball(mArena);
	mScene->append(mBall->getScene());

	mAI = new BV3D::AI(this);

	mHud = new BV3D::HUD();
	mScene->append(VRS_Cast(VRS::SceneThing, mHud->getScene()));
	mCanvas->append(mScene);

	mMenu = new BV3D::Menu(this);
	mCanvas->append(mMenu->getScene());
	mCanvas->append(mMenu->getPickingCallback());

	// Init sound
	setupSound();

	// init update callback
	mCbUpdate = new VRS::BehaviorCallback();
	mCbUpdate->setTimeCallback(new VRS::MethodCallback<BV3D::Game>(this,&BV3D::Game::update));
	mCanvas->append(mCbUpdate);

	// init input callback
	mCbInput = new VRS::BehaviorCallback();
	mCbInput->setCanvasCallback(new VRS::MethodCallback<BV3D::Game>(this,&BV3D::Game::processInput));
	mCanvas->append(mCbInput);

	// init JumpNavigation
	mNavigation = new VRS::JumpNavigation(VRS::Vector(0.0, 1.0, 0.0), mLookAt, 3.0);
	mCanvas->append(mNavigation);//m_InteractionConcept);

	switchToMenu(false);	// start showing menu
}

BV3D::Game::~Game() {
	delete[] mBlobbArray;
}

void BV3D::Game::applyMenuSettings() {
	mBlobbArray->getElement(TEAM1)->setColor(mMenu->getColor(TEAM1));
	mBlobbArray->getElement(TEAM2)->setColor(mMenu->getColor(TEAM2));

	VRS::SO<BV3D::Controls> controls = NULL;
	switch(mMenu->getControls(TEAM1)) {
		case BV3D::Menu::KB_ARROWS:	controls = new BV3D::KeyboardControls(); break;
		case BV3D::Menu::KB_WASDQ:	controls = new BV3D::KeyboardControls(119,115,97,100,113); break;
		case BV3D::Menu::MOUSE:		controls = new BV3D::MouseControls(); break;
		case BV3D::Menu::AI:		controls = 0; mAI->enableAI(BV3D::TEAM1); break;
		default:					controls = 0; break;
	}
	mBlobbArray->getElement(BV3D::TEAM1)->setControls(controls);
	if (controls != NULL && mAI->isAiControlled(BV3D::TEAM1))
		mAI->disableAI(BV3D::TEAM1);

	controls = NULL;
	switch(mMenu->getControls(TEAM2)) {
		case BV3D::Menu::KB_ARROWS:	controls = new BV3D::KeyboardControls(); break;
		case BV3D::Menu::KB_WASDQ:	controls = new BV3D::KeyboardControls(119,115,97,100,113); break;
		case BV3D::Menu::MOUSE:		controls = new BV3D::MouseControls(); break;
		case BV3D::Menu::AI:		controls = 0; mAI->enableAI(BV3D::TEAM2); break;
		default:					controls = 0; break;
	}
	mBlobbArray->getElement(TEAM2)->setControls(controls);
	if (controls != NULL && mAI->isAiControlled(BV3D::TEAM2))
		mAI->disableAI(BV3D::TEAM2);

	if (mScene->contains(mBackground))
		mScene->remove(mBackground);
	if(mMenu->getPlace()== BV3D::Menu::BEACH)
		mBackground = mSceneLoader->loadBeach();
	else if(mMenu->getPlace()== BV3D::Menu::HEAVEN)
		mBackground = mSceneLoader->loadHeaven();
	else if(mMenu->getPlace() == BV3D::CITY)
		mBackground = mSceneLoader->loadCity();
	else
		mBackground = mSceneLoader->loadArena();

	mScene->append(mBackground);

	VRS::SO<BV3D::Referee> newReferee;
	if(mMenu->getRules()== BV3D::Menu::CLASSIC)
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

	// store correct team colors in HUD
	mHud->setColors(mMenu->getColor(TEAM1), mMenu->getColor(TEAM2));
	mReferee->setHUD(mHud);	// associate HUD with referee and refresh HUD

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
	VRS::VRSTime time = mCanvas->clock()->time();
	// test if current frame's time is over (0.7/MAX_FPS seems to be a good approximation)
	if(double(time) - mDLastUpdateTime >= 0.7/BV3D::MAX_FPS) {
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
		//skip physics update if game is paused, currently in menu or a camera animation is active in movie-style mode
		if(mCanvas->isSwitchedOn(mScene) && (!isPaused()) && (!(mNavigation->pathActive() && isCameraMovieStyle()))){	// simulate world only if root scene is visible
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
		}
		//mCanvas->redisplay();
		mCbUpdate->nextRedraw(VRS::BehaviorNode::RedrawWindow);

		mFmodSystem->update();
	}
}

/**
 * evaluates incoming InputEvents from canvas, processes and dispatches them
 */
void BV3D::Game::processInput() {
	static bool warpingMousePointer = false;	// indicates whether mouse pointer was warped (see below)
	VRS::InputEvent* ie = VRS_Cast(VRS::InputEvent, mCbInput->currentCanvasEvent());
	if(ie==NULL)
		return;

	// process general controls (pausing, camera positioning,...)
	VRS::KeyEvent* ke = VRS_Cast(VRS::KeyEvent, ie);
	if(ke != NULL)
		if(ke->pressed()) {
			switch (ke->keyCode()) {
				case VRS::Key::Escape:
					switchToMenu(!mReferee->isGameOver());
					return;
				case VRS::Key::F2:	//change to standard camera position
					switchCameraposition(BV3D::CLASSIC_CAMERA);
					break;
				case VRS::Key::F3:	//view field from opposite the standard position
					switchCameraposition(BV3D::REVERSE_CAMERA);
					break;
				case VRS::Key::F4:	//view field from baseline of Team1
					switchCameraposition(BV3D::TEAM1_BASECAMERA);
					break;
				case VRS::Key::F5:	//view field from baseline of Team2
					switchCameraposition(BV3D::TEAM2_BASECAMERA);
					break;
				case VRS::Key::F8:
					setCameraDistant(!isCameraDistant());
					switchCameraposition(mCurrentCameraPosition);
					break;
				case VRS::Key::F9:
					setMovieStyleCamera(!isCameraMovieStyle());
					if (isCameraMovieStyle())
						mNavigation->setDuration(2.0);
					else
						mNavigation->setDuration(3.0);
					switchCameraposition(mCurrentCameraPosition);
					break;
				case VRS::Key::F7:
					setCameraHigher(!isCameraHigher());
					switchCameraposition(mCurrentCameraPosition);
					break;
				case 112:	//'p' key
					setPaused(!isPaused());
					break;
			}
		}

		VRS::SO<VRS::MotionEvent> me = VRS_Cast(VRS::MotionEvent, ie);

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
	VRS::Vector pos(mArena->getTeamBounds(team).center());
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
	if(mCanvas->contains(mMenu->getPickingCallback())) mCanvas->switchOff(mMenu->getPickingCallback());

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
	if(mCanvas->contains(mMenu->getPickingCallback())) mCanvas->switchOn(mMenu->getPickingCallback());
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
	std::string file = BV3D::SOUNDS_PATH;
	FMOD::System_Create(&mFmodSystem);
	mFmodSystem->init(32, FMOD_INIT_NORMAL, 0);
	file.append("bums.wav");
	mFmodSystem->createSound(file.c_str(), FMOD_DEFAULT, 0, &soundTouch);
	file = BV3D::SOUNDS_PATH + "pfiff.wav";
	mFmodSystem->createSound(file.c_str(), FMOD_DEFAULT, 0, &soundWhistle);
	soundWhistle->setDefaults(44100, 0.5, 1.0, 128);
}

void BV3D::Game::playSoundTouch() {
	mFmodSystem->playSound(FMOD_CHANNEL_FREE, soundTouch, false, NULL);
}

void BV3D::Game::playSoundWhistle() {
	mFmodSystem->playSound(FMOD_CHANNEL_FREE, soundWhistle, false, NULL);
}

/**
 * Initializes a camera animation to the specified cameraposition.
 * Animation style depends on weather setMovieStyleCamera is set to true.
 * In movie-style mode every time the camera moves, the game is paused, 
 * and the camera always takes the long way to a new camera position
 */
void BV3D::Game::switchCameraposition(BV3D::CAMERAPOSITION position) {
	if (isCameraMovieStyle()){
		VRS::SO<VRS::Array<VRS::Vector> > positions = new VRS::Array<VRS::Vector>();
		VRS::SO<VRS::Array<VRS::Vector>	> directions = new VRS::Array<VRS::Vector>();
		positions->clear();
		directions->clear();
		bool moveLeft = true;
		if ( ((mCurrentCameraPosition == BV3D::CLASSIC_CAMERA) && (position == BV3D::TEAM1_BASECAMERA)) ||
			 ((mCurrentCameraPosition == BV3D::TEAM1_BASECAMERA) && (position == BV3D::REVERSE_CAMERA)) ||
			 ((mCurrentCameraPosition == BV3D::REVERSE_CAMERA) && (position == BV3D::TEAM2_BASECAMERA)) ||
			 ((mCurrentCameraPosition == BV3D::TEAM2_BASECAMERA) && (position == BV3D::CLASSIC_CAMERA)) )
			moveLeft = false;
		do{
			if (moveLeft)
				mCurrentCameraPosition = (BV3D::CAMERAPOSITION) ((mCurrentCameraPosition + 1) % BV3D::MAX_CAMERAS);
			else
				mCurrentCameraPosition = (BV3D::CAMERAPOSITION) ((mCurrentCameraPosition + ((int)BV3D::MAX_CAMERAS - 1)) % BV3D::MAX_CAMERAS);
			positions->append(getPositionVector(mCurrentCameraPosition));
			directions->append(getDirectionVector(mCurrentCameraPosition));
		}while(mCurrentCameraPosition != position);

		mNavigation->initMultiStepPath(positions, directions);
	}
	else{
		mNavigation->initPath(getPositionVector(position), getDirectionVector(position));
		mCurrentCameraPosition = position;
	}
}

/**
 * Returns the position vector of the given camera position, modified  
 * in case setCameraDistant and/or setCameraHigher were set to true.
 */
VRS::Vector BV3D::Game::getPositionVector(BV3D::CAMERAPOSITION position) {
	double distanceOffset = 0.0;
	double heightOffset = 0.0;
	if (isCameraDistant())
		distanceOffset = 5.0;
	if (isCameraHigher())
		heightOffset = 5.0;
	switch (position){
			case BV3D::CLASSIC_CAMERA:
				return(BV3D::LOOK_FROM + VRS::Vector(0.0, heightOffset, -distanceOffset));
			case BV3D::REVERSE_CAMERA:
				return(VRS::Vector(0.0, BV3D::LOOK_FROM[1] + heightOffset, -(BV3D::LOOK_FROM[2] - distanceOffset) ));
			case BV3D::TEAM1_BASECAMERA:
				return(VRS::Vector(BV3D::LOOK_FROM[2] - distanceOffset - 2.0, BV3D::LOOK_FROM[1] + heightOffset, 0.0));
			case BV3D::TEAM2_BASECAMERA:
				return(VRS::Vector(-(BV3D::LOOK_FROM[2] - distanceOffset - 2.0), BV3D::LOOK_FROM[1] + heightOffset, 0.0));
			default:
				return(BV3D::LOOK_FROM + VRS::Vector(0.0, heightOffset, -distanceOffset));
	}
}

/**
 * Returns the direction vector of the given camera position, modified 
 * in case setCameraHigher was set to true.
 */
VRS::Vector BV3D::Game::getDirectionVector(BV3D::CAMERAPOSITION position) {
	double heightOffset = 0.0;
	if (isCameraHigher())
		heightOffset = 4.0;
	switch (position){
			case BV3D::CLASSIC_CAMERA:
				return (-BV3D::LOOK_FROM + BV3D::LOOK_TO + VRS::Vector(0.0, -heightOffset, 0.0));
			case BV3D::REVERSE_CAMERA:
				return (VRS::Vector(0.0, -BV3D::LOOK_FROM[1] - heightOffset, BV3D::LOOK_FROM[2]) + BV3D::LOOK_TO);
			case BV3D::TEAM1_BASECAMERA:
				return (VRS::Vector(-BV3D::LOOK_FROM[2], -BV3D::LOOK_FROM[1] - heightOffset, 0.0) + BV3D::LOOK_TO);
			case BV3D::TEAM2_BASECAMERA:
				return (VRS::Vector(BV3D::LOOK_FROM[2], -BV3D::LOOK_FROM[1] - heightOffset, 0.0) + BV3D::LOOK_TO);
			default:
				return (-BV3D::LOOK_FROM + BV3D::LOOK_TO);
	}
}