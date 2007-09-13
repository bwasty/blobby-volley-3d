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

	/*!	\brief Represents the players in BlobbyVolley3D

		it is responsible for:
		- creating(loading) and managing: the 3D shape of the Blobb
		- forwarding InputEvents to its Controls
		- managing the Controls orientation and movement distance
		- updating the position based on input and physics. */
	class Blobb : public VRS::SharedObj {
	public:
		Blobb(VRS::SO<BV3D::Arena> arena, BV3D::TEAM team, VRS::SO<VRS::LookAt> lookAt);
		~Blobb();
		void setPosition(VRS::Vector position);
		void setLookAt(VRS::SO<VRS::LookAt> lookAt) {mLookAt = lookAt;}
		void setControls(VRS::SO<BV3D::Controls> controls){mControls = controls;}
		void setColor(VRS::Color color);
		VRS::SO<BV3D::Controls> getControls()		{return mControls;}
		VRS::SO<VRS::SceneThing> getScene()			{return mScene;}
		VRS::Color BV3D::Blobb::getColor();
		BV3D::TEAM	getTeam()						{return mTeam;}
		void updateShape(VRS::SO<VRS::Canvas> canvas);
		void processInput(VRS::SO<VRS::InputEvent> ie);
		void forceSingleAnimation();
		static void applyForceAndTorqueCallback(const NewtonBody* body);
		void maxJump();					// let blobb do a jump with maximum height

	protected:
		VRS::Vector getMovement();
		void update();
		
	private:
		VRS::SO<VRS::SceneThing>		mScene;				// blobb local scene
		VRS::SO<VRS::SceneThing>		mBlobbScene;		// subscene holding actual blobb
		VRS::SO<VRS::SceneThing>		mShadowScene;		// subscene holding blobb shadow
		VRS::SO<VRS::ShapeMaterialGL>	mMaterial;				// blobb material
		VRS::SO<Controls>				mControls;				// blobb controls
		VRS::SO<VRS::LookAt>			mLookAt;				// blobb movement orientation
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

		// physics
		VRS::SO<BV3D::Arena>			mArena;		// parent physics object
		NewtonBody*						mBody;		// physical body in simulated world
		NewtonCollision**				mCollision;	// pointer to newton (compound) collision objects
	};
}

#endif	// #ifndef BV3D_BLOBB