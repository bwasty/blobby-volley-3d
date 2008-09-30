#include "Controls.h"

//ExitListener::ExitListener(OIS::Keyboard *keyboard)
//    : mKeyboard(keyboard) {
//}
//
//bool ExitListener::frameStarted(const FrameEvent& evt) {
//    mKeyboard->capture();
//    return !mKeyboard->isKeyDown(OIS::KC_ESCAPE);
//}


ControlsListener::ControlsListener(RenderWindow* win, Camera* cam, SceneManager *sceneMgr) 
		: ExampleFrameListener(win, cam, false, false) {
	//// Populate the camera and scene manager containers
 //   mCamNode = cam->getParentSceneNode();
 //   mSceneMgr = sceneMgr;

 //   // set the rotation and move speed
 //   mRotate = 0.13;
 //   mMove = 250;

 //   // continue rendering
 //   mContinue = true;

	//mMouse->setEventCallback(this);
 //   mKeyboard->setEventCallback(this);

	//mDirection = Vector3::ZERO;

}

bool ControlsListener::frameRenderingQueued(const FrameEvent &evt) {
 //   if(mMouse)
 //       mMouse->capture();
 //   if(mKeyboard) 
 //       mKeyboard->capture();

	//mCamNode->translate(mDirection * evt.timeSinceLastFrame, Node::TS_LOCAL);

 //   return mContinue;
	return ExampleFrameListener::frameRenderingQueued(evt);
}

// MouseListener
bool ControlsListener::mouseMoved(const OIS::MouseEvent &e) {
	return true;
}

bool ControlsListener::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id) { 
	return true;
}

bool ControlsListener::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id) { 
	return true; 
}

// KeyListener
bool ControlsListener::keyPressed(const OIS::KeyEvent &e) { 
	//switch (e.key)
 //  {
	//   case OIS::KC_ESCAPE: 
	//	   mContinue = false;
	//	   break;
	//   case OIS::KC_UP:
	//   case OIS::KC_W:
	//	   mDirection.z = -mMove;
	//	   break;
	//   case OIS::KC_DOWN:
	//   case OIS::KC_S:
	//	   mDirection.z = mMove;
	//	   break;
	//   case OIS::KC_LEFT:
	//   case OIS::KC_A:
	//	   mDirection.x = -mMove;
	//	   break;
	//   case OIS::KC_RIGHT:
	//   case OIS::KC_D:
	//	   mDirection.x = mMove;
	//	   break;
	//   case OIS::KC_PGDOWN:
	//   case OIS::KC_E:
	//	   mDirection.y = -mMove;
	//	   break;
	//   case OIS::KC_PGUP:
	//   case OIS::KC_Q:
	//	   mDirection.y = mMove;
	//	   break;
	//   default:
	//	   break;
 //  }
 //  return mContinue;

	return true;
}

bool ControlsListener::keyReleased(const OIS::KeyEvent &e) { 
	//switch (e.key)
 //  {
 //  case OIS::KC_UP:
 //  case OIS::KC_W:
 //      mDirection.z = 0;
 //      break;

 //  case OIS::KC_DOWN:
 //  case OIS::KC_S:
 //      mDirection.z = 0;
 //      break;

 //  case OIS::KC_LEFT:
 //  case OIS::KC_A:
 //      mDirection.x = 0;
 //      break;

 //  case OIS::KC_RIGHT:
 //  case OIS::KC_D:
 //      mDirection.x = 0;
 //      break;

 //  case OIS::KC_PGDOWN:
 //  case OIS::KC_E:
 //      mDirection.y = 0;
 //      break;

 //  case OIS::KC_PGUP:
 //  case OIS::KC_Q:
 //      mDirection.y = 0;
 //      break;

 //  default:
 //      break;
 //  } // switch

   return true;
}


