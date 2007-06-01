#ifndef BV3D_CONTROLS
#define BV3D_CONTROLS

#include <vrs/sg/inputevent.h>

using namespace VRS;

namespace BV3D
{
	class Controls {
	public:
		enum COMMAND {FORWARD=0, BACKWARD=1, RIGHT=2, LEFT=3, JUMP=4};
	public:
		Controls();
		~Controls();
		virtual void processInput(InputEvent* ie) = 0;
		virtual bool isRequested(COMMAND cmd) = 0;
		virtual void consumed() = 0;
	};
}

#endif	// #ifndef BV3D_CONTROLS