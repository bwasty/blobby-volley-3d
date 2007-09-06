
#include "KeyboardControls.h"
#include <vrs/sg/key.h>
#include <vrs/sg/keyevent.h>


/**
 * ctor
 */
BV3D::KeyboardControls::KeyboardControls() : Controls() {
	setBinding(FORWARD, VRS::Key::Up);
	setBinding(BACKWARD, VRS::Key::Down);
	setBinding(RIGHT ,VRS::Key::Right);
	setBinding(LEFT ,VRS::Key::Left);
	setBinding(JUMP, 32);	// Space

	for(int i=0;i<5;i++) mRequests[i] = false;	// reset requests
}


/**
 * is a callback for processing InputEvent's (KeyEvent)
 */
void BV3D::KeyboardControls::processInput(VRS::SO<VRS::InputEvent> ie) {
	VRS::SO<VRS::KeyEvent> ke = VRS_Cast(VRS::KeyEvent, ie);

	// process KeyEvents only
	if(ke != NULL) { printf("%d\n",ke->keyCode());
		for(int i=0;i<5;i++)
			if(ke->keyCode()==m_Binding[i])
				mRequests[i] = ke->pressed();	// signal state of request
	}
}


/**
 * is called by Blobb to check for move requests for the current frame
 * blobbs should move accordingly
 */
VRS::Vector BV3D::KeyboardControls::getRequestedMovement() {
	VRS::Vector movement;
	if(mRequests[0]) movement += VRS::Vector(0.0,0.0,1.0);
	if(mRequests[1]) movement += VRS::Vector(0.0,0.0,-1.0);
	if(mRequests[2]) movement += VRS::Vector(1.0,0.0,0.0);
	if(mRequests[3]) movement += VRS::Vector(-1.0,0.0,0.0);
	if(mRequests[4]) movement += VRS::Vector(0.0,1.0,0.0);
	return movement;
}


/**
 * map VRS virtual key codes to move requests
 */
void BV3D::KeyboardControls::setBinding(REQUEST req, int keyCode) {
	switch(req) {
		case FORWARD:	m_Binding[0] = keyCode; break;
		case BACKWARD:	m_Binding[1] = keyCode; break;
		case RIGHT:		m_Binding[2] = keyCode; break;
		case LEFT:		m_Binding[3] = keyCode; break;
		case JUMP:		m_Binding[4] = keyCode; break;
		default: break;
	}
}
