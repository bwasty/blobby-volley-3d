#ifndef BV3D_KEYBOARDCONTROLS
#define BV3D_KEYBOARDCONTROLS

#include "Controls.h"
#include <vrs/sg/key.h>

namespace BV3D {
	/**
		\brief KeyboardControls is a sub-class of Controls for handling keyboard input for blobbs

		it maintains a mapping of VRS key codes that are bound to movement requests.
		VRS::KeyEvents are examined, and if a mapped key code is determined, the corresponding request is issued.
	*/
	class KeyboardControls : public Controls {
	public:
		/**
		 * \ctor
		 * \param VRS key codes that are bound to corresponding movement requests
		 */
		KeyboardControls(
			int keyForward = VRS::Key::Up,
			int keyBackward = VRS::Key::Down,
			int keyLeft = VRS::Key::Left,
			int keyRight = VRS::Key::Right,
			int keyJump = 32);	// Spacebar = 32

		/**
		 * dtor
		 */
		virtual ~KeyboardControls() {}

		void processInput(VRS::SO<VRS::InputEvent> ie);
		virtual VRS::Vector getRequestedMovement();
		void setBinding(REQUEST request, int keyCode);

	private:
		int		mBinding[5];	// key-code-to-request mapping
		bool	mRequests[5];	// bit field representing issued requests
	};
}

#endif	// BV3D_KEYBOARDCONTROLS