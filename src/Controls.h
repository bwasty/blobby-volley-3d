#ifndef BV3D_CONTROLS
#define BV3D_CONTROLS

#include <vrs/sg/inputevent.h>

using namespace VRS;

namespace BV3D
{
	class Controls {
	public:
		enum COMMAND {UP=0,DOWN=1,RIGHT=2,LEFT=3,JUMP=4};
	public:
		Controls();
		~Controls();
		virtual void processInput(InputEvent* ie) = 0;
		bool isRequested(COMMAND cmd);
	protected:
		bool					m_Command[5];
	};
}

#endif	// #ifndef BV3D_CONTROLS