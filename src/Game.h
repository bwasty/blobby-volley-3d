#ifndef BV3D_GAME
#define BV3D_GAME

#include <vrs/glut/glutcanvas.h>
#include <vrs/camera.h>
#include <vrs/sg/scenething.h>
#include <vrs/ambientlight.h>
#include <vrs/sg/behaviorcallback.h>
#include <vrs/opengl/transparencytechniquegl.h>

using namespace VRS;

namespace BV3D
{
	class Arena; class Blobb;

	class Game {
	public:
		Game();
		~Game();
		void update();
		void processInput();
		void notifyBoundsUpdated();
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
		Arena*		m_Arena;
		Blobb*		m_Blobb;
	};
}

#endif	// #ifndef BV3D_GAME