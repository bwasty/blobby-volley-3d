/**
 * The Game class represents an instance of the BlobbyVolley3D game.
 * It is responsible for:
 * - creating and managing:
 *   - the obvious game objects: the Arena, the Blobbs, the Ball and the Net
 *   - the canvas(window) to which the scene is rendered
 *   - the root scene node
 *   - the scene camera
 * - hooking in/out the sub-scenes of the game objects
 * - processing and dispatching of input events from the canvas
 * - periodically issue rendering
 */

#include <vrs/time.h>
#include <vrs/sg/clock.h>
#include <vrs/sg/key.h>
#include <vrs/sg/keyevent.h>
#include <vrs/sg/jumpnavigation.h>
#include <vrs/opengl/backgroundgl.h>

#include <vrs/glut/glutcanvas.h>
#include <vrs/camera.h>
#include <vrs/sg/scenething.h>
#include <vrs/ambientlight.h>
#include <vrs/sg/behaviorcallback.h>
#include <vrs/opengl/transparencytechniquegl.h>
//#include <vrs/container/array.h>
#include <vrs/sg/interactionmode.h>
#include <vrs/sg/interactionconcept.h>
#include <vrs/opengl/imagecubemaptexturegl.h>
//#include <vrs/image/compressedimage.h>
#include <vrs/io/pngreader.h>
#include <vrs/lookat.h>
#include <vrs/perspective.h>
#include <vrs/opengl/texgengl.h>
#include <vrs/sphere.h>
#include <vrs/color.h>

#include <Newton.h>

#include "Game.h"
#include "MouseControls.h"	// TODO: delete if no longer needed
#include "Ball.h"
//#include "Constants.h"
#include "Arena.h"
#include "Blobb.h"
#include "ClassicReferee.h"
#include "TieBreakReferee.h"

using namespace BV3D;

Game::Game() {
	m_Canvas = new GlutCanvas("BlobbyVolley3D",600,300);	// create the main window

	m_RootScene = new SceneThing();

	// add transparency support to scene
	m_TransparencyTechnique = new TransparencyTechniqueGL();
	m_RootScene->append(m_TransparencyTechnique);

	m_perspective = new Perspective(30, 1.0, 1000.0);
	m_lookAt = new LookAt(Vector(0.0, 8.0, -15.0));
	m_Camera = new Camera(m_perspective, m_lookAt);
	m_RootScene->append(m_Camera);

	// do some global lighting
	m_AmbientLight = new AmbientLight(Color(0.7));
	m_RootScene->append(m_AmbientLight);

	// add PointLight for shadows and reflection
	m_PointLight = new PointLight(Vector(0, -20, 0));
	m_RootScene->append(m_PointLight);

	// TODO: select specific referee via menu/config
	m_Referee = new BV3D::TieBreakReferee(this);

	m_Arena = new Arena();
	m_RootScene->append(m_Arena->getScene());

	// init Blobbs and add them to the scene
	m_BlobbArray = new Array<SO<Blobb> >();
	SO<Blobb> blobb = new Blobb(m_Arena, BV3D::BV3D_TEAM1);
	blobb->setPosition(Vector(-2.0,1.0,1.0));
	m_RootScene->append(blobb->getScene());
	m_BlobbArray->append(blobb);

	blobb = new Blobb(m_Arena, BV3D::BV3D_TEAM2);
	blobb->setPosition(Vector(2.0,2.0,2.0));
	blobb->setControls(new MouseControls());
	blobb->setColor(Color(0.0,0.0,1.0,0.4));
	m_RootScene->append(blobb->getScene());
	m_BlobbArray->append(blobb);

	m_Arena->setExtent(Vector(8.0,10.0,6.0));

	m_Ball = new BV3D::Ball(m_Arena);
	m_RootScene->append(m_Ball->getScene());
	m_Ball->resetPosition(Vector(0.0,3.5,0.0));

	m_Canvas->append(m_RootScene);

	m_Arena->setupMaterials(this);

	// init update callback
	m_FPS = 30.0;	// assuming 30 fps are desired
	m_cbUpdate = new BehaviorCallback();
	m_cbUpdate->setTimeCallback(new MethodCallback<Game>(this,&Game::update));
	m_Canvas->append(m_cbUpdate);

	// init input callback
	m_cbInput = new BehaviorCallback();
	m_cbInput->setCanvasCallback(new MethodCallback<Game>(this,&Game::processInput));
	m_Canvas->append(m_cbInput);

	// init BookmarkNavigation
	m_Navigation = new JumpNavigation(Vector(0.0, 1.0, 0.0), m_lookAt, 3.0);
	///m_Navigation = new BookmarkNavigation();
	///m_Navigation->setPathStyle(BookmarkNavigation::Simple);
	///m_InteractionMode = new InteractionMode();
	///m_InteractionMode->addInteractionTechnique(m_Navigation);
	///m_InteractionConcept = new InteractionConcept(m_lookAt);
	///m_InteractionConcept->addInteractionMode(m_InteractionMode);
	///m_InteractionConcept->activate(BehaviorNode::Begin);
	m_Canvas->append(m_Navigation);//m_InteractionConcept);
}

Game::~Game() {
	delete[] m_BlobbArray;
}

/**
 * update() is called periodically on timer events to redisplay the whole scene
 */
void Game::update() {
	VRSTime time = m_Canvas->clock()->time();

	// test if current frame's time is over (0.8/m_FPS seems to be a good approximation)
	if(double(time) - m_dLastUpdateTime >= 0.8/m_FPS) {
		float timestep = (double)time - m_dLastUpdateTime;
		m_dLastUpdateTime = double(time);

		// count frames per second
		m_iFramerate++;
		if(m_dLastUpdateTime - m_dLastSecond >= 1.0) {
			printf("framerate: %d\n",m_iFramerate);
			m_dLastSecond = m_dLastUpdateTime;
			m_iFramerate = 0;
		}

		// update blobbs'
		//Iterator it = m_BlobbArray->
		//m_BlobbArray->getElement(0)->update();
		//m_BlobbArray->getElement(1)->update();

		m_Arena->updateWorld(timestep);
		m_Canvas->redisplay();
	}
}

/**
 * evaluates incoming InputEvents from canvas, processes and dispatches them
 */
void Game::processInput() {
	InputEvent* ie = VRS_Cast(InputEvent, m_cbInput->currentCanvasEvent());
	if(ie==NULL)
		return;

	// process general controls (pausing, camera positioning,...)
	KeyEvent* ke = VRS_Cast(KeyEvent, ie);
	if(ke != NULL)
		if(ke->pressed()) {
			switch (ke->keyCode())
			{
				case Key::Escape:
					exit(0);
					break;
				case Key::Home:
						m_Arena->setExtent(Vector(6.0,2.0,2.0));
					break;
				case Key::F1:
					initBackgroundCubeMap();
					break;
				case Key::F2:	//view field from the side
					m_Navigation->initPath(Vector(0.0, 10.0, -15.0), Vector(0.0, -10.0, 15.0));
					break;
				case Key::F3:	//view field from above
					m_Navigation->initPath(Vector(0.0, 15.0, -0.1), Vector(0.0, -15.0, 0.1));
					//printf("Key F2 pressed\n");
					break;
				case Key::F4:	//view field from the front(from baseline of one blobb's field)
					m_Navigation->initPath(Vector(15.0, 15.0, 0.0), Vector(-15.0, -15.0, 0.0));
					break;
				case Key::F5:	//view field from the side "lying on the ground"
					m_Navigation->initPath(Vector(0.0, 0.0, -15.0), Vector(0.0, 3.0, 15.0));
					break;
			}
			printf("Key %i pressed\n", ke->keyCode());
		}

	// pass input to blobbs
		m_BlobbArray->getElement(0)->processInput(ie);
	m_BlobbArray->getElement(1)->processInput(ie);
}

void Game::initBackgroundCubeMap()
{
	printf("Loading Background Cupemap...\n");
	SO<Array<SO<Image> > > cubemapImages = new Array<SO<Image> >(6);
    (*cubemapImages)[ImageCubeMapTextureGL::Right] = VRS_GuardedLoadObject(Image, "waterscape_cubemap/waterscape_posx.png");
	(*cubemapImages)[ImageCubeMapTextureGL::Left] = VRS_GuardedLoadObject(Image, "waterscape_cubemap/waterscape_negx.png");
	(*cubemapImages)[ImageCubeMapTextureGL::Top] = VRS_GuardedLoadObject(Image, "waterscape_cubemap/waterscape_posy.png");
	(*cubemapImages)[ImageCubeMapTextureGL::Bottom] = VRS_GuardedLoadObject(Image, "waterscape_cubemap/waterscape_negy.png");
	(*cubemapImages)[ImageCubeMapTextureGL::Front] = VRS_GuardedLoadObject(Image, "waterscape_cubemap/waterscape_posz.png");
	(*cubemapImages)[ImageCubeMapTextureGL::Back] = VRS_GuardedLoadObject(Image, "waterscape_cubemap/waterscape_negz.png");

	m_BackCubeMap = new ImageCubeMapTextureGL(cubemapImages->newIterator());
	//m_BackNode->append(new TexGenGL(TexGenGL::EyeLocal));//Spherical, ReflectionMap, Object, Eye, Object, EyeLocal
	//m_BackNode->append(new Sphere(19.0));
	m_Background = new BackgroundGL(m_BackCubeMap);
	m_RootScene->append(m_Background);

	///m_BackCubeMap = new ImageCubeMapTextureGL(cubemapImages->newIterator());
	///m_BackNode = new SceneThing(m_RootScene);
    ///m_BackNode->append(m_BackCubeMap);
	///m_BackNode->append(new TexGenGL(TexGenGL::EyeLocal));//Spherical, ReflectionMap, Object, Eye, Object, EyeLocal
	///m_BackNode->append(new Sphere(19.0));
}

void Game::newServe() {
	BV3D::BV3D_TEAM team = m_Referee->getServingTeam();

	// Reset ball -> setForce into applyForceAndTorque-Callback?
	dFloat nullForce[3] = {0.0f, 0.0f, 0.0f};
	NewtonBodySetForce(m_Ball->getBody(), nullForce);
	//NewtonBodySetTorque(collData->ball->getBody(), nullForce);
	//collData->ball->setLocked(true);
	m_Ball->resetPosition(m_Arena->getTeamBounds(team).center());
}