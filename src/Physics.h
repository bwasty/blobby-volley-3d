/// Physics.h: declares class Physics
///
/// class Physics encapsulates the physics engine.

#ifndef BV3D_PHYSICS
#define BV3D_PHYSICS

#include <vrs/so.h>
#include <vrs/sharedobj.h>

struct NewtonBody;
struct NewtonWorld;

namespace BV3D
{
	class Ball;

	class Physics : public VRS::SharedObj
	{
	public:
		Physics();
		~Physics();
		void updateWorld(float timestep);
		void registerBall(VRS::SO<BV3D::Ball> ball);
	protected:
		static void applyForceAndTorqueToBall(const NewtonBody* body);
	private:
		NewtonWorld* m_World;
	};
}

#endif	// BV3D_PHYSICS