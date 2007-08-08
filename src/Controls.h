/**
 * Controls represents the abstract super class for handling user input for Blobbs.
 *
 * the primary intent of the Controls is to allow tracking of multiple keys.
 * since for example if more than one key one the keyboard is held down, only the last one of
 * the keys will be periodically reported to the system. still, any messages regarding initial key pressing
 * and releasing should be captured. therefor, if a key was pressed that is bound to a command, the Controls
 * sets the corresponding command until the key is released.
 *
 * Blobbs query the commands from the Controls when they are updated.
 */

#ifndef BV3D_CONTROLS
#define BV3D_CONTROLS

#include <vrs/sharedobj.h>
#include <vrs/so.h>

#include <vrs/sg/inputevent.h>

namespace BV3D
{
	class Controls : public VRS::SharedObj {

	public:
		/**
		 * listing of possible (abstract) input commands
		 */
		enum COMMAND {FORWARD=0, BACKWARD=1, RIGHT=2, LEFT=3, JUMP=4};

	protected:
		/**
		 * ctor
		 */
		Controls() {}
	public:
		/**
		 * dtor
		 */
		virtual ~Controls() {}

		/**
		 * is a callback for processing InputEvent's
		 */
		virtual void processInput(VRS::SO<VRS::InputEvent> ie) = 0;

		/**
		 * is called by Blobb to check if a certain input command was issued.
		 * blobbs should move accordingly
		 * \param cmd is one of the possible input commands of enum BV3D::Controls::COMMAND
		 */
		virtual bool isRequested(COMMAND cmd) = 0;

		/**
		 * is called by Blobb to signal that the input for the current frame was acknowlegded.
		 * (this is necessary ie for MouseControls)
		 */
		virtual void consumed() = 0;
	};
}

#endif	// #ifndef BV3D_CONTROLS