#ifndef BV3D_MOUSECONTROLS
#define BV3D_MOUSECONTROLS

#include "Controls.h"
#include <vrs/vector.h>

using namespace VRS;

namespace BV3D {
	class MouseControls : public Controls {
	public:
		MouseControls();
		~MouseControls();
		void processInput(InputEvent* ie);
		void setJumpBinding(InputEvent::Button button);
		bool isRequested(COMMAND cmd);
		void consumed();
	protected:
		Vector	m_Target;
		Vector	m_CurrentPosition;
		int		m_JumpBinding;
		bool	m_IsJumping;
	};
}

#endif	// BV3D_MOUSECONTROLS