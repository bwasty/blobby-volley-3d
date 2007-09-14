#include "KeyboardControls.h"
#include <vrs/sg/keyevent.h>

BV3D::KeyboardControls::KeyboardControls(int keyForward, int keyBackward, int keyLeft, int keyRight, int keyJump) : Controls() {
	// bind keys to movement requests
	setBinding(FORWARD, keyForward);
	setBinding(BACKWARD, keyBackward);
	setBinding(RIGHT, keyRight);
	setBinding(LEFT, keyLeft);
	setBinding(JUMP, keyJump);

	// clear requests bit-field
	for(int i=0;i<5;i++) mRequests[i] = false;
}


/**
 * is a callback for processing InputEvents (KeyEvent)
 * \param ie is the VRS::InputEvent to be processed
 */
void BV3D::KeyboardControls::processInput(VRS::SO<VRS::InputEvent> ie) {
	VRS::SO<VRS::KeyEvent> ke = VRS_Cast(VRS::KeyEvent, ie);

	if(ke != NULL)				// process KeyEvents only
		for(int i=0;i<5;i++)
			if(ke->keyCode()==mBinding[i])		// if key code corresponds to mapped reuqest...
				mRequests[i] = ke->pressed();	// ...store state of request according to key state (pressed/released)
}


/**
 * is called by Blobb to check for move requests for the current frame
 * Blobbs should move accordingly
 * Blobbs should call this function only once per frame!
 * \result a VRS::Vector is returned which represents the movement requests of the user
 */
VRS::Vector BV3D::KeyboardControls::getRequestedMovement() {
	VRS::Vector movement;
	if(mRequests[FORWARD])	movement += VRS::Vector(0.0,0.0,1.0);
	if(mRequests[BACKWARD])	movement += VRS::Vector(0.0,0.0,-1.0);
	if(mRequests[LEFT])		movement += VRS::Vector(-1.0,0.0,0.0);
	if(mRequests[RIGHT])	movement += VRS::Vector(1.0,0.0,0.0);
	if(mRequests[JUMP])		movement += VRS::Vector(0.0,1.0,0.0);
	return movement;
}


/**
 * map VRS key codes to move requests
 * \param request is one of the movement requests in BV3D::Controls::REQUEST
 * \param keyCode is the VRS key code to be associated with the movement request
 */
void BV3D::KeyboardControls::setBinding(REQUEST request, int keyCode) {
	mBinding[request] = keyCode;
}
