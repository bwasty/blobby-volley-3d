#ifndef BV3D_BLOBB
#define BV3D_BLOBB

#include "Controls.h"
#include <vrs/vector.h>
#include <vrs/bounds.h>

using namespace BV3D;

namespace BV3D
{
	class Blobb {
	public:
		Blobb();
		~Blobb();
		void moveUp();
		void moveDown();
		void moveRight();
		void moveLeft();
		void jump();
		void setPosition(Vector vtrPos);
		void setCtrlsOrientation(Vector vtrCtrlsOri);
		void setStepDistance(double distance);
		void setBounds(Bounds bounds);
		Controls* getControls();
		void update();
	protected:
		Controls*	m_Controls;
		Vector		m_vtrCtrlsOrientation;
		Vector		m_vtrPosition;
		Bounds		m_Bounds;
	};
}

#endif	// #ifndef BV3D_BLOBB