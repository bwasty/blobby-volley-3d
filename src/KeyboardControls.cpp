
#include "KeyboardControls.h"
#include <vrs/sg/key.h>
#include <vrs/sg/keyevent.h>

using namespace BV3D;

KeyboardControls::KeyboardControls() : Controls() {
	m_Binding[UP] = Key::Up;
	m_Binding[DOWN] = Key::Down;
	m_Binding[RIGHT] = Key::Right;
	m_Binding[LEFT] = Key::Left;
	m_Binding[JUMP] = Key::End;	// Enter dont work :-(
}

KeyboardControls::~KeyboardControls() {
}

void KeyboardControls::processInput(InputEvent* ie) {
	KeyEvent* ke = VRS_Cast(KeyEvent, ie);
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