#pragma once

#include <Ogre.h>
#include <OIS/OIS.h>
//#include <CEGUI/CEGUI.h>
//#include <OgreCEGUIRenderer.h> //TODO: comes from OgreSDK/samples/include...what to do?
#include <NxOgre.h> // TODO: move to CPP?

using namespace Ogre;

class ControlsListener;

// Basic initialisation of Ogre, Plugins and additional libraries. Mostly independent from the concrete application and taken from Tutorials/Demos. 
// Subclasses should at least override setupScene() for setting up the visual scene...
class BaseApplication
{
public:
    void go();
    ~BaseApplication();

	NxOgre::World* getNxWorld() { return mNxWorld; }

protected:
    Root *mRoot;
    OIS::Keyboard *mKeyboard;
    OIS::InputManager *mInputManager;
    //CEGUI::OgreCEGUIRenderer *mRenderer;
    //CEGUI::System *mSystem;
    ControlsListener *mListener;
	Camera *mCamera;
	SceneManager *mSceneMgr;
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
    //void setupCEGUI();
    virtual void createFrameListener();
    void startRenderLoop();
};

#pragma once