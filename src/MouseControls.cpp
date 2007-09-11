#include "MouseControls.h"
#include <vrs/sg/motionevent.h>
#include <vrs/sg/buttonevent.h>


/**
 * is a callback for processing InputEvent's (MotionEvent, ButtonEvent)
 */
void BV3D::MouseControls::processInput(VRS::SO<VRS::InputEvent> ie) {
	// check for mouse movement
	VRS::SO<VRS::MotionEvent> me = VRS_Cast(VRS::MotionEvent, ie);
	if(me!=NULL) {
		// clamp mouse movement deltas to -30 and 30
		int dx = me->deltaX(); if(dx>30) dx=30; if(dx<-30) dx=-30;
		int dy = -me->deltaY(); if(dy>30) dy=30; if(dy<-30) dy=-30;

		// yield mouse sensitivity
		mRequestedMovement[0] = (double)dx / 10.0;
		mRequestedMovement[2] = (double)dy / 10.0;
	}

	// check if button was pressed or released
	VRS::SO<VRS::ButtonEvent> be = VRS_Cast(VRS::ButtonEvent, ie);
	if(be!=NULL)
		if(be->button()==mJumpButton)
			// signal jump request if jump button was pressed
			mRequestedMovement[1] = (be->pressed()==VRS::ButtonEvent::Pressed) ? 1.0 : 0.0;
}

/**
 * is called by Blobb to check for move requests for the current frame
 * blobbs should move accordingly
 */
VRS::Vector BV3D::MouseControls::getRequestedMovement() {
	VRS::Vector tmpVector = mRequestedMovement;
	mRequestedMovement = VRS::Vector(0.0,tmpVector[1],0.0);
	return tmpVector;
}
