#ifndef BV3D_BLOBB
#define BV3D_BLOBB

#include <vrs/sharedobj.h>
#include <vrs/so.h>
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
	class Blobb : public SharedObj {
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
		void setControls(SO<Controls> controls);
		SO<Controls> getControls();
		void update();
		SO<SceneThing> getScene();
		Color getColor();
		void setColor(Color color);
		void processInput(SO<InputEvent> ie);
	private:
		SO<SceneThing>	m_Scene;
		SO<Translation>	m_Translation;
		SO<ShapeMaterialGL>	m_Material;
		SO<Controls>	m_Controls;
		Vector		m_CtrlsOrientation;
		Vector		m_Position;
		Bounds		m_Bounds;
	};
}

#endif	// #ifndef BV3D_BLOBB