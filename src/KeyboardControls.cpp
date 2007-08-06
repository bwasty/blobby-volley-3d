
#include "KeyboardControls.h"
#include <vrs/sg/key.h>
#include <vrs/sg/keyevent.h>

using namespace BV3D;

KeyboardControls::KeyboardControls() : Controls() {
	for(int i=0;i<5;i++) m_Command[i] = false;
	m_Binding[FORWARD] = VRS::Key::Up;
	m_Binding[BACKWARD] = VRS::Key::Down;
	m_Binding[RIGHT] = VRS::Key::Right;
	m_Binding[LEFT] = VRS::Key::Left;
	m_Binding[JUMP] = VRS::Key::End;	// Enter dont work :-(
}

KeyboardControls::~KeyboardControls() {
}

void KeyboardControls::processInput(VRS::SO<VRS::InputEvent> ie) {
	VRS::SO<VRS::KeyEvent> ke = VRS_Cast(VRS::KeyEvent, ie);
	if(ke != NULL) {
		bool state = ke->pressed();
		for(int i=0;i<5;i++)
			if(ke->keyCode()==m_Binding[i])
				m_Command[i] = state;
	}
}

void KeyboardControls::setBinding(COMMAND cmd, int keyCode) {
	m_Binding[cmd] = keyCode;
}

bool KeyboardControls::isRequested(COMMAND cmd) {
	return m_Command[cmd];
}
