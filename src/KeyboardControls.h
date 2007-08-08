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

namespace BV3D {
	class KeyboardControls : public Controls {

	public:
		/**
		 * ctor
		 */
		KeyboardControls();

		/**
		 * dtor
		 */
		virtual ~KeyboardControls() {}

		/**
		 * is a callback for processing InputEvent's (KeyEvent)
		 */
		void processInput(VRS::SO<VRS::InputEvent> ie);

		/**
		 * map VRS virtual key codes to move requests
		 */
		void setBinding(REQUEST req, int keyCode);

	protected:
		int		m_Binding[5];	// key-to-request mapping
	};
}

#endif	// BV3D_KEYBOARDCONTROLS