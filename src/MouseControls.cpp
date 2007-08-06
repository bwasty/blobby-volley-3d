
#include "MouseControls.h"
#include <vrs/sg/motionevent.h>
#include <vrs/sg/buttonevent.h>
#include <stdio.h>

using namespace BV3D;

MouseControls::MouseControls() {
	for(int i=0;i<5;i++) m_Command[i] = false;
	m_JumpBinding = VRS::InputEvent::MouseButton1;
}

MouseControls::~MouseControls() {
}

void MouseControls::setJumpBinding(VRS::InputEvent::Button button) {
	m_JumpBinding = button;
}

void MouseControls::processInput(VRS::SO<VRS::InputEvent> ie) {

	// check for movement
	VRS::SO<VRS::MotionEvent> me = VRS_Cast(VRS::MotionEvent, ie);
	if(me!=NULL) {
		if(me->deltaX()>0) {m_Command[RIGHT] = true; m_Command[LEFT] = false;}
		if(me->deltaX()<0) {m_Command[RIGHT] = false; m_Command[LEFT] = true;}
		if(me->deltaY()>0) {m_Command[FORWARD] = false; m_Command[BACKWARD] = true;}
		if(me->deltaY()<0) {m_Command[FORWARD] = true; m_Command[BACKWARD] = false;}
	}

	// check if jump button clicked
	VRS::SO<VRS::ButtonEvent> be = VRS_Cast(VRS::ButtonEvent, ie);
	if(be!=NULL)
		if(be->button()==m_JumpBinding)
			m_Command[JUMP] = true;
}

bool MouseControls::isRequested(COMMAND cmd) {
	return m_Command[cmd];
}

void MouseControls::consumed() {
	for(int i=0;i<5;i++) m_Command[i] = false;
}