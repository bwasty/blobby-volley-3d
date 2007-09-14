#ifndef BV3D_MOUSECONTROLS
#define BV3D_MOUSECONTROLS

#include "Controls.h"

namespace BV3D {
	/**
	 * BV3D::MouseControls is a sub-class of Controls for handling mouse input for Blobbs.
	 *
	 * Basic movement requests are implicitly bound to mouse movement (ie: mouse move to the right -> move right).
	 * The jump button can be set to be any of the mouse buttons.
	 * 
	 * Since mouse movement is an event, all such events in a single frame are acknowledged until they are evaluated by the Blobb.
	 * That is, even if the mouse stopped moving long before the frame ended, the previous movement request will be stored.
	 * After the Blobb queried the requests, the internal requests are cleared. This ensures that mouse input only
	 * affects the current frame.
	 */
	class MouseControls : public Controls {

	public:
		/*
		 * ctor
		 * \param jumpButton is the VRS button code for the button to be mapped to the jump request
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
		VRS::InputEvent::Button		mJumpButton;		// holds button mapped to jump request
		VRS::Vector					mRequestedMovement;	// vector holding move requests for the Blobb
	};
}

#endif	// BV3D_MOUSECONTROLS