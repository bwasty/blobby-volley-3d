#ifndef BV3D_GAME
#define BV3D_GAME

#include <vrs/sharedobj.h>
#include <vrs/glut/glutcanvas.h>
#include <vrs/camera.h>
#include <vrs/sg/scenething.h>
#include <vrs/ambientlight.h>
#include <vrs/sg/behaviorcallback.h>
#include <vrs/opengl/transparencytechniquegl.h>
#include <vrs/container/array.h>
#include <vrs/sg/interactionmode.h>
#include <vrs/sg/interactionconcept.h>
#include <vrs/opengl/imagecubemaptexturegl.h>
//#include <vrs/image/compressedimage.h>
#include <vrs/io/pngreader.h>
#include <vrs/lookat.h>
#include <vrs/perspective.h>
#include <vrs/opengl/texgengl.h>
#include <vrs/sphere.h>

//#include "Blobb.h"
//#include "Ball.h"
//#include "Arena.h"
//#include "Referee.h"

using namespace VRS;

namespace VRS {
	class JumpNavigation;
	class BackgroundGL;
	
}

namespace BV3D
{
	class Arena;
	class Referee;
	class Ball;
	class Blobb;

	class Game : public SharedObj {
	public:
		Game();
		~Game();
		void update();			// render new frame
		void processInput();	// process and dispatch input
		VRS::SO<BV3D::Ball> getBall() {return m_Ball;}
		VRS::SO<BV3D::Referee> getReferee() {return m_Referee;}

	protected:
		SO<GlutCanvas>			m_Canvas;		// main canvas
		SO<SceneThing>			m_RootScene;	// root node where Blobbs etc will be appended to
		SO<TransparencyTechniqueGL>	m_TransparencyTechnique;	// to enable transparency
		SO<Perspective>			m_perspective;
		SO<LookAt>				m_lookAt;
		SO<Camera>				m_Camera;			// scene camera
		SO<AmbientLight>		m_AmbientLight;
		SO<BehaviorCallback>	m_cbInput;		// callback to receive input from canvas
		SO<BehaviorCallback>	m_cbUpdate;		// callback to receive timer events for frame stepping
		SO<InteractionMode>		m_InteractionMode;	//needed for navigation technique
		SO<InteractionConcept>	m_InteractionConcept;	//needed for navigation technique
		SO<JumpNavigation>		m_Navigation;	// select and fly between different camera settings
		SO<BackgroundGL>		m_Background;		//SceneNode for background
		SO<VRS::ImageCubeMapTextureGL> m_BackCubeMap;	//CubeMap for background

		int			m_iFramerate;		// frame counter to allow frame rate checking
		double		m_dLastSecond;		// auxiliary variable for frame rate checking
		double		m_dLastUpdateTime;	// auxiliary variable for frame stepping
		double		m_FPS;		// desired frame rate
		VRS::SO<Arena>	m_Arena;	// Arena object
		VRS::SO<Array<SO<Blobb>>>	m_BlobbArray;	// list of Blobbs
		VRS::SO<BV3D::Ball>		m_Ball;
		VRS::SO<BV3D::Referee>	m_Referee;


		void initBackgroundCubeMap();
	};
}

#endif	// #ifndef BV3D_GAME
