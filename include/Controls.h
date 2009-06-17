#pragma once

#include <OgreWindowEventUtilities.h>
#include <OgreUTFString.h>
#include <OIS\OISMouse.h>
#include <OIS\OISKeyboard.h>
#include <OgreFrameListener.h>
#include <OgreVector3.h>

class Application;
class GUI;

namespace Ogre {
	class RenderWindow;
	class SceneManager;
	class Camera;
}


class ControlsListener : public OIS::MouseListener, public OIS::KeyListener
{
public:
	ControlsListener(Application* app, Ogre::Camera* cam, Ogre::SceneManager *sceneMgr, OIS::Keyboard* keyboard, OIS::Mouse* mouse, GUI* gui);

    // MouseListener
    bool mouseMoved(const OIS::MouseEvent &e);
    bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

    // KeyListener
    bool keyPressed(const OIS::KeyEvent &e);
    bool keyReleased(const OIS::KeyEvent &e);

	Ogre::Vector3 mDirection;     // Value to move in the correct direction
								//TODO!!: mDirection: better name/description//refactor

protected:
	Application* mApp;

	Ogre::Camera* mCamera;

	//OIS Input devices
	OIS::Mouse*    mMouse;
	OIS::Keyboard* mKeyboard;

	GUI* mGUI;

    Ogre::Real mRotate;          // The rotate constant
    Ogre::Real mMove;            // The movement constant

    Ogre::SceneManager *mSceneMgr;   // The current SceneManager
    Ogre::SceneNode *mCamNode;   // The SceneNode the camera is currently attached to

    bool mContinueRendering;        // Whether to continue rendering or not


	bool mStatsOn;

	int mSceneDetailIndex ; // for switching to wireframe / point rendering

	int mControlBothBlobbs; // to make development easier: control both blobbs with same mouse

	bool mIsPhysicsVisualDebuggerOn;

	bool mGuiMode; // when true mouse pointer is displayed and blobbs can't be controlled
};


