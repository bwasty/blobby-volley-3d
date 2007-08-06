/**
 * Controls represents the abstract super class for handling user input for Blobbs.
 * it declares the following interface methods:
 * - virtual void processInput(InputEvent*):
 *   for Blobbs to pass InputEvents to the Controls
 * - virtual bool isRequested(COMMAND):
 *   for Blobbs to test if any commands are issued
 * - virtual void consumed():
 *   for Blobbs to notify the Controls that the commands for the current frame were evaluated.
 *   this is necessary for mouse controls, where requests cannot be evaluated by the state of keys.
 *   mouse movement requests are therefore kept until consumed() is called each frame.
 *
 * the primary intent of the Controls is to allow tracking of multiple keys.
 * since for example if more than one key one the keyboard is held down, only the last one of
 * the keys will be periodically reported to the system. still, any messages regarding initial key pressing
 * and releasing should be captured. therefor, if a key was pressed that is bound to command, the Controls
 * sets the corresponding command until the key is released. Blobbs query the commands from the Controls
 * when they are updated.
 */

#ifndef BV3D_CONTROLS
#define BV3D_CONTROLS

#include <vrs/sharedobj.h>
#include <vrs/so.h>

#include <vrs/sg/inputevent.h>

namespace BV3D
{
	class Controls : public VRS::SharedObj{
	public:
		enum COMMAND {FORWARD=0, BACKWARD=1, RIGHT=2, LEFT=3, JUMP=4};
	public:
		Controls() {}
		virtual ~Controls() {}
		virtual void processInput(VRS::SO<VRS::InputEvent> ie) = 0;
		virtual bool isRequested(COMMAND cmd) = 0;
		virtual void consumed() = 0;
	};
}

#endif	// #ifndef BV3D_CONTROLS