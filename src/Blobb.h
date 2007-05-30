#ifndef BV3D_BLOBB
#define BV3D_BLOBB

#include "Controls.h"
#include <vrs/bounds.h>
#include <vrs/sg/scenething.h>
#include <vrs/translation.h>
#include <vrs/opengl/shapematerialgl.h>

using namespace BV3D;
using namespace VRS;

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
		void setControls(Controls* controls);
		Controls* getControls();
		void update();
		SO<SceneThing> getScene();
		Color getColor();
		void setColor(Color color);
	protected:
		SO<SceneThing>	m_Scene;
		SO<Translation>	m_Translation;
		SO<ShapeMaterialGL>	m_Material;
		Color		m_Color;
		Controls*	m_Controls;
		Vector		m_vtrCtrlsOrientation;
		Vector		m_vtrPosition;
		Bounds		m_Bounds;
	};
}

#endif	// #ifndef BV3D_BLOBB