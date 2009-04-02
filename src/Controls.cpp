//#include "ExampleApplication.h"
#include "Controls.h"

// Code originally taken from http://www.ogre3d.org/wiki/index.php/BasicTutorial5Source
ControlsListener::ControlsListener(RenderWindow* win, Camera* cam, SceneManager *sceneMgr)
    : ExampleFrameListener(win, cam, true, true)
{
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
    return true;
}

bool ControlsListener::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
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
    } // switch
    return true;
}


