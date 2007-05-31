#ifndef BV3D_GAME
#define BV3D_GAME

#include <vrs/glut/glutcanvas.h>
#include <vrs/camera.h>
#include <vrs/sg/scenething.h>
#include <vrs/ambientlight.h>
#include <vrs/sg/behaviorcallback.h>
#include <vrs/opengl/transparencytechniquegl.h>

#include "Arena.h"
#include "Blobb.h"

using namespace VRS;

namespace BV3D
{
	class Game {
	public:
		Game();
		~Game();
		void update();			// render new frame
		void processInput();	// process and dispatch input
		void setArenaBounds(Vector extent);		// change Arena bounds and notify Blobbs etc
	protected:
		SO<GlutCanvas>		m_Canvas;		// main canvas
		SO<SceneThing>		m_RootScene;	// root node where Blobbs etc will be appended to
		SO<TransparencyTechniqueGL>	m_TransparencyTechnique;	// to enable transparency
		SO<Camera>			m_Camera;			// scene camera
		SO<AmbientLight>	m_AmbientLight;
		SO<BehaviorCallback>	m_cbInput;		// callback to receive input from canvas
		SO<BehaviorCallback>	m_cbUpdate;		// callback to receive timer events for frame stepping
		int			m_iFramerate;		// frame counter to allow frame rate checking
		double		m_dLastSecond;		// auxiliary variable for frame rate checking
		double		m_dLastUpdateTime;	// auxiliary variable for frame stepping
		double		m_FPS;		// desired frame rate
		Arena		m_Arena;	// Arena object
		Blobb*		m_Blobb;	// list of Blobbs
	};
}

#endif	// #ifndef BV3D_GAME