#include "BaseApplication.h"

//#include <OgreRoot.h>
//#include <OgreConfigFile.h>
//#include <OgreTextureManager.h>
//#include <NxOgre.h>
#include <OGRE3DRenderSystem.h> //TODO: Problem: OGRE3DRenderSystem.h includes Ogre.h
#include <OGRE3DRenderable.h>

#include <OIS\OISInputManager.h>

#include "Constants.h"
#include "GUI.h"
#include "Controls.h"


using namespace Ogre;

BaseApplication::BaseApplication() : mConfig() {
	//const String pathToConfig = "../BV3D.cfg"; 
	mConfig.load(PATH_TO_CONFIG, "=\t:", true);

	/* dump of current default config
	ARENA_EXTENT=16.0 30.0 7.0
	BALL_RADIUS=0.9;
	LOOK_FROM=0.0 9.0 -16.0
	LOOK_TO=0.0 4.0 0.0
	NET_HEIGHT=4.2;
	*/

}

void BaseApplication::go() {
    mRoot = new Root(); // 3 optional params: plugins.cfg, config.cfg, logfile
    defineResources();
    setupRenderSystem();
    mWindow = mRoot->initialise(true, "Blobby Volley 3D 2.0"); // creates RenderWindow
    initializeResourceGroups();
    setupScene();
	setupPhysics();
	fillScene();
    setupInputSystem();

	mGUI = new GUI((Application*)this, mWindow); //TODO!: dirty cast

    createFrameListener();

    mRoot->startRendering();
}

BaseApplication::~BaseApplication() {
	//OIS
	//mInputManager->destroyInputObject(mKeyboard); //TODO: causes problem, maybe because of ExampleFrameListener?
    //OIS::InputManager::destroyInputSystem(mInputManager); //TODO: same as above

	//delete mControls;

	// delete NxOgre stuff -> see http://www.ogre3d.org/wiki/index.php/NxOgre_Tutorial_Usefull_Things
	//delete mPhysicsWorld;
	NxOgre::World::destroyWorld();

    //delete mRoot; // deletes also SceneManager, the RenderWindow and so on TODO: Problem: access violation on shutdown of D3D9 renderer
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
	//if (!mRoot->restoreConfig() && !mRoot->showConfigDialog())
 //      throw Exception(52, "User canceled the config dialog!", "Application::setupRenderSystem()");
	if (!mRoot->restoreConfig() || mConfig.getSettingBool("showConfigDialog")) {
		if (!mRoot->showConfigDialog())
			throw Exception(52, "User canceled the config dialog!", "Application::setupRenderSystem()");
	}


	//alternative to config file/dialog:
	// Do not add this to the application
    //RenderSystem *rs = mRoot->getRenderSystemByName("Direct3D9 Rendering Subsystem");
    //                                     // or use "OpenGL Rendering Subsystem"
    //mRoot->setRenderSystem(rs);
    //rs->setConfigOption("Full Screen", "No");
    //rs->setConfigOption("Video Mode", "800 x 600 @ 32-bit colour");
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

	Viewport *vp = mWindow->addViewport(mCamera);
	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));

	vp->setBackgroundColour(ColourValue(0.1,0.1,0.1));

	// camera position and orientation
	mCamera->setPosition(mConfig.getSettingVector3("LOOK_FROM"));
	mCamera->lookAt(mConfig.getSettingVector3("LOOK_TO"));
}



void BaseApplication::setupPhysics() {
	mPhysicsWorld = NxOgre::World::createWorld();
	//TODO: several PhysX scenes for several arenas?
	mPhysicsScene = mPhysicsWorld->createScene();
	mPhysicsRenderSystem = new OGRE3DRenderSystem(mPhysicsScene);
	
	mPhysicsTimeController = NxOgre::TimeController::getSingleton();

	mVisualDebugger = mPhysicsWorld->getVisualDebugger();
	mVisualDebuggerRenderable = new OGRE3DRenderable(NxOgre::Enums::RenderableType_VisualDebugger);
	mVisualDebugger->setRenderable(mVisualDebuggerRenderable);
	mVisualDebuggerNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mVisualDebuggerNode->attachObject(mVisualDebuggerRenderable);
	mVisualDebugger->setVisualisationMode(NxOgre::Enums::VisualDebugger_ShowNone);

	////change default PhysX material
	//mPhysicsScene->getMaterial(0)->setRestitution(0.1);
	//mPhysicsScene->getMaterial(0)->setDynamicFriction(0.5);
	//mPhysicsScene->getMaterial(0)->setStaticFriction(0.5);
}

void BaseApplication::setupInputSystem() {
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

	LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	mInputManager = OIS::InputManager::createInputSystem( pl );

	//Create all devices
	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

	//Set initial mouse clipping size
	windowResized(mWindow);

	//Register as a Window listener
	WindowEventUtilities::addWindowEventListener(mWindow, this);
}

//Adjust mouse clipping area
void BaseApplication::windowResized(RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void BaseApplication::windowClosed(RenderWindow* rw)
{
	//Only close for window that created OIS (the main window in these demos)
	if( rw == mWindow )
	{
		if( mInputManager )
		{
			mInputManager->destroyInputObject( mMouse );
			mInputManager->destroyInputObject( mKeyboard );

			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = 0;
		}
	}
}
