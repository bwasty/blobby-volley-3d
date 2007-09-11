/**
 * BV3D::MouseControls is a sub-class of BV3D::Controls for handling mouse input for blobbs.
 *
 * basic movement requests are implicitly bound to mouse movement (ie: mouse move to the right -> move right).
 * a jump button can be set to be any of the mouse buttons.
 * 
 * since mouse movement is an event, all events in a single frame are stored until they are evaluated by the Blobb.
 * that is, even if the mouse stopped moving long before the frame ended, the previous movement request will be stored.
 * after the Blobb received the requests, the internal requests are cleared. this ensures that mouse input only
 * affects the current frame.
 */

#ifndef BV3D_MOUSECONTROLS
#define BV3D_MOUSECONTROLS

#include "Controls.h"

namespace BV3D {
	class MouseControls : public Controls {

	public:
		/*
		 * ctor
		 */
		MouseControls(VRS::InputEvent::Button jumpButton = VRS::InputEvent::MouseButton1) {mJumpButton = jumpButton;}

		/**
		 * dtor
		 */
		virtual ~MouseControls() {}

		void processInput(VRS::SO<VRS::InputEvent> ie);

		/**
		 * set mouse button for jump request (all other requests are implcitly bound to mouse movement)
		 */
		void setJumpBinding(VRS::InputEvent::Button button) {mJumpButton = button;}

		virtual VRS::Vector getRequestedMovement();

	protected:
		VRS::InputEvent::Button		mJumpButton;		// stores button for jumping
		VRS::Vector					mRequestedMovement;	// vector holding move requests for the Blobb
	};
}

#endif	// BV3D_MOUSECONTROLS