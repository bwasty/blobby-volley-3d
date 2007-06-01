#ifndef BV3D_BLOBB
#define BV3D_BLOBB

#include "Controls.h"
#include <vrs/bounds.h>
#include <vrs/sg/scenething.h>
#include <vrs/translation.h>
#include <vrs/opengl/shapematerialgl.h>
#include <vrs/sg/inputevent.h>

using namespace BV3D;
using namespace VRS;

namespace BV3D
{
	class Blobb {
	public:
		Blobb();
		~Blobb();
		void moveForward();
		void moveBackward();
		void moveRight();
		void moveLeft();
		void jump();
		void setPosition(Vector position);
		void setCtrlsOrientation(Vector ctrlsOri);
		void setStepDistance(double distance);
		void setBounds(Bounds bounds);
		void setControls(Controls* controls);
		Controls* getControls();
		void update();
		SO<SceneThing> getScene();
		Color getColor();
		void setColor(Color color);
		void processInput(InputEvent* ie);
	protected:
		SO<SceneThing>	m_Scene;
		SO<Translation>	m_Translation;
		SO<ShapeMaterialGL>	m_Material;
		Controls*	m_Controls;
		Vector		m_CtrlsOrientation;
		Vector		m_Position;
		Bounds		m_Bounds;
	};
}

#endif	// #ifndef BV3D_BLOBB