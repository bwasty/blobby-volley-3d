/// Physics.h: declares class Physics
///
/// class Physics encapsulates the physics engine.

#ifndef BV3D_PHYSICS
#define BV3D_PHYSICS

#include <Newton.h>
#include "Ball.h"
#include <vrs/so.h>
#include <vrs/sharedobj.h>
#include <vrs/container/dictionary.h>

namespace BV3D
{
	class Physics
	{
	public:
		Physics();
		~Physics();
		void updateWorld(dFloat timestep);
		void registerBall(VRS::SO<BV3D::Ball> ball);
	protected:
		static void applyForceAndTorqueToBall(const NewtonBody* body);
	private:
		NewtonWorld* m_World;
	};
}

#endif	// BV3D_PHYSICS