#pragma once
#include "OgreImprovedConfigFile.h"
#include <OgreWindowEventUtilities.h>

namespace Ogre {
	class Root;
	class Camera;
	class SceneManager;
	class SceneNode;
}

class ControlsListener;

namespace OIS {
	class Keyboard;
	class Mouse;
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

// Basic initialization of Ogre, Plug-ins and additional libraries. Mostly independent from the concrete application and originally taken from Tutorials/Demos. 
// Subclasses should at least override fillScene() for setting up the visual scene...
class BaseApplication : public Ogre::WindowEventListener
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

	// WindowEventListener
	void windowResized(Ogre::RenderWindow* rw);
	void windowClosed(Ogre::RenderWindow* rw);

protected:
	Ogre::ImprovedConfigFile mConfig;

	Ogre::Root *mRoot;
    ControlsListener *mListener;
	Ogre::Camera *mCamera;
	Ogre::SceneManager *mSceneMgr;
	Ogre::RenderWindow* mWindow;

	// NxOgre
	NxOgre::World* mPhysicsWorld;
	NxOgre::TimeController* mPhysicsTimeController;
	NxOgre::Scene* mPhysicsScene;
	OGRE3DRenderSystem* mPhysicsRenderSystem;
	NxOgre::VisualDebugger*	mVisualDebugger;
	OGRE3DRenderable*		mVisualDebuggerRenderable;
	Ogre::SceneNode*		mVisualDebuggerNode;

	//OIS Input devices
	OIS::InputManager* mInputManager;
	OIS::Mouse*    mMouse;
	OIS::Keyboard* mKeyboard;

    void defineResources();
    void setupRenderSystem();
    void initializeResourceGroups();
    void setupScene();
	void setupPhysics();
	virtual void fillScene() {};
    void setupInputSystem();
    virtual void createFrameListener()=0;
};

#pragma once