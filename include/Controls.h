#ifndef BV3D_CONTROLS
#define BV3D_CONTROLS

#include "BaseApplication.h" //TODO: include only needed files?

#include "ExampleFrameListener.h"

class ExampleFrameListener;

//class ExitListener : public FrameListener
//{
//public:
//    ExitListener(OIS::Keyboard *keyboard);
//
//    bool frameStarted(const FrameEvent& evt);
//
//private:
//    OIS::Keyboard *mKeyboard;
//};

class ControlsListener : public ExampleFrameListener, public OIS::MouseListener, public OIS::KeyListener
{
public:
    ControlsListener(RenderWindow* win, Camera* cam, SceneManager *sceneMgr);

    bool frameRenderingQueued(const FrameEvent &evt);

    // MouseListener
    bool mouseMoved(const OIS::MouseEvent &e);
    bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

    // KeyListener
    bool keyPressed(const OIS::KeyEvent &e);
    bool keyReleased(const OIS::KeyEvent &e);
protected:
    Real mRotate;          // The rotate constant
    Real mMove;            // The movement constant

    SceneManager *mSceneMgr;   // The current SceneManager
    SceneNode *mCamNode;   // The SceneNode the camera is currently attached to

    bool mContinue;        // Whether to continue rendering or not
    Vector3 mDirection;     // Value to move in the correct direction
};





#endif //#ifndef BV3D_CONTROLS