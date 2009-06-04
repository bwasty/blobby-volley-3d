#include "Controls.h"
#include <OgreRoot.h>
#include <OgreConfigFile.h>
#include <OgreTextureManager.h>
#include <NxOgre.h>
#include <OGRE3DRenderSystem.h>

#include "BaseApplication.h"

using namespace Ogre;

void BaseApplication::go() {
    createRoot();
    defineResources();
    setupRenderSystem();
    createRenderWindow();
    initializeResourceGroups();
    setupScene();
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
	//delete mNxWorld;
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
	if (!mRoot->restoreConfig() && !mRoot->showConfigDialog())
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
	mRoot->initialise(true, "Tutorial Render Window");

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

void BaseApplication::setupPhysics() {
	//mNxWorld = new NxOgre::World("time-controller:ogre, log:yes");
	//mNxScene = mNxWorld->createScene("NxOgreScene", mSceneMgr, "gravity:yes, floor:yes, renderer:ogre"); //TODO: several scenes for several arenas?

	mNxWorld = NxOgre::World::createWorld();
	NxOgre::SceneDescription description;
	description.mGravity.y = -9.81f; // -9.81 m/s
	mNxScene = mNxWorld->createScene(description); //TODO!: NxScene needed to be publicly available? mNxRenderSystem should be enough
	mNxRenderSystem = new OGRE3DRenderSystem(mNxScene);
	
	mNxTimeController = NxOgre::TimeController::getSingleton();

	//mNxWorld->createDebugRenderer(mSceneMgr); //TODO:!!make switchable via keyboard...?
	//mNxWorld->getPhysXDriver()->createDebuggerConnection();

}

void BaseApplication::setupInputSystem() {

	//TODO: currently done by ControlsListener, remove that later?
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
	//mListener = new ExitListener(mKeyboard);
	//mListener = new ControlsListener(mRoot->getAutoCreatedWindow(), mCamera, mSceneMgr);
 //   mRoot->addFrameListener(mListener);
}

void BaseApplication::startRenderLoop() {
	mRoot->startRendering();

	// Do not add this to the application
   //while (mRoot->renderOneFrame())
   //{
   //    // Do some things here, like sleep for x milliseconds or perform other actions.
   //}
}


