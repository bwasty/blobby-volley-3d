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
class GUI;

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

namespace Hydrax {
	class Hydrax;
}

class OGRE3DRenderSystem;
class OGRE3DRenderable;

/** Basic initialization of Ogre, Plug-ins and additional libraries. Mostly independent from the concrete application and originally taken from Tutorials/Demos (but greatly changed meanwhile...). 
	Subclasses should at least override fillScene() for setting up the visual scene... */
class BaseApplication : public Ogre::WindowEventListener
{
protected:
	BaseApplication();
public:
    virtual void go();
    virtual ~BaseApplication();

	inline Ogre::RenderWindow* getRenderWindow() { return mWindow; }
	inline Ogre::SceneManager* getSceneManager() { return mSceneMgr; }
	inline Ogre::ImprovedConfigFile& getConfig() { return mConfig; }
	inline NxOgre::World* getPhysicsWorld() { return mPhysicsWorld; }
	inline NxOgre::TimeController* getPhysicsTimeController() const { return mPhysicsTimeController; }
	inline OGRE3DRenderSystem* getPhysicsRenderSystem() const { return mPhysicsRenderSystem; }
	inline NxOgre::VisualDebugger* getVisualDebugger() const { return mVisualDebugger; }
	inline Ogre::SceneNode* getVisualDebuggerNode() const { return mVisualDebuggerNode; }

	// WindowEventListener
	void windowResized(Ogre::RenderWindow* rw);
	void windowClosed(Ogre::RenderWindow* rw);

protected:
	Ogre::ImprovedConfigFile mConfig;

	Ogre::Root *mRoot;
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

	Hydrax::Hydrax* mHydrax;


    virtual void defineResources();
    virtual void setupRenderSystem();
    virtual void initializeResourceGroups();
    virtual void setupScene();
	virtual void setupPhysics();
	virtual void setupHydrax();
	virtual void fillScene()=0;
    virtual void setupInputSystem();
	virtual void setupGUI()=0;
	virtual void setupListener()=0;
};
