#ifndef BV3D_CONTROLS
#define BV3D_CONTROLS

#include <vrs/sharedobj.h>
#include <vrs/so.h>

#include <vrs/sg/inputevent.h>

namespace BV3D
{
	class Controls : VRS::SharedObj{
	public:
		enum COMMAND {FORWARD=0, BACKWARD=1, RIGHT=2, LEFT=3, JUMP=4};
	public:
		Controls();
		virtual ~Controls();
		virtual void processInput(VRS::SO<VRS::InputEvent> ie) = 0;
		virtual bool isRequested(COMMAND cmd) = 0;
		virtual void consumed() = 0;
	};
}

#endif	// #ifndef BV3D_CONTROLS