#include "MouseControls.h"
#include <vrs/sg/motionevent.h>
#include <vrs/sg/buttonevent.h>


void BV3D::MouseControls::processInput(VRS::SO<VRS::InputEvent> ie) {

	// check for mouse movement
	VRS::SO<VRS::MotionEvent> me = VRS_Cast(VRS::MotionEvent, ie);
	if(me!=NULL) {
		if(me->deltaX()>0) {setRequest(RIGHT); clearRequest(LEFT);}
		if(me->deltaX()<0) {setRequest(LEFT); clearRequest(RIGHT);}
		if(me->deltaY()>0) {setRequest(BACKWARD); clearRequest(FORWARD);}
		if(me->deltaY()<0) {setRequest(FORWARD); clearRequest(BACKWARD);}
	}

	// check if button was pressed or released
	VRS::SO<VRS::ButtonEvent> be = VRS_Cast(VRS::ButtonEvent, ie);
	if(be!=NULL)
		if(be->button()==m_JumpButton) {
			if(be->pressed()==VRS::ButtonEvent::Pressed)
				setRequest(JUMP);	// set jump request if jump button was pressed
			else
				clearRequest(JUMP);	// clear jump request if jump button was released
		}
}
