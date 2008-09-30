#ifndef BV3D_BASEAPPLICATION
#define BV3D_BASEAPPLICATION

#include <Ogre.h>
#include <OIS/OIS.h>
#include <CEGUI/CEGUI.h>
#include <OgreCEGUIRenderer.h> //TODO: comes from OgreSDK/samples/include...what to do?

using namespace Ogre;

class ControlsListener;

// Basic initialisation of Ogre and Plugins. Mainly independent from the concrete application. 
// Subclasses should override setupScene()
class BaseApplication
{
public:
    void go();
    ~BaseApplication();

protected:
    Root *mRoot;
    OIS::Keyboard *mKeyboard;
    OIS::InputManager *mInputManager;
    CEGUI::OgreCEGUIRenderer *mRenderer;
    CEGUI::System *mSystem;
    ControlsListener *mListener;
	Camera *mCamera;
	SceneManager *mSceneMgr;

    void createRoot();
    void defineResources();
    void setupRenderSystem();
    void createRenderWindow();
    void initializeResourceGroups();
    virtual void setupScene()=0;
    void setupInputSystem();
    void setupCEGUI();
    void createFrameListener();
    void startRenderLoop();
};

#endif // #ifndef BV3D_BASEAPPLICATION