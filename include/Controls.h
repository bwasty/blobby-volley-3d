#pragma once

#include <OgreWindowEventUtilities.h>
#include <OgreUTFString.h>
#include <OIS\OISMouse.h>
#include <OIS\OISKeyboard.h>
#include <OgreFrameListener.h>
#include <OgreVector3.h>

class Application;
class Console;

namespace Ogre {
	class RenderWindow;
	class SceneManager;
	class Camera;
}

namespace MyGUI {
	class Gui;
}


class ControlsListener : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::MouseListener, public OIS::KeyListener
{
public:
	ControlsListener(Ogre::RenderWindow* win, Ogre::Camera* cam, Ogre::SceneManager *sceneMgr, Application* app);

	bool frameStarted(const Ogre::FrameEvent &evt);
    bool frameRenderingQueued(const Ogre::FrameEvent &evt);
	bool frameEnded(const Ogre::FrameEvent &evt);

    // MouseListener
    bool mouseMoved(const OIS::MouseEvent &e);
    bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

    // KeyListener
    bool keyPressed(const OIS::KeyEvent &e);
    bool keyReleased(const OIS::KeyEvent &e);

	void showDebugOverlay(bool show);
	void updateStats();

	void windowResized(Ogre::RenderWindow* rw);
	void windowClosed(Ogre::RenderWindow* rw);

	void consoleCommand(const Ogre::UTFString & _key, const Ogre::UTFString & _value);

protected:
	Application* mApp;

	Ogre::Camera* mCamera;

	Ogre::RenderWindow* mWindow;

	//OIS Input devices
	OIS::InputManager* mInputManager;
	OIS::Mouse*    mMouse;
	OIS::Keyboard* mKeyboard;

    Ogre::Real mRotate;          // The rotate constant
    Ogre::Real mMove;            // The movement constant

    Ogre::SceneManager *mSceneMgr;   // The current SceneManager
    Ogre::SceneNode *mCamNode;   // The SceneNode the camera is currently attached to

    bool mContinue;        // Whether to continue rendering or not
    Ogre::Vector3 mDirection;     // Value to move in the correct direction

	bool mStatsOn;
	Ogre::Overlay* mDebugOverlay;
	std::string mDebugText;

	int mSceneDetailIndex ; // for switching to wireframe / point rendering

	int mControlBothBlobbs; // to make development easier: control both blobbs with same mouse

	bool mIsPhysicsVisualDebuggerOn;

	MyGUI::Gui * mGUI;
	bool mGuiMode; // when true mouse pointer is displayed and blobbs can't be controlled

	Console* mConsole;
};


