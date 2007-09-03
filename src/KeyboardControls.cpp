
#include "KeyboardControls.h"
#include <vrs/sg/key.h>
#include <vrs/sg/keyevent.h>

BV3D::KeyboardControls::KeyboardControls() : Controls() {
	setBinding(FORWARD, VRS::Key::Up);
	setBinding(BACKWARD, VRS::Key::Down);
	setBinding(RIGHT ,VRS::Key::Right);
	setBinding(LEFT ,VRS::Key::Left);
	setBinding(JUMP, 32);	// Space
}

void BV3D::KeyboardControls::processInput(VRS::SO<VRS::InputEvent> ie) {
	VRS::SO<VRS::KeyEvent> ke = VRS_Cast(VRS::KeyEvent, ie);

	// process KeyEvents only
	if(ke != NULL)
		for(int i=0;i<5;i++)
			if(ke->keyCode()==m_Binding[i]) {
				if(ke->pressed())
					setRequest((REQUEST)(1<<i));	// set request if key was pressed
				else
					clearRequest((REQUEST)(1<<i));	// clear request if key was released
			}
}

void BV3D::KeyboardControls::setBinding(REQUEST req, int keyCode) {
	switch(req) {
		case FORWARD:	m_Binding[0] = keyCode; break;
		case BACKWARD:	m_Binding[1] = keyCode; break;
		case RIGHT:		m_Binding[2] = keyCode; break;
		case LEFT:		m_Binding[3] = keyCode; break;
		case JUMP:		m_Binding[4] = keyCode; break;
		default: break;
	}
}
