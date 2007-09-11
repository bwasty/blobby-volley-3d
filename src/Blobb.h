#ifndef BV3D_BLOBB
#define BV3D_BLOBB

#include <vrs/sharedobj.h>
#include <vrs/container/array.h>
#include <vrs/vector.h>
#include "Constants.h"

struct NewtonBody;

namespace VRS {
	class SceneThing;
	class ShapeMaterialGL;
	class InputEvent;
	class Canvas;
	class LookAt;
}

struct NewtonCollision;

namespace BV3D
{
	class Arena;
	class Controls;

	class Blobb : public VRS::SharedObj {
	public:
		Blobb(VRS::SO<BV3D::Arena> arena, BV3D::TEAM team, VRS::SO<VRS::LookAt> lookAt);
		~Blobb();
		void setPosition(VRS::Vector position);
		void setLookAt(VRS::SO<VRS::LookAt> lookAt) {mLookAt = lookAt;}
		void setStepDistance(double distance) {mStepDistance = distance;}
		void setControls(VRS::SO<BV3D::Controls> controls);
		void setColor(VRS::Color color);
		VRS::SO<BV3D::Controls> getControls();
		VRS::SO<VRS::SceneThing> getScene();
		VRS::Color BV3D::Blobb::getColor();
		BV3D::TEAM	getTeam();
		bool isMoving();
		void updateShape(VRS::SO<VRS::Canvas> canvas);
		void processInput(VRS::SO<VRS::InputEvent> ie);
		void forceSingleAnimation();

	protected:
		VRS::Vector getMovement();
		void update();

	public:
		static void applyForceAndTorqueCallback(const NewtonBody* body);
		void maxJump();					// let blobb do a jump with maximum height
		
	private:
		VRS::SO<VRS::SceneThing>		mScene;				// blobb local scene
		VRS::SO<VRS::SceneThing>		mBlobbScene;		// subscene holding actual blobb
		VRS::SO<VRS::SceneThing>		mShadowScene;		// subscene holding blobb shadow
		VRS::SO<VRS::ShapeMaterialGL>	mMaterial;				// blobb material
		VRS::SO<Controls>				mControls;				// blobb controls
		VRS::SO<VRS::LookAt>			mLookAt;				// blobb movement orientation
		double							mStepDistance;			// blobb step distance
		bool							mJumpAllowed;			// indicates if blobb may jump
		BV3D::TEAM						mTeam;
		static const int				mNumShapes = 5;		//number of animation shapes
		int								mCurrentShape;
		VRS::SO<VRS::Array< VRS::SO<VRS::SceneThing>> >	mShapes;	//shapes for animation of blobb
		bool							mDecreasing;			//true if current shape is decreasing
		bool							mIsMoving;
		bool							mForceAnimation;
		int								mStep;
		static const int				mMaxStep = 1;
		bool							mMaxJump;			// indicates that blobb should do a maximum high jump

	private:	// physics
		VRS::SO<BV3D::Arena>			mArena;		// parent physics object
		NewtonBody*						mBody;		// physical body in simulated world
		NewtonCollision**				mCollision;	// pointer to newton (compound) collision objects

	};
}

#endif	// #ifndef BV3D_BLOBB