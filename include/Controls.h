#pragma once

#include "BaseApplication.h" //TODO: include only needed files?

//#include "ExampleFrameListener.h"

//class ExampleFrameListener;
class Application;

namespace MyGUI {
	class Gui;
}

class ControlsListener : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::MouseListener, public OIS::KeyListener
{
public:
	ControlsListener(RenderWindow* win, Camera* cam, SceneManager *sceneMgr, Application* app);

	bool frameStarted(const FrameEvent &evt);
    bool frameRenderingQueued(const FrameEvent &evt);
	bool frameEnded(const FrameEvent &evt);

    // MouseListener
    bool mouseMoved(const OIS::MouseEvent &e);
    bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

    // KeyListener
    bool keyPressed(const OIS::KeyEvent &e);
    bool keyReleased(const OIS::KeyEvent &e);

	void showDebugOverlay(bool show);
	void updateStats();

	void windowResized(RenderWindow* rw);
	void windowClosed(RenderWindow* rw);

protected:
	Application* mApp;

	Camera* mCamera;

	RenderWindow* mWindow;

	//OIS Input devices
	OIS::InputManager* mInputManager;
	OIS::Mouse*    mMouse;
	OIS::Keyboard* mKeyboard;

    Real mRotate;          // The rotate constant
    Real mMove;            // The movement constant

    SceneManager *mSceneMgr;   // The current SceneManager
    SceneNode *mCamNode;   // The SceneNode the camera is currently attached to

    bool mContinue;        // Whether to continue rendering or not
    Vector3 mDirection;     // Value to move in the correct direction

	bool mStatsOn;
	Overlay* mDebugOverlay;
	std::string mDebugText;

	int mSceneDetailIndex ; // for switching to wireframe / point rendering

	int mControlBothBlobbs; // to make development easier: control both blobbs with same mouse

	bool mIsPhysicsVisualDebuggerOn;

	MyGUI::Gui * mGUI;
};


