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
#include <vrs/pointlight.h>
#include <vrs/distantlight.h>
#include <vrs/sg/behaviorcallback.h>
#include <vrs/opengl/transparencytechniquegl.h>
#include <vrs/sg/interactionmode.h>
#include <vrs/sg/interactionconcept.h>
#include <vrs/opengl/imagecubemaptexturegl.h>
#include <vrs/io/pngreader.h>
#include <vrs/lookat.h>
#include <vrs/perspective.h>
#include <vrs/opengl/texgengl.h>
#include <vrs/color.h>
#include <vrs/opengl/shadowtechniquegl.h>
#include <vrs/shadowcaster.h>
#include <vrs/shadowed.h>
#include <vrs/sg/selector.h>

// temporarily used for floor
#include <vrs/box.h>

#include <Newton.h>

#include "Game.h"
#include "MouseControls.h"	// TODO: delete if no longer needed
#include "Ball.h"
//#include "Constants.h"
#include "Arena.h"
#include "Blobb.h"
#include "ClassicReferee.h"
#include "TieBreakReferee.h"
#include "SceneLoader.h"
#include "Menu.h"

BV3D::Game::Game() {
	m_Canvas = new GlutCanvas("BlobbyVolley3D",600,300);	// create the main window

	m_SceneLoader = new SceneLoader();
	m_RootScene = new SceneThing();

	// add transparency and shadow support to scene
	m_TransparencyTechnique = new TransparencyTechniqueGL();
	m_RootScene->append(m_TransparencyTechnique);
	m_RootScene->append(new VRS::ShadowTechniqueGL());

	m_perspective = new Perspective(30, 1.0, 1000.0);
	m_lookAt = new LookAt(Vector(0.0, 8.0, -15.0));
	m_Camera = new Camera(m_perspective, m_lookAt);
	m_RootScene->append(m_Camera);

	// do some global lighting
	m_AmbientLight = new AmbientLight(Color(0.7));
	m_RootScene->append(m_AmbientLight);

	//// add PointLight for shadows and reflection
	//m_PointLight = new PointLight(Vector(0, -20, 0));
	//m_RootScene->append(m_PointLight);

	// create directional light for shadows
	VRS::SO<VRS::DistantLight> topLight = new VRS::DistantLight(VRS::Vector(0.0,1.0,0.0), VRS::Color(0.5));
	m_RootScene->append(topLight);

	// create TEMPORARY floor
	VRS::SO<VRS::SceneThing> floorScene = new VRS::SceneThing(m_RootScene);
	floorScene->append(new VRS::Shadowed(topLight));
	floorScene->append(new VRS::Box(VRS::Vector(-BV3D::arenaExtent[0]/2,0.0,-BV3D::arenaExtent[2]/2),
									VRS::Vector(BV3D::arenaExtent[0]/2,0.01,BV3D::arenaExtent[2]/2)));
	
	// TODO: select specific referee via menu/config
	m_Referee = new BV3D::TieBreakReferee(this);

	m_Arena = new BV3D::Arena();
	m_RootScene->append(m_Arena->getScene());

	// init Blobbs and add them to the scene
	m_BlobbArray = new Array<SO<Blobb> >();
	SO<Blobb> blobb = new BV3D::Blobb(m_Arena, BV3D::BV3D_TEAM1);
	blobb->setPosition(Vector(-2.0,0.0,0.0));
	blobb->getScene()->prepend(new VRS::ShadowCaster(topLight));
	m_RootScene->append(blobb->getScene());
	m_BlobbArray->append(blobb);

	blobb = new BV3D::Blobb(m_Arena, BV3D::BV3D_TEAM2);
	blobb->setPosition(Vector(2.0,0.0,0.0));
	blobb->setControls(new BV3D::MouseControls());
	blobb->setColor(Color(0.0,0.0,1.0,0.4));
	blobb->getScene()->prepend(new VRS::ShadowCaster(topLight));
	m_RootScene->append(blobb->getScene());
	m_BlobbArray->append(blobb);

	//m_Arena->setExtent(Vector(8.0,10.0,6.0));

	m_Ball = new BV3D::Ball(m_Arena);
	m_Ball->getScene()->prepend(new VRS::ShadowCaster(topLight));
	m_RootScene->append(m_Ball->getScene());
	m_Ball->resetPosition(Vector(0.0,3.5,0.0));
	
	m_Canvas->append(m_RootScene);

	m_Arena->setupMaterials(this);

	mMenu = new Menu(this);
	switchToMenu();

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

BV3D::Game::~Game() {
	delete[] m_BlobbArray;
}

/**
 * update() is called periodically on timer events to redisplay the whole scene
 */
void BV3D::Game::update() {
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

		if(!mMenu->isActive())	// simulate world only if not in menu mode
			m_Arena->updateWorld(timestep);
		m_Canvas->redisplay();
	}
}

/**
 * evaluates incoming InputEvents from canvas, processes and dispatches them
 */
void BV3D::Game::processInput() {
	InputEvent* ie = VRS_Cast(InputEvent, m_cbInput->currentCanvasEvent());
	if(ie==NULL)
		return;

	if(mMenu->isActive()) return;	// process input only if not in menu mode

	// process general controls (pausing, camera positioning,...)
	KeyEvent* ke = VRS_Cast(VRS::KeyEvent, ie);
	if(ke != NULL)
		if(ke->pressed()) {
			switch (ke->keyCode())
			{
				case Key::Escape:
					switchToMenu();
					return;
				case Key::F1:
					initBackgroundCubeMap();
					break;
				case Key::F2:	//view field from the side
					m_Navigation->initPath(Vector(0.0, 10.0, -15.0), Vector(0.0, -10.0, 15.0));
					m_BlobbArray->getElement(0)->setCtrlsOrientation(VRS::Vector(0.0, 0.0, 1.0));
					m_BlobbArray->getElement(1)->setCtrlsOrientation(VRS::Vector(0.0, 0.0, 1.0));
					break;
				case Key::F3:	//view field from above
					m_Navigation->initPath(Vector(0.0, 15.0, -0.1), Vector(0.0, -15.0, 0.1));
					m_BlobbArray->getElement(0)->setCtrlsOrientation(VRS::Vector(0.0, 0.0, 1.0));
					m_BlobbArray->getElement(1)->setCtrlsOrientation(VRS::Vector(0.0, 0.0, 1.0));
					//printf("Key F2 pressed\n");
					break;
				case Key::F4:	//view field from the front(from baseline of one blobb's field)
					m_Navigation->initPath(Vector(15.0, 15.0, 0.0), Vector(-15.0, -15.0, 0.0));
					m_BlobbArray->getElement(0)->setCtrlsOrientation(VRS::Vector(-1.0, 0.0, 0.0));
					m_BlobbArray->getElement(1)->setCtrlsOrientation(VRS::Vector(-1.0, 0.0, 0.0));
					break;
				case Key::F5:	//view field from the side "lying on the ground"
					m_Navigation->initPath(Vector(0.0, 0.0, -15.0), Vector(0.0, 3.0, 15.0));
					m_BlobbArray->getElement(0)->setCtrlsOrientation(VRS::Vector(0.0, 0.0, 1.0));
					m_BlobbArray->getElement(1)->setCtrlsOrientation(VRS::Vector(0.0, 0.0, 1.0));
					break;
				case Key::F6:
					m_Navigation->initPath(Vector(0.0, 10.0, 15.0), Vector(0.0, -10.0, -15.0));
					m_BlobbArray->getElement(0)->setCtrlsOrientation(VRS::Vector(0.0, 0.0, -1.0));
					m_BlobbArray->getElement(1)->setCtrlsOrientation(VRS::Vector(0.0, 0.0, -1.0));
					break;
				case Key::F7:	//view field from 
					m_Navigation->initPath(Vector(-15.0, 15.0, 0.0), Vector(15.0, -15.0, 0.0));
					m_BlobbArray->getElement(0)->setCtrlsOrientation(VRS::Vector(1.0, 0.0, 0.0));
					m_BlobbArray->getElement(1)->setCtrlsOrientation(VRS::Vector(1.0, 0.0, 0.0));
					break;
				case Key::F8:	//view field from center in -x direction
					m_Navigation->initPath(Vector(0.0, 0.0, 0.0), Vector(-1.0, 1.0, 0.0));
					m_BlobbArray->getElement(0)->setCtrlsOrientation(VRS::Vector(1.0, 0.0, 0.0));
					m_BlobbArray->getElement(1)->setCtrlsOrientation(VRS::Vector(1.0, 0.0, 0.0));
					break;	
				case Key::F9:	//view field from center in -z direction
					m_Navigation->initPath(Vector(0.0, 0.0, 0.0), Vector(0.0, 1.0, -1.0));
					m_BlobbArray->getElement(0)->setCtrlsOrientation(VRS::Vector(1.0, 0.0, 0.0));
					m_BlobbArray->getElement(1)->setCtrlsOrientation(VRS::Vector(1.0, 0.0, 0.0));
					break;
				case Key::F10:	//view field from center in x direction
					m_Navigation->initPath(Vector(0.0, 0.0, 0.0), Vector(1.0, 1.0, 0.0));
					m_BlobbArray->getElement(0)->setCtrlsOrientation(VRS::Vector(1.0, 0.0, 0.0));
					m_BlobbArray->getElement(1)->setCtrlsOrientation(VRS::Vector(1.0, 0.0, 0.0));
					break;
				case Key::F11:	//view field from center in z direction
					m_Navigation->initPath(Vector(0.0, 0.0, 0.0), Vector(0.0, 1.0, 1.0));
					m_BlobbArray->getElement(0)->setCtrlsOrientation(VRS::Vector(1.0, 0.0, 0.0));
					m_BlobbArray->getElement(1)->setCtrlsOrientation(VRS::Vector(1.0, 0.0, 0.0));
					break;
				case Key::Insert:	//load surroundings
					printf("Loading beach...\n");
					m_RootScene->append(m_SceneLoader->loadBeach());
			}
			printf("Key %i pressed\n", ke->keyCode());
		}

	// pass input to blobbs
		m_BlobbArray->getElement(0)->processInput(ie);
	m_BlobbArray->getElement(1)->processInput(ie);
}

void BV3D::Game::initBackgroundCubeMap()
{
	printf("Loading Background Cupemap...\n");
	SO<Array<VRS::SO<VRS::Image> > > cubemapImages = new VRS::Array<SO<Image> >(6);
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Right] = VRS_GuardedLoadObject(Image, BV3D::cubemapsPath + "waterscape_cubemap/waterscape_posx.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Left] = VRS_GuardedLoadObject(Image, BV3D::cubemapsPath + "waterscape_cubemap/waterscape_negx.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Top] = VRS_GuardedLoadObject(Image, BV3D::cubemapsPath + "waterscape_cubemap/waterscape_posy.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Bottom] = VRS_GuardedLoadObject(Image, BV3D::cubemapsPath + "waterscape_cubemap/waterscape_negy.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Front] = VRS_GuardedLoadObject(Image, BV3D::cubemapsPath + "waterscape_cubemap/waterscape_posz.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Back] = VRS_GuardedLoadObject(Image, BV3D::cubemapsPath + "waterscape_cubemap/waterscape_negz.png");

	m_BackCubeMap = new VRS::ImageCubeMapTextureGL(cubemapImages->newIterator());
	//m_BackNode->append(new TexGenGL(TexGenGL::EyeLocal));//Spherical, ReflectionMap, Object, Eye, Object, EyeLocal
	//m_BackNode->append(new Sphere(19.0));
	m_Background = new VRS::BackgroundGL(m_BackCubeMap);
	m_RootScene->append(m_Background);

	///m_BackCubeMap = new ImageCubeMapTextureGL(cubemapImages->newIterator());
	///m_BackNode = new SceneThing(m_RootScene);
    ///m_BackNode->append(m_BackCubeMap);
	///m_BackNode->append(new TexGenGL(TexGenGL::EyeLocal));//Spherical, ReflectionMap, Object, Eye, Object, EyeLocal
	///m_BackNode->append(new Sphere(19.0));
}

void BV3D::Game::newServe() {
	BV3D::BV3D_TEAM team = m_Referee->getServingTeam();

	// Reset ball -> setForce into applyForceAndTorque-Callback?
	dFloat nullForce[3] = {0.0f, 0.0f, 0.0f};
	NewtonBodySetForce(m_Ball->getBody(), nullForce);
	//NewtonBodySetTorque(collData->ball->getBody(), nullForce);
	//collData->ball->setLocked(true);
	m_Ball->resetPosition(m_Arena->getTeamBounds(team).center());
}

// called by menu
void BV3D::Game::switchToGame(bool bRestart) {
	// replace menu scene with root scene
	if(m_Canvas->contains(mMenu->getScene())) m_Canvas->remove(mMenu->getScene());
	if(m_Canvas->contains(mMenu->getSelector())) m_Canvas->remove(mMenu->getSelector());
	if(!m_Canvas->contains(m_RootScene)) m_Canvas->append(m_RootScene);

	//if(bRestart) start new game
	//else resume old game
}

void BV3D::Game::switchToMenu() {
	// replace root scene with menu scene
	if(m_Canvas->contains(m_RootScene)) m_Canvas->remove(m_RootScene);
	mMenu->showMainMenu();
	if(!m_Canvas->contains(mMenu->getScene())) m_Canvas->append(mMenu->getScene());
	if(!m_Canvas->contains(mMenu->getSelector())) m_Canvas->append(mMenu->getSelector());
}
