#include "MouseControls.h"
#include "Constants.h"
#include <vrs/sg/motionevent.h>
#include <vrs/sg/buttonevent.h>


/**
 * is a callback for processing InputEvents (MotionEvent, ButtonEvent)
 * \param ie is the VRS::InputEvent to be processed
 */
void BV3D::MouseControls::processInput(VRS::SO<VRS::InputEvent> ie) {
	// check for mouse movement
	VRS::SO<VRS::MotionEvent> me = VRS_Cast(VRS::MotionEvent, ie);
	if(me!=NULL) {
		// clamp mouse movement deltas to +/-MOUSE_MAX_DELTA
		// That is, if mouse moves more than MOUSE_MAX_DELTA pixels in one direction
		// it will be treated like having moved only MOUSE_MAX_DELTA pixels.
		int dx = me->deltaX();
		if(dx>MOUSE_MAX_DELTA)
			dx=MOUSE_MAX_DELTA;
		else if(dx<-MOUSE_MAX_DELTA)
			dx=-MOUSE_MAX_DELTA;

		int dy = -me->deltaY();
		if(dy>MOUSE_MAX_DELTA)
			dy=MOUSE_MAX_DELTA;
		else if(dy<-MOUSE_MAX_DELTA)
			dy=-MOUSE_MAX_DELTA;

		// yield mouse sensitivity
		// intensity of movement request is 1/10 of the mouse movement deltas
		// this realizes mouse sensitve Blobb movement
		mRequestedMovement[0] = (double)dx / 10.0;
		mRequestedMovement[2] = (double)dy / 10.0;
	}

	// check if button was pressed or released
	VRS::SO<VRS::ButtonEvent> be = VRS_Cast(VRS::ButtonEvent, ie);
	if(be!=NULL)
		if(be->button()==mJumpButton)
			// indicate jump request only if jump button was pressed
			mRequestedMovement[1] = (be->pressed()==VRS::ButtonEvent::Pressed) ? 1.0 : 0.0;
}


/**
 * is called by Blobb to check for move requests for the current frame
 * Blobbs should move accordingly
 * Blobbs should call this function only once per frame!
 * \result a VRS::Vector is returned which represents the movement requests of the user
 */
VRS::Vector BV3D::MouseControls::getRequestedMovement() {
	// return copy of movement requests vector and reset movement requests vector
	VRS::Vector tmpVector = mRequestedMovement;
	mRequestedMovement = VRS::Vector(0.0,tmpVector[1],0.0);	// jump state is preserved; [de]activation through processInput() only!
	return tmpVector;
}
