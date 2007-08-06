#ifndef BV3D_KEYBOARDCONTROLS
#define BV3D_KEYBOARDCONTROLS

#include "Controls.h"

namespace BV3D {
	class KeyboardControls : public Controls {
	public:
		KeyboardControls();
		virtual ~KeyboardControls();
		void processInput(VRS::SO<VRS::InputEvent> ie);
		void setBinding(COMMAND cmd, int keyCode);
		bool isRequested(COMMAND cmd);
		void consumed() {}		// nothing to be done for keyboard input
	protected:
		bool	m_Command[5];
		int		m_Binding[5];
	};
}

#endif	// BV3D_KEYBOARDCONTROLS