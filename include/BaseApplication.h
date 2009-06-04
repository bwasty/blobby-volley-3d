#pragma once

//#include <Ogre.h>
//#include <OIS/OIS.h>
//#include <NxOgre.h> // TODO: move to CPP?

//namespace Ogre {
//	class Root;
//	class Camera;
//	class SceneManager;
//}

class ControlsListener;

namespace OIS {
	class Keyboard;
	class InputManager;
}

//namespace NxOgre {
//	class World;
//	class TimeController;
//	class Scene;
//}

class OGRE3DRenderSystem;

// Basic initialisation of Ogre, Plugins and additional libraries. Mostly independent from the concrete application and taken from Tutorials/Demos. 
// Subclasses should at least override setupScene() for setting up the visual scene...
class BaseApplication
{
public:
    void go();
    ~BaseApplication();

	NxOgre::World* getNxWorld() { return mNxWorld; }
	NxOgre::TimeController* mNxTimeController;
	OGRE3DRenderSystem* mNxRenderSystem;

protected:
	Ogre::Root *mRoot;
    OIS::Keyboard *mKeyboard;
    OIS::InputManager *mInputManager;
    ControlsListener *mListener;
	Ogre::Camera *mCamera;
	Ogre::SceneManager *mSceneMgr;
	NxOgre::World* mNxWorld;
	NxOgre::Scene* mNxScene;
	

    void createRoot();
    void defineResources();
    void setupRenderSystem();
    void createRenderWindow();
    void initializeResourceGroups();
	void setupPhysics();
    virtual void setupScene()=0;
    void setupInputSystem();
    virtual void createFrameListener();
    void startRenderLoop();
};

#pragma once