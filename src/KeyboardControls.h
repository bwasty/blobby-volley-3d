/**
 * BV3D::KeyboardControls is a sub-class of BV3D::Controls for handling keyboard input for blobbs.
 *
 * it maintains a mapping of VRS key codes that refer to movement requests.
 * VRS::KeyEvents are examined, and if a mapped key code is determined, the corresponding
 * request is issued.
 */

#ifndef BV3D_KEYBOARDCONTROLS
#define BV3D_KEYBOARDCONTROLS

#include "Controls.h"
#include <vrs/sg/key.h>

namespace BV3D {
	class KeyboardControls : public Controls {

	public:
		KeyboardControls(
			int keyForward = VRS::Key::Up,
			int keyBackward = VRS::Key::Down,
			int keyLeft = VRS::Key::Left,
			int keyRight = VRS::Key::Right,
			int keyJump = 32);

		/**
		 * dtor
		 */
		virtual ~KeyboardControls() {}

		void processInput(VRS::SO<VRS::InputEvent> ie);
		void setBinding(REQUEST req, int keyCode);
		virtual VRS::Vector getRequestedMovement();
	protected:
		int		mBinding[5];	// key-to-request mapping
		bool	mRequests[5];	// bit field representing issued requests
	};
}

#endif	// BV3D_KEYBOARDCONTROLS