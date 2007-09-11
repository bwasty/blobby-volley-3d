
#include "KeyboardControls.h"
#include <vrs/sg/keyevent.h>


/**
 * ctor
 */
BV3D::KeyboardControls::KeyboardControls(int keyForward, int keyBackward, int keyLeft, int keyRight, int keyJump) : Controls() {
	setBinding(FORWARD, keyForward);
	setBinding(BACKWARD, keyBackward);
	setBinding(RIGHT, keyRight);
	setBinding(LEFT, keyLeft);
	setBinding(JUMP, keyJump);

	for(int i=0;i<5;i++) mRequests[i] = false;	// reset requests
}


/**
 * is a callback for processing InputEvent's (KeyEvent)
 */
void BV3D::KeyboardControls::processInput(VRS::SO<VRS::InputEvent> ie) {
	VRS::SO<VRS::KeyEvent> ke = VRS_Cast(VRS::KeyEvent, ie);

	// process KeyEvents only
	if(ke != NULL) {
		for(int i=0;i<5;i++)
			if(ke->keyCode()==mBinding[i])
				mRequests[i] = ke->pressed();	// signal state of request
	}
}


/**
 * is called by Blobb to check for move requests for the current frame
 * blobbs should move accordingly
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
 * map VRS virtual key codes to move requests
 */
void BV3D::KeyboardControls::setBinding(REQUEST req, int keyCode) {
	mBinding[req] = keyCode;
}
