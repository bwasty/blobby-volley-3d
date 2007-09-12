/// Ball.h: declare class BV3D::Ball

#ifndef BV3D_BALL
#define BV3D_BALL

#include <vrs/sharedobj.h>

struct NewtonBody;

namespace VRS {
	class SceneThing;
	template<typename T> class Matrix4x4Base;
	typedef Matrix4x4Base<double> Matrix;
	class Disc;
}

namespace BV3D
{
	class Arena;

	class Ball : public VRS::SharedObj
	{
	public:
		/**
		 * ctor
		 * \param arena specifies the arena in which the ball is simulated
		 */
		Ball(VRS::SO<BV3D::Arena> arena);

		/**
		 * dtor
		 */
		~Ball();

		/**
		 * returns the local ball scene
		 */
		VRS::SO<VRS::SceneThing> getScene() {return mScene;}

		/**
		 * position ball
		 * \param position is the ball's new location
		 */
		void resetPosition(VRS::Vector& position);

		VRS::Vector getPosition();

		NewtonBody* getBody() {return mBody;}

		/**
		 * callback which notifies that a NewtonBody in the NewtonWorld has changed
		 * \param body is the changed NewtonBody
		 */
		static void applyForceAndTorqueCallback(const NewtonBody* body);

	protected:
		/**
		 * update ball physics and visuals
		 */
		void update();

	private:
		double						mRadius;	// ball radius
		VRS::SO<VRS::SceneThing>	mScene;		// local scene
		VRS::SO<VRS::SceneThing>	mBallScene;	// subscene holding the actual ball
		VRS::SO<VRS::SceneThing>	mShadowScene;	// subscene holding fake shadow

		// physics
		VRS::SO<BV3D::Arena>	mArena;	// parent physics object
		NewtonBody*				mBody;		// physical body in simulated world
		bool					mIsLocked;	// locks the ball at its current position until collision
	};
}

#endif	// BV3D_BALL
