#include "Controls.h"

//#include <OgreRenderWindow.h>
//#include <OgreCamera.h>
//#include <OgreSceneManager.h>

//#include <NxOgre.h>
#include <OGRE3DBody.h>

#include "Application.h"
#include "Blobb.h"
#include "Ball.h"
#include "Console.h"
#include "GUI.h"
#include "GameLogic.h"


using namespace Ogre;

// Code originally taken from http://www.ogre3d.org/wiki/index.php/BasicTutorial5Source
ControlsListener::ControlsListener(Application* app, Camera* cam, SceneManager *sceneMgr, OIS::Keyboard* keyboard, OIS::Mouse* mouse, GUI* gui)
	: mCamera(cam), mSceneMgr(sceneMgr), mStatsOn(true), mApp(app), mCurrentCameraMovementPerSecond(Vector3::ZERO),
	mControlBlobb1(true), mIsPhysicsVisualDebuggerOn(false), mGuiMode(false), mContinueRendering(true), 
	mKeyboard(keyboard), mMouse(mouse), mGUI(gui)
{
    // set the rotation and move speed
	// TODO!!: make mMove,mRotate  configurable?
    mCameraRotationPerMouseMovement = 0.10;
    mCameraMovementPerSecond = 100;

    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);
}

// MouseListener
bool ControlsListener::mouseMoved(const OIS::MouseEvent &e)
{
	if (mGuiMode)
		mGUI->getMyGui()->injectMouseMove(e);

    if (e.state.buttonDown(OIS::MB_Right))
    {
		mCamera->yaw(Degree(-mCameraRotationPerMouseMovement * e.state.X.rel));
        mCamera->pitch(Degree(-mCameraRotationPerMouseMovement * e.state.Y.rel));
    }
	else /*if (e.state.buttonDown(OIS::MB_Left))*/ {
		if (!mGuiMode) {
			// move blobb taking into account camera direction
			Vector3 mouseMovement(-e.state.X.rel, 0, -e.state.Y.rel); // ground is xz-plane
			Vector3 camDir = mCamera->getDirection();
			camDir.y = 0; // only interested in xz-plane, not sure if this makes a difference though
			Quaternion rotation = Vector3::UNIT_Z.getRotationTo(camDir); //mouseMovement works correct when camera looks to positive z, so get rotation to that vector 
			mouseMovement = rotation * mouseMovement;

			int factor = mApp->getConfig().getSettingInt("BlobbMovementPerMouseMovement"); // TODO: config value: move to member for efficiency + change function?
			mouseMovement *= factor; // length of vector determines how big the force is (-> how far blobb is moved)
			
			 
			if (mControlBlobb1)
				mApp->getBlobb1()->move(Vector2(mouseMovement.x, mouseMovement.z));
			else
				mApp->getBlobb2()->move(Vector2(mouseMovement.x, mouseMovement.z));
		}
	}
    return true;
}

bool ControlsListener::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	if (mGuiMode)
		mGUI->getMyGui()->injectMousePress(e, id);
	else {
		if (id == OIS::MB_Left) {
			if (mControlBlobb1)
				mApp->getBlobb1()->jump();
			else
				mApp->getBlobb2()->jump();
		}
	}
	return true;
}

bool ControlsListener::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id) { 
	mGUI->getMyGui()->injectMouseRelease(e, id);

	return true; 
}

// KeyListener
bool ControlsListener::keyPressed(const OIS::KeyEvent &e)
{
	bool processed = mGUI->getMyGui()->injectKeyPress(e);
	if (processed) 
		return true;

    switch (e.key)
    {
    case OIS::KC_ESCAPE: 
        mContinueRendering = false;
        break;

    case OIS::KC_UP:
    case OIS::KC_W:
        mCurrentCameraMovementPerSecond.z -= mCameraMovementPerSecond;
        break;

    case OIS::KC_DOWN:
    case OIS::KC_S:
        mCurrentCameraMovementPerSecond.z += mCameraMovementPerSecond;
        break;

    case OIS::KC_LEFT:
    case OIS::KC_A:
        mCurrentCameraMovementPerSecond.x -= mCameraMovementPerSecond;
        break;

    case OIS::KC_RIGHT:
    case OIS::KC_D:
        mCurrentCameraMovementPerSecond.x += mCameraMovementPerSecond;
        break;

    case OIS::KC_PGDOWN:
    case OIS::KC_E:
        mCurrentCameraMovementPerSecond.y -= mCameraMovementPerSecond;
        break;

    case OIS::KC_PGUP:
    case OIS::KC_Q:
        mCurrentCameraMovementPerSecond.y += mCameraMovementPerSecond;
        break;
	case OIS::KC_GRAVE:
		mGUI->getConsole()->setVisible(!mGUI->getConsole()->isVisible());
		break;
	case OIS::KC_RETURN:
		mApp->getGameLogic()->newGame();
		break;
    }
    return true;
}

bool ControlsListener::keyReleased(const OIS::KeyEvent &e)
{
	bool processed = mGUI->getMyGui()->injectKeyRelease(e);
	if (processed) 
		return true;

	Vector3 p;

    switch (e.key)
    {
    case OIS::KC_UP:
    case OIS::KC_W:
        mCurrentCameraMovementPerSecond.z += mCameraMovementPerSecond;
        break;

    case OIS::KC_DOWN:
    case OIS::KC_S:
        mCurrentCameraMovementPerSecond.z -= mCameraMovementPerSecond;
        break;

    case OIS::KC_LEFT:
    case OIS::KC_A:
        mCurrentCameraMovementPerSecond.x += mCameraMovementPerSecond;
        break;

    case OIS::KC_RIGHT:
    case OIS::KC_D:
        mCurrentCameraMovementPerSecond.x -= mCameraMovementPerSecond;
        break;

    case OIS::KC_PGDOWN:
    case OIS::KC_E:
        mCurrentCameraMovementPerSecond.y += mCameraMovementPerSecond;
        break;

    case OIS::KC_PGUP:
    case OIS::KC_Q:
        mCurrentCameraMovementPerSecond.y -= mCameraMovementPerSecond;
        break;
	case OIS::KC_R:
		mSceneDetailIndex = (mSceneDetailIndex+1)%3 ;
		switch(mSceneDetailIndex) {
			case 0 : mCamera->setPolygonMode(PM_SOLID); break;
			case 1 : mCamera->setPolygonMode(PM_WIREFRAME); break;
			case 2 : mCamera->setPolygonMode(PM_POINTS); break;
		}
		break;
	case OIS::KC_F:
		mStatsOn = !mStatsOn;
		mGUI->showDebugOverlay(mStatsOn);
		break;
	case OIS::KC_B:
		mControlBlobb1 = !mControlBlobb1;
		break;
	case OIS::KC_P:
		if (!mIsPhysicsVisualDebuggerOn)
			mApp->getVisualDebugger()->setVisualisationMode(NxOgre::Enums::VisualDebugger_ShowAll);
		else
			mApp->getVisualDebugger()->setVisualisationMode(NxOgre::Enums::VisualDebugger_ShowNone);
		mIsPhysicsVisualDebuggerOn = !mIsPhysicsVisualDebuggerOn;
		break;
	case OIS::KC_1:
		//mApp->getBall()->reset();
		break;		
	case OIS::KC_SPACE:
		mGuiMode ? mGUI->getMyGui()->hidePointer() : mGUI->getMyGui()->showPointer();
		mGuiMode = !mGuiMode;
		mGUI->setDebugText("");
		break;
    } // switch
    return true;
}

