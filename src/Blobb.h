#ifndef BV3D_BLOBB
#define BV3D_BLOBB

#include <vrs/sharedobj.h>
#include <vrs/vector.h>
#include <vrs/container/array.h>
#include "Constants.h"

struct NewtonBody;

namespace VRS {
	class SceneThing;
	class ShapeMaterialGL;
	class InputEvent;
	class Canvas;
	//template <typename T> class Array;
}

namespace BV3D
{
	class Arena;
	class Controls;

	class Blobb : public VRS::SharedObj {
	public:
		/**
		 * ctor
		 * \param arena specifies the arena in which the blobb is simulated
		 */
		Blobb(VRS::SO<BV3D::Arena> arena, BV3D::BV3D_TEAM team);

		/**
		 * dtor
		 */
		~Blobb();

		/**
		 * move blobb to specified position
		 */
		void setPosition(VRS::Vector position);

		/**
		 * sets the orientation of the controls
		 * \param ctrlsOri is the direction the blobb moves to when controlling forward
		 */
		void setCtrlsOrientation(VRS::Vector ctrlsOri);

		/**
		 * sets the distance the blobb moves with one step
		 */
		void setStepDistance(double distance);

		/**
		 * assigns the controls for controlling the blobb
		 */
		void setControls(VRS::SO<BV3D::Controls> controls) {mControls = controls;}

		/**
		 * returns the current controls for this blobb
		 */
		VRS::SO<BV3D::Controls> getControls() {return mControls;}

		/**
		 * returns the blobb local scene
		 */
		VRS::SO<VRS::SceneThing> getScene() {return mScene;}

		/**
		 * returns the blobb's current color
		 */
		VRS::Color BV3D::Blobb::getColor();

		/**
		 * set the blobb's color
		 */
		void setColor(VRS::Color color);

		/**
		 * processes user input for this blobb
		 * \param ie is an input event that was explicitely forwarded from the input callback in BV3D::Game
		 */
		void processInput(VRS::SO<VRS::InputEvent> ie);

	protected:
		/**
		 * evaluates the controls state and returns the movement vector for the current frame.
		 * this resets the controls state for the next frame.
		 */
		VRS::Vector getMovement();

		/**
		 * updates physical and visual blobb
		 */
		void update();

	public:
		/**
		 * callback which notifies that a NewtonBody in the NewtonWorld has changed
		 * \param body is the changed NewtonBody
		 */
		static void applyForceAndTorqueCallback(const NewtonBody* body);

		BV3D::BV3D_TEAM	getTeam() {return mTeam;}

		bool isMoving()		{return mIsMoving;}

		VRS::SO<VRS::SceneThing> updateShape(VRS::SO<VRS::Canvas> canvas);

	private:
		VRS::SO<VRS::SceneThing>		mScene;				// blobb local scene
		//VRS::SO<VRS::SceneThing>		mBlobbShape;			// scene for blobb-shape
		VRS::SO<VRS::ShapeMaterialGL>	mMaterial;				// blobb material
		VRS::SO<Controls>				mControls;				// blobb controls
		VRS::Vector						mCtrlsOrientation;		// blobb controls orientation
		bool							mJumpAllowed;			// indicates if blobb may jump
		BV3D::BV3D_TEAM					mTeam;
		static const int				mNumShapes = 5;		//number of animation shapes
		int								mCurrentShape;
		VRS::SO<VRS::Array< VRS::SO<VRS::SceneThing>> >	mShapes;	//shapes for animation of blobb
		bool							mDecreasing;			//true if current shape is decreasing
		bool							mIsMoving;
		bool							mInit;

	private:	// physics
		VRS::SO<BV3D::Arena>			mArena;	// parent physics object
		NewtonBody*						mBody;		// physical body in simulated world
	};
}

#endif	// #ifndef BV3D_BLOBB