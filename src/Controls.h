#ifndef BV3D_CONTROLS
#define BV3D_CONTROLS

#include <vrs/sharedobj.h>
#include <vrs/sg/inputevent.h>
#include <vrs/vector.h>

namespace BV3D
{
	/**
		\brief Controls represents the abstract super class for handling user input for Blobbs.

		The primary intent of the Controls is to allow tracking of multiple keys.
		If more than one key on the keyboard is held down, only the last one of the keys will be periodically reported to/by the system.
		Still, any messages regarding initial key pressing and releasing are captured.
		Therefor, if a key was pressed that is bound to a move request, the Controls sets the corresponding flag until the key is released.

		Blobbs should query the requests from the Controls each frame and move accordingly.
	 */
	class Controls : public VRS::SharedObj {
	public:
		/**
		 * listing of possible (abstract) input commands
		 */
		enum REQUEST {FORWARD, BACKWARD, RIGHT, LEFT, JUMP};

		/**
		 * \dtor
		 */
		virtual ~Controls() {}

		/**
		 * is a callback method for processing InputEvents
		 * \param ie is the VRS::InputEvent to be processed
		 */
		virtual void processInput(VRS::SO<VRS::InputEvent> ie) = 0;

		/**
		 * is called by Blobb to check for move requests for the current frame
		 * Blobbs should move accordingly
		 * Blobbs should call this function only once per frame!
		 * \result a VRS::Vector is returned which represents the movement requests of the user
		 */
		virtual VRS::Vector getRequestedMovement() = 0;
	};
}

#endif	// #ifndef BV3D_CONTROLS