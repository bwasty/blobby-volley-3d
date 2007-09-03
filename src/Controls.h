/**
 * Controls represents the abstract super class for handling user input for Blobbs.
 *
 * the primary intent of the Controls is to allow tracking of multiple keys.
 * since for example if more than one key one the keyboard is held down, only the last one of
 * the keys will be periodically reported to the system. still, any messages regarding initial key pressing
 * and releasing should be captured. therefor, if a key was pressed that is bound to a move request, the Controls
 * sets the corresponding request until the key is released.
 *
 * Blobbs should query the requests from the Controls each frame and move accordingly.
 */

#ifndef BV3D_CONTROLS
#define BV3D_CONTROLS

#include <vrs/sharedobj.h>
#include <vrs/sg/inputevent.h>
#include <vrs/vector.h>

namespace BV3D
{
	class Controls : public VRS::SharedObj {

	public:
		/**
		 * listing of possible (abstract) input commands
		 */
		enum REQUEST {FORWARD=1, BACKWARD=2, RIGHT=4, LEFT=8, JUMP=16, MOVE_REQUESTS=15, ALL_REQUESTS=31};

		/**
		 * dtor
		 */
		virtual ~Controls() {}

		/**
		 * is a callback for processing InputEvent's
		 */
		virtual void processInput(VRS::SO<VRS::InputEvent> ie) = 0;

		/**
		 * is called by Blobb to check for move requests for the current frame
		 * blobbs should move accordingly
		 */
		virtual VRS::Vector getRequestedMovement() = 0;
	};
}

#endif	// #ifndef BV3D_CONTROLS