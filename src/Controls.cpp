/**
 * Controls represents the abstract super class for handling user input for Blobbs.
 * it declares the following interface methods:
 * - virtual void processInput(InputEvent*):
 *   for Blobbs to pass InputEvents to the Controls
 * - virtual bool isRequested(COMMAND):
 *   for Blobbs to test if any commands are issued
 * - virtual void consumed():
 *   for Blobbs to notify the Controls that the commands for the current frame were evaluated.
 *   this is necessary for mouse controls, where a target location is pointed to by the mouse
 *   and the Blobb will approach that location in the following frames
 *
 * the primary intent of the Controls is to allow tracking of multiple keys.
 * since for example if more than one key one the keyboard is held down, only the last one of
 * the keys will be periodically reported to the system. still, any messages regarding initial key pressing
 * and releasing should be captured. therefor, if a key was pressed that is bound to command, the Controls
 * sets the corresponding command until the key is released. Blobbs query the commands from the Controls
 * when they are updated.
 */
#include "Controls.h"

using namespace VRS;
using namespace BV3D;

Controls::Controls() {
}

Controls::~Controls() {
}
