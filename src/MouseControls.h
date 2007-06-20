#ifndef BV3D_MOUSECONTROLS
#define BV3D_MOUSECONTROLS

#include "Controls.h"
#include <vrs/vector.h>

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
		VRS::Vector	m_Target;
		VRS::Vector	m_CurrentPosition;
		int		m_JumpBinding;
		bool	m_IsJumping;
	};
}

#endif	// BV3D_MOUSECONTROLS