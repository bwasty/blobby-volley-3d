/// Ball.h: declare class BV3D::Ball

#ifndef BV3D_BALL
#define BV3D_BALL

#include <vrs/sharedobj.h>

struct NewtonBody;

namespace VRS {
	class SceneThing;
	template<typename T> class Matrix4x4Base;
	typedef Matrix4x4Base<double> Matrix;
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
		VRS::SO<VRS::SceneThing> getScene() {return m_Scene;}

		/**
		 * position ball
		 * \param position is the ball's new location
		 */
		void resetPosition(VRS::Vector& position);

	protected:
		/**
		 * update ball physics and visuals
		 */
		void update();

	public:
		/**
		 * callback which notifies that a NewtonBody in the NewtonWorld has changed
		 * \param body is the changed NewtonBody
		 */
		static void applyForceAndTorqueCallback(const NewtonBody* body);

	private:
		double						m_Radius;	// ball radius
		VRS::SO<VRS::SceneThing>	m_Scene;	// local scene
		VRS::SO<VRS::SceneThing>	m_BallScene;	// ball scene
		VRS::SO<VRS::SceneThing>	m_ShadowScene;	// scene for shadows
		VRS::SO<VRS::SceneThing>			m_FloorShadowScene;	// scene for ball's shadow on the floor

	private: // physics
		VRS::SO<BV3D::Arena>	m_Arena;	// parent physics object
		NewtonBody*				m_Body;		// physical body in simulated world
		bool					m_IsLocked;	// locks the ball at its current position until collision
	};
}

#endif	// BV3D_BALL