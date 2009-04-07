//#include "ExampleApplication.h"
#include "Controls.h"
#include "Application.h"
#include "Blobb.h"

// Code originally taken from http://www.ogre3d.org/wiki/index.php/BasicTutorial5Source
//TODO!: copy all useful stuff from ExampleFrameListener (keys, stats, overlay, constructor, member variables)
ControlsListener::ControlsListener(RenderWindow* win, Camera* cam, SceneManager *sceneMgr, Application* app)
    : mCamera(cam), mWindow(win), mStatsOn(true), mApp(app), mControlBothBlobbs(false), mIsPhysicsVisualDebuggerOn(false)
{
	//TODO!: copy needed parts from ExampleFrameListener constructor
	using namespace OIS;

	mDebugOverlay = OverlayManager::getSingleton().getByName("Core/DebugOverlay");
	mDebugOverlay->remove2D(mDebugOverlay->getChild("Core/LogoPanel"));

	LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
	ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	win->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	mInputManager = InputManager::createInputSystem( pl );

	//Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
	mKeyboard = static_cast<Keyboard*>(mInputManager->createInputObject( OISKeyboard, true ));
	mMouse = static_cast<Mouse*>(mInputManager->createInputObject( OISMouse, true ));

	//Set initial mouse clipping size
	windowResized(mWindow);

	showDebugOverlay(true);

	//Register as a Window listener
	WindowEventUtilities::addWindowEventListener(mWindow, this);

	// end copy from ExampleFrameListener


    // Populate the camera and scene manager containers
    //mCamNode = cam->getParentSceneNode();
    mSceneMgr = sceneMgr;

    // set the rotation and move speed
    mRotate = 0.10;
    mMove = 10;

    // continue rendering
    mContinue = true;

    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);

    mDirection = Vector3::ZERO;
}

bool ControlsListener::frameRenderingQueued(const FrameEvent &evt)
{
    if(mMouse)
        mMouse->capture();
    if(mKeyboard) 
        mKeyboard->capture();

    //mCamNode->translate(mDirection * evt.timeSinceLastFrame, Node::TS_LOCAL);
	mCamera->moveRelative(mDirection * evt.timeSinceLastFrame); // TODO: does it the same as above?
    return mContinue;
}

bool ControlsListener::frameEnded(const Ogre::FrameEvent &evt) {
	updateStats();
	return true;
}

// MouseListener
bool ControlsListener::mouseMoved(const OIS::MouseEvent &e)
{
    if (e.state.buttonDown(OIS::MB_Right))
    {
        //mCamNode->yaw(Degree(-mRotate * e.state.X.rel), Node::TS_WORLD);
        //mCamNode->pitch(Degree(-mRotate * e.state.Y.rel), Node::TS_LOCAL);
		mCamera->yaw(Degree(-mRotate * e.state.X.rel));
        mCamera->pitch(Degree(-mRotate * e.state.Y.rel));
    }
	else /*if (e.state.buttonDown(OIS::MB_Left))*/ {
		// move blobb taking into account camera direction
		Vector3 mouseMovement(-e.state.X.rel, 0, -e.state.Y.rel); // ground is xz-plane
		Vector3 camDir = mCamera->getDirection();
		camDir.y = 0; // only interested in xz-plane, not sure if this makes a difference though
		Quaternion rotation = Vector3::UNIT_Z.getRotationTo(camDir); //mouseMovement works correct when camera looks to positive z, so get rotation to that vector 
		mouseMovement = rotation * mouseMovement;
		mouseMovement *= 15; // length of vector determines how big the force is (-> how far blobb is moved)
		
		mApp->getBlobb1()->move(Vector2(mouseMovement.x, mouseMovement.z)); //TODO: signature of Blobb->move()
		if (mControlBothBlobbs)
			mApp->getBlobb2()->move(Vector2(mouseMovement.x, mouseMovement.z));
	}
    return true;
}

bool ControlsListener::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	if (id == OIS::MB_Left) {
		mApp->getBlobb1()->jump(2400);
		if (mControlBothBlobbs)
			mApp->getBlobb2()->jump(2400);
	}
	return true;
}

bool ControlsListener::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id) { return true; }

// KeyListener
bool ControlsListener::keyPressed(const OIS::KeyEvent &e)
{
    switch (e.key)
    {
    case OIS::KC_ESCAPE: 
        mContinue = false;
        break;

    case OIS::KC_UP:
    case OIS::KC_W:
        mDirection.z -= mMove;
        break;

    case OIS::KC_DOWN:
    case OIS::KC_S:
        mDirection.z += mMove;
        break;

    case OIS::KC_LEFT:
    case OIS::KC_A:
        mDirection.x -= mMove;
        break;

    case OIS::KC_RIGHT:
    case OIS::KC_D:
        mDirection.x += mMove;
        break;

    case OIS::KC_PGDOWN:
    case OIS::KC_E:
        mDirection.y -= mMove;
        break;

    case OIS::KC_PGUP:
    case OIS::KC_Q:
        mDirection.y += mMove;
        break;
    }
    return true;
}

bool ControlsListener::keyReleased(const OIS::KeyEvent &e)
{
    switch (e.key)
    {
    case OIS::KC_UP:
    case OIS::KC_W:
        mDirection.z += mMove;
        break;

    case OIS::KC_DOWN:
    case OIS::KC_S:
        mDirection.z -= mMove;
        break;

    case OIS::KC_LEFT:
    case OIS::KC_A:
        mDirection.x += mMove;
        break;

    case OIS::KC_RIGHT:
    case OIS::KC_D:
        mDirection.x -= mMove;
        break;

    case OIS::KC_PGDOWN:
    case OIS::KC_E:
        mDirection.y += mMove;
        break;

    case OIS::KC_PGUP:
    case OIS::KC_Q:
        mDirection.y -= mMove;
        break;
	case OIS::KC_R:
		mSceneDetailIndex = (mSceneDetailIndex+1)%3 ;
		switch(mSceneDetailIndex) {
			case 0 : mCamera->setPolygonMode(PM_SOLID); break;
			case 1 : mCamera->setPolygonMode(PM_WIREFRAME); break;
			case 2 : mCamera->setPolygonMode(PM_POINTS); break;
		}
	case OIS::KC_F:
		mStatsOn = !mStatsOn;
		showDebugOverlay(mStatsOn);
		break;
	case OIS::KC_B:
		mControlBothBlobbs = !mControlBothBlobbs;
		break;
	case OIS::KC_P:
		if (!mIsPhysicsVisualDebuggerOn)
			mApp->getNxWorld()->createDebugRenderer(mSceneMgr);
		else
			mApp->getNxWorld()->destroyDebugRenderer();
		mIsPhysicsVisualDebuggerOn = !mIsPhysicsVisualDebuggerOn;
		break;
    } // switch
    return true;
}


void ControlsListener::updateStats(void)
{
	static String currFps = "Current FPS: ";
	static String avgFps = "Average FPS: ";
	static String bestFps = "Best FPS: ";
	static String worstFps = "Worst FPS: ";
	static String tris = "Triangle Count: ";
	static String batches = "Batch Count: ";

	// update stats when necessary
	try {
		OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
		OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
		OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
		OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

		const RenderTarget::FrameStats& stats = mWindow->getStatistics();
		guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
		guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
		guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS)
			+" "+StringConverter::toString(stats.bestFrameTime)+" ms");
		guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS)
			+" "+StringConverter::toString(stats.worstFrameTime)+" ms");

		OverlayElement* guiTris = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
		guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));

		OverlayElement* guiBatches = OverlayManager::getSingleton().getOverlayElement("Core/NumBatches");
		guiBatches->setCaption(batches + StringConverter::toString(stats.batchCount));

		OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
		guiDbg->setCaption(mDebugText);
	}
	catch(...) { /* ignore */ }
}


//Adjust mouse clipping area
void ControlsListener::windowResized(RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void ControlsListener::windowClosed(RenderWindow* rw)
{
	//Only close for window that created OIS (the main window in these demos)
	if( rw == mWindow )
	{
		if( mInputManager )
		{
			mInputManager->destroyInputObject( mMouse );
			mInputManager->destroyInputObject( mKeyboard );

			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = 0;
		}
	}
}

void ControlsListener::showDebugOverlay(bool show)
{
	if (mDebugOverlay)
	{
		if (show)
			mDebugOverlay->show();
		else
			mDebugOverlay->hide();
	}
}