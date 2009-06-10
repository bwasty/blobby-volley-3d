#pragma once
#include "OgreImprovedConfigFile.h"

namespace Ogre {
	class Root;
	class Camera;
	class SceneManager;
	class SceneNode;
}

class ControlsListener;

namespace OIS {
	class Keyboard;
	class InputManager;
}

namespace NxOgre {
	class World;
	class TimeController;
	class Scene;
	class VisualDebugger;
}

class OGRE3DRenderSystem;
class OGRE3DRenderable;

// Basic initialisation of Ogre, Plugins and additional libraries. Mostly independent from the concrete application and originally taken from Tutorials/Demos. 
// Subclasses should at least override fillScene() for setting up the visual scene...
class BaseApplication
{
protected:
	BaseApplication();
public:
    virtual void go();
    ~BaseApplication();

	Ogre::ImprovedConfigFile& getConfig() { return mConfig; }
	NxOgre::World* getPhysicsWorld() { return mPhysicsWorld; }
	NxOgre::TimeController* getPhysicsTimeController() const { return mPhysicsTimeController; }
	OGRE3DRenderSystem* getPhysicsRenderSystem() const { return mPhysicsRenderSystem; }
	NxOgre::VisualDebugger* getVisualDebugger() const { return mVisualDebugger; }
	Ogre::SceneNode* getVisualDebuggerNode() const { return mVisualDebuggerNode; }

protected:
	Ogre::ImprovedConfigFile mConfig;

	Ogre::Root *mRoot;
    OIS::Keyboard *mKeyboard;
    OIS::InputManager *mInputManager;
    ControlsListener *mListener;
	Ogre::Camera *mCamera;
	Ogre::SceneManager *mSceneMgr;

	NxOgre::World* mPhysicsWorld;
	NxOgre::TimeController* mPhysicsTimeController;
	NxOgre::Scene* mPhysicsScene;
	OGRE3DRenderSystem* mPhysicsRenderSystem;
	NxOgre::VisualDebugger*	mVisualDebugger;
	
	OGRE3DRenderable*		mVisualDebuggerRenderable;
	Ogre::SceneNode*		mVisualDebuggerNode;

    void createRoot();
    void defineResources();
    void setupRenderSystem();
    void createRenderWindow();
    void initializeResourceGroups();
    void setupScene();
	void setupPhysics();
	virtual void fillScene() {};
    void setupInputSystem();
    virtual void createFrameListener();
    void startRenderLoop();
};

#pragma once