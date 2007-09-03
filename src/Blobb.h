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

struct NewtonCollision;

namespace BV3D
{
	class Arena;
	class Controls;

	class Blobb : public VRS::SharedObj {
	public:
		Blobb(VRS::SO<BV3D::Arena> arena, BV3D::BV3D_TEAM team);
		~Blobb();
		void setPosition(VRS::Vector position);
		void setCtrlsOrientation(VRS::Vector ctrlsOri);
		void setStepDistance(double distance);
		void setControls(VRS::SO<BV3D::Controls> controls);
		void setColor(VRS::Color color);
		VRS::SO<BV3D::Controls> getControls();
		VRS::SO<VRS::SceneThing> getScene();
		VRS::Color BV3D::Blobb::getColor();
		BV3D::BV3D_TEAM	getTeam();
		bool isMoving();
		VRS::SO<VRS::SceneThing> updateShape(VRS::SO<VRS::Canvas> canvas);
		void processInput(VRS::SO<VRS::InputEvent> ie);

	protected:
		VRS::Vector getMovement();
		void update();

	public:
		static void applyForceAndTorqueCallback(const NewtonBody* body);
		

	private:
		VRS::SO<VRS::SceneThing>		mScene;				// blobb local scene
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
		int								mStep;
		static const int				mMaxStep = 1;

	private:	// physics
		VRS::SO<BV3D::Arena>			mArena;		// parent physics object
		NewtonBody*						mBody;		// physical body in simulated world
		NewtonCollision**				mCollision;	// pointer to newton (compound) collision objects
	};
}

#endif	// #ifndef BV3D_BLOBB