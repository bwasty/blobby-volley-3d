#ifndef BV3D_GAME
#define BV3D_GAME

#include <vrs/glut/glutcanvas.h>
#include <vrs/camera.h>
#include <vrs/sg/scenething.h>
#include <vrs/ambientlight.h>
#include <vrs/sg/behaviorcallback.h>
#include <vrs/opengl/transparencytechniquegl.h>

#include "Blobb.h"

using namespace VRS;

namespace BV3D
{
	class Game {
	public:
		Game();
		~Game();
		void setExtent(Vector vtrExtent);
		void update();
		void processInput();
	protected:
		SO<GlutCanvas>		m_Canvas;
		SO<SceneThing>		m_RootScene;
		SO<TransparencyTechniqueGL>	m_TransparencyTechnique;
		SO<Camera>			m_Camera;
		SO<AmbientLight>	m_AmbientLight;
		SO<BehaviorCallback>	m_cbInput;
		SO<BehaviorCallback>	m_cbUpdate;
		int			m_iFramerate;
		double		m_dLastSecond;
		double		m_dLastUpdateTime;
		double		m_FPS;
		Bounds		m_Bounds;
		Blobb		m_Blobb[2];
	};
}

#endif	// #ifndef BV3D_GAME