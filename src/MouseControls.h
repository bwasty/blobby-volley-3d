#ifndef BV3D_MOUSECONTROLS
#define BV3D_MOUSECONTROLS

#include "Controls.h"
#include <vrs/vector.h>
#include <vrs/sg/inputevent.h>

namespace BV3D {
	class MouseControls : public Controls {
	public:
		MouseControls();
		virtual ~MouseControls();
		void processInput(VRS::SO<VRS::InputEvent> ie);
		void setJumpBinding(VRS::InputEvent::Button button);
		bool isRequested(COMMAND cmd);
		void consumed();
	protected:
		VRS::InputEvent::Button		m_JumpBinding;	// stores button for jumping
		bool	m_Command[5];
	};
}

#endif	// BV3D_MOUSECONTROLS