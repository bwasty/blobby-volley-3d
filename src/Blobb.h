#ifndef BV3D_BLOBB
#define BV3D_BLOBB

#include <vrs/sharedobj.h>
#include <vrs/vector.h>
#include "Constants.h"

struct NewtonBody;

namespace VRS {
	class SceneThing;
	class ShapeMaterialGL;
	class InputEvent;
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
		void setControls(VRS::SO<BV3D::Controls> controls) {m_Controls = controls;}

		/**
		 * returns the current controls for this blobb
		 */
		VRS::SO<BV3D::Controls> getControls() {return m_Controls;}

		/**
		 * returns the blobb local scene
		 */
		VRS::SO<VRS::SceneThing> getScene() {return m_Scene;}

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

		BV3D::BV3D_TEAM	getTeam() {return m_Team;}

	private:
		VRS::SO<VRS::SceneThing>		m_Scene;				// blobb local scene
		VRS::SO<VRS::ShapeMaterialGL>	m_Material;				// blobb material
		VRS::SO<Controls>				m_Controls;				// blobb controls
		VRS::Vector						m_CtrlsOrientation;		// blobb controls orientation
		bool							m_JumpAllowed;			// indicates if blobb may jump
		BV3D::BV3D_TEAM					m_Team;

	private:	// physics
		VRS::SO<BV3D::Arena>	m_Arena;	// parent physics object
		NewtonBody*				m_Body;		// physical body in simulated world
	};
}

#endif	// #ifndef BV3D_BLOBB