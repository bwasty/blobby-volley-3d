
#include "MouseControls.h"
#include <vrs/sg/motionevent.h>
#include <vrs/sg/buttonevent.h>
#include <stdio.h>

using namespace BV3D;

MouseControls::MouseControls() {
	m_JumpBinding = InputEvent::MouseButton1;
	m_IsJumping = false;
}

MouseControls::~MouseControls() {
}

void MouseControls::setJumpBinding(InputEvent::Button button) {
	m_JumpBinding = button;
}

void MouseControls::processInput(InputEvent* ie) {
	MotionEvent* me = VRS_Cast(MotionEvent, ie);
	if(me != NULL) {
		m_Target[0] += me->deltaX();
		m_Target[2] += me->deltaY();
		//printf("(%d,%d)",me->deltaX(),me->deltaY());
	}
}

bool MouseControls::isRequested(COMMAND cmd) {
	switch(cmd) {
		case UP:	return (m_Target[2] < m_CurrentPosition[2]);
		case DOWN:	return (m_Target[2] > m_CurrentPosition[2]);
		case RIGHT:	return (m_Target[0] > m_CurrentPosition[0]);
		case LEFT:	return (m_Target[0] < m_CurrentPosition[0]);
		case JUMP:	return m_IsJumping;
		default: return false;
	}

}

void MouseControls::consumed() {
	// advance m_CurrentPosition towards m_Target
	Vector vtrTmp = m_Target - m_CurrentPosition;
	vtrTmp.normalize();
	m_CurrentPosition += vtrTmp;
}