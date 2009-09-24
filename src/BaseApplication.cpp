#include "BaseApplication.h"

//#include <OgreRoot.h>
//#include <OgreConfigFile.h>
//#include <OgreTextureManager.h>
//#include <NxOgre.h>
#include <OGRE3DRenderSystem.h>
#include <OGRE3DRenderable.h>

#include <OIS\OISInputManager.h>

#include <Hydrax/Hydrax.h>
#include <Hydrax/Modules/ProjectedGrid/ProjectedGrid.h>

#include "Constants.h"
#include "GUI.h"
#include "Controls.h"


using namespace Ogre;

BaseApplication::BaseApplication() : mConfig() {
	//const String pathToConfig = "../BV3D.cfg"; 
	mConfig.load(PATH_TO_CONFIG, "=\t:", true);
}

void BaseApplication::go() {
    mRoot = new Root(); // 3 optional params: plugins.cfg, config.cfg, logfile
    defineResources();
    setupRenderSystem();
    mWindow = mRoot->initialise(true, "Blobby Volley 3D 2.0"); // creates RenderWindow
    initializeResourceGroups();
    setupScene();
	setupPhysics();
	setupHydrax();
	fillScene();
    setupInputSystem();

	setupGUI();

    createFrameListener();

    mRoot->startRendering();
}

BaseApplication::~BaseApplication() {
	//OIS
	//mInputManager->destroyInputObject(mKeyboard); //TODO: causes problem, maybe because of ExampleFrameListener?
    //OIS::InputManager::destroyInputSystem(mInputManager); //TODO: same as above


	// delete NxOgre stuff -> see http://www.ogre3d.org/wiki/index.php/NxOgre_Tutorial_Usefull_Things
	// TODO!: deleting mVisualDebuggerNode causes problem when deleting root (-> scenemanager)
	//delete mVisualDebuggerNode;
	delete mPhysicsRenderSystem;
	NxOgre::World::destroyWorld();

    delete mRoot; // deletes also SceneManager, the RenderWindow and so on
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

void BaseApplication::setupHydrax() {
		// copied from Hydrax Demo
	    // Create Hydrax object
		mHydrax = new Hydrax::Hydrax(mSceneMgr, mCamera, mWindow->getViewport(0));

		// Create our projected grid module  
		Hydrax::Module::ProjectedGrid *mModule 
			= new Hydrax::Module::ProjectedGrid(// Hydrax parent pointer
			                                    mHydrax,
												// Noise module
			                                    new Hydrax::Noise::Perlin(/*Generic one*/),
												// Base plane
			                                    Ogre::Plane(Ogre::Vector3(0,1,0), Ogre::Vector3(0,0,0)),
												// Normal mode
												Hydrax::MaterialManager::NM_VERTEX,
												// Projected grid options
										        Hydrax::Module::ProjectedGrid::Options(/*264 /*Generic one*/));

		// Set our module
		mHydrax->setModule(static_cast<Hydrax::Module::Module*>(mModule));

		// Load all parameters from config file
		// Remarks: The config file must be in Hydrax resource group.
		// All parameters can be set/updated directly by code(Like previous versions),
		// but due to the high number of customizable parameters, since 0.4 version, Hydrax allows save/load config files.
		mHydrax->loadCfg("HydraxDemo.hdx");

		//mHydrax->setComponents(
  //          static_cast<Hydrax::HydraxComponent>(Hydrax::HYDRAX_COMPONENT_SUN        |
  //                                               Hydrax::HYDRAX_COMPONENT_FOAM       |
  //                                               Hydrax::HYDRAX_COMPONENT_DEPTH      |
  //                                               Hydrax::HYDRAX_COMPONENT_SMOOTH     |
  //                                               Hydrax::HYDRAX_COMPONENT_CAUSTICS   |
  //                                   Hydrax::HYDRAX_COMPONENT_UNDERWATER |
  //                                   Hydrax::HYDRAX_COMPONENT_UNDERWATER_REFLECTIONS |
  //                                   Hydrax::HYDRAX_COMPONENT_UNDERWATER_GODRAYS));

        // Create water
        mHydrax->create();

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
