#include "Controls.h"
#include <OgreRoot.h>
#include <OgreConfigFile.h>
#include <OgreTextureManager.h>
#include <NxOgre.h>
#include <OGRE3DRenderSystem.h> //TODO: Problem: OGRE3DRenderSystem.h includes Ogre.h
#include <OGRE3DRenderable.h>

#include "BaseApplication.h"

using namespace Ogre;

BaseApplication::BaseApplication() : mConfig() {
	const String pathToConfig = "../BV3D.cfg"; // TODO: working directory-independent solution?
	mConfig.load(pathToConfig, "=\t:", true);

	/* dump of current default config
	ARENA_EXTENT=16.0 30.0 7.0
	BALL_RADIUS=0.9;
	LOOK_FROM=0.0 9.0 -16.0
	LOOK_TO=0.0 4.0 0.0
	NET_HEIGHT=4.2;
	*/

}

void BaseApplication::go() {
    createRoot();
    defineResources();
    setupRenderSystem();
    createRenderWindow();
    initializeResourceGroups();
    setupScene();
	setupPhysics();
	fillScene();
    //setupInputSystem();
    createFrameListener();
    startRenderLoop();
}

BaseApplication::~BaseApplication() {
	//OIS
	//mInputManager->destroyInputObject(mKeyboard); //TODO: causes problem, maybe because of ExampleFrameListener?
    //OIS::InputManager::destroyInputSystem(mInputManager); //TODO: same as above

	delete mListener;

	// delete NxOgre stuff -> see http://www.ogre3d.org/wiki/index.php/NxOgre_Tutorial_Usefull_Things
	//delete mPhysicsWorld;
	NxOgre::World::destroyWorld();

    //delete mRoot; // deletes also SceneManager, the RenderWindow and so on TODO: Problem: access violation on shutdown of D3D9 renderer
}

void BaseApplication::createRoot() {
	mRoot = new Root(); // 3 optional params: plugins.cfg, config.cfg, logfile
}
    
void BaseApplication::defineResources() {
	String secName, typeName, archName;
    ConfigFile cf;
    cf.load("resources.cfg");
	ConfigFile::SectionIterator seci = cf.getSectionIterator();
    while (seci.hasMoreElements())
    {
		secName = seci.peekNextKey();
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
        }
    }
}
    
void BaseApplication::setupRenderSystem() {
	//note: I would recommend that if you catch an exception during Ogre's startup that you 
	//      delete the ogre.cfg file in the catch block. It is possible that the settings they 
	//      have chosen in the config dialog has caused a problem and they need to change them.
	if (!mRoot->restoreConfig() && !mRoot->showConfigDialog()) //TODO!!: make configurable - show RenderSystem dialog + load cfg even if displayed
       throw Exception(52, "User canceled the config dialog!", "Application::setupRenderSystem()");

	//alternative to config file/dialog:
	// Do not add this to the application
    //RenderSystem *rs = mRoot->getRenderSystemByName("Direct3D9 Rendering Subsystem");
    //                                     // or use "OpenGL Rendering Subsystem"
    //mRoot->setRenderSystem(rs);
    //rs->setConfigOption("Full Screen", "No");
    //rs->setConfigOption("Video Mode", "800 x 600 @ 32-bit colour");
}
    
void BaseApplication::createRenderWindow() {
	mRoot->initialise(true, "Blobby Volley 3D 2.0");

	//Win32 alternative:
	// Do not add this to the application
   //mRoot->initialise(false);
   //HWND hWnd = 0;  // Get the hWnd of the application!
   //NameValuePairList misc;
   //misc["externalWindowHandle"] = StringConverter::toString((int)hWnd);
   //RenderWindow *win = mRoot->createRenderWindow("Main RenderWindow", 800, 600, false, &misc);
}

void BaseApplication::initializeResourceGroups() {
	TextureManager::getSingleton().setDefaultNumMipmaps(5);
    ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void BaseApplication::setupScene() {
	mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "Default SceneManager");
	mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

	mCamera = mSceneMgr->createCamera("Camera");
	mCamera->setNearClipDistance(1.0);

	Viewport *vp = mRoot->getAutoCreatedWindow()->addViewport(mCamera);
	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));

	vp->setBackgroundColour(ColourValue(0.1,0.1,0.1));

	// camera position and orientation
	mCamera->setPosition(mConfig.getSettingVector3("LOOK_FROM"));
	mCamera->lookAt(mConfig.getSettingVector3("LOOK_TO"));
}



void BaseApplication::setupPhysics() {
	mPhysicsWorld = NxOgre::World::createWorld();
	NxOgre::SceneDescription description;
	description.mGravity.y = -9.81f; // -9.81 m/s
	//TODO: several PhysX scenes for several arenas?
	mPhysicsScene = mPhysicsWorld->createScene(description);
	mPhysicsRenderSystem = new OGRE3DRenderSystem(mPhysicsScene);
	
	mPhysicsTimeController = NxOgre::TimeController::getSingleton();

	mVisualDebugger = mPhysicsWorld->getVisualDebugger();
	mVisualDebuggerRenderable = new OGRE3DRenderable(NxOgre::Enums::RenderableType_VisualDebugger);
	mVisualDebugger->setRenderable(mVisualDebuggerRenderable);
	mVisualDebuggerNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mVisualDebuggerNode->attachObject(mVisualDebuggerRenderable);
	mVisualDebugger->setVisualisationMode(NxOgre::Enums::VisualDebugger_ShowNone);

	//TODO: make configurable - default PhysX material
	mPhysicsScene->getMaterial(0)->setRestitution(0.5);
	mPhysicsScene->getMaterial(0)->setDynamicFriction(0.5);
	mPhysicsScene->getMaterial(0)->setStaticFriction(0.5);
}

void BaseApplication::setupInputSystem() {

	//TODO: setupInputSystem - currently done by ControlsListener, remove that later?
	//size_t windowHnd = 0;
 //   std::ostringstream windowHndStr;
 //   OIS::ParamList pl;
 //   RenderWindow *win = mRoot->getAutoCreatedWindow();

 //   win->getCustomAttribute("WINDOW", &windowHnd);
 //   windowHndStr << windowHnd;
 //   pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
 //   mInputManager = OIS::InputManager::createInputSystem(pl);

	//try
 //   {
 //       mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, false /* buffered input */));
 //       //mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, false));
 //       //mJoy = static_cast<OIS::JoyStick*>(mInputManager->createInputObject(OIS::OISJoyStick, false));
 //   }
 //   catch (const OIS::Exception &e)
 //   {
 //       throw Exception(42, e.eText, "Application::setupInputSystem");
 //   }
}

void BaseApplication::createFrameListener() {
}

void BaseApplication::startRenderLoop() {
	mRoot->startRendering();

	// Do not add this to the application
   //while (mRoot->renderOneFrame())
   //{
   //    // Do some things here, like sleep for x milliseconds or perform other actions.
   //}
}


