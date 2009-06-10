#include <OgreRenderWindow.h>
#include <OgreCamera.h>
#include <OgreSceneManager.h>

#include <NxOgre.h>
#include <OGRE3DBody.h>

#include "Application.h"
#include "Blobb.h"
#include "Ball.h"
#include "Console.h"
#include "Controls.h"

using namespace Ogre;

// Code originally taken from http://www.ogre3d.org/wiki/index.php/BasicTutorial5Source
ControlsListener::ControlsListener(RenderWindow* win, Camera* cam, SceneManager *sceneMgr, Application* app)
	: mCamera(cam), mWindow(win), mSceneMgr(sceneMgr), mStatsOn(true), mApp(app), mDirection(Vector3::ZERO),
	mControlBothBlobbs(false), mIsPhysicsVisualDebuggerOn(false), mGuiMode(false), mContinueRendering(true), mDebugText("Press SPACE to enter GUI mode")
{
	//TODO!: refactor: move OIS and MyGUI init to BaseApplication
	//TODO!: split Controls into FrameListener, GUI? Custom RenderLoop?
	//copied needed parts from ExampleFrameListener constructor
	using namespace OIS;

	mDebugOverlay = OverlayManager::getSingleton().getByName("Core/DebugOverlay");
	mDebugOverlay->remove2D(mDebugOverlay->getChild("Core/LogoPanel"));

	LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
	ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	win->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	mInputManager = InputManager::createInputSystem( pl );

	//Create all devices
	mKeyboard = static_cast<Keyboard*>(mInputManager->createInputObject( OISKeyboard, true ));
	mMouse = static_cast<Mouse*>(mInputManager->createInputObject( OISMouse, true ));

	//Set initial mouse clipping size
	windowResized(mWindow);

	showDebugOverlay(true);

	//Register as a Window listener
	WindowEventUtilities::addWindowEventListener(mWindow, this);

	////// end copy from ExampleFrameListener

    // set the rotation and move speed
    mRotate = 0.10;
    mMove = 10;

    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);

	// setup MyGUI TODO: right place to setup MyGUI?
	mGUI = new MyGUI::Gui();
	mGUI->initialise(mWindow);
	mGUI->hidePointer();

	// MyGUI help tooltip TODO: extra function for gui creation
	//TODO: HelpWidget: pin help tooltip on click?
	Ogre::UTFString _text("Controls for Blobby Volley 3D:\n\
SPACE         switch between GUI and play mode\n\
W,A,S,D,Q,E   Move camera (Alternative: Cursors)\n\
P             Toggle PhysX Debug Renderer\n\
B			  Toggle control of both blobbs\n\
1			  Reset ball\n\
F			  Toggle display of frame stats\n\
R			  Cycle Rendering Modes: normal, wireframe, point\n\
^			  Toggle Console");
	MyGUI::EditPtr text = nullptr;
	MyGUI::WidgetPtr panel = mGUI->createWidget<MyGUI::Widget>("PanelSmall", mGUI->getViewWidth(), -128, 400, 178, MyGUI::Align::Default, "Statistic");
	text = panel->createWidget<MyGUI::Edit>("WordWrapSimple", 10, 10, 380, 158, MyGUI::Align::Default);
	//text->setTextColour(MyGUI::Colour(0, 1, 0, 1));
	MyGUI::StaticImagePtr image = panel->createWidget<MyGUI::StaticImage>(MyGUI::WidgetStyle::Popup, "StaticImage", MyGUI::IntCoord(mGUI->getViewWidth()-48, 0, 48, 48), MyGUI::Align::Default, "Back");
	image->setItemResource("pic_CoreMessageIcon");
	image->setItemGroup("Icons");
	image->setItemName("Quest");

	MyGUI::ControllerEdgeHide * controller = new MyGUI::ControllerEdgeHide(0.5);
	MyGUI::ControllerManager::getInstance().addItem(panel, controller);
	text->setCaption(_text);


	// create console
	mConsole = new Console();
	//mConsole->setVisible(false);

	mConsole->registerConsoleDelegate("help", MyGUI::newDelegate(this, &ControlsListener::consoleCommand));
	mConsole->registerConsoleDelegate("config", MyGUI::newDelegate(this, &ControlsListener::consoleCommand));
	mConsole->registerConsoleDelegate("config_save", MyGUI::newDelegate(this, &ControlsListener::consoleCommand));
	mConsole->registerConsoleDelegate("config_load", MyGUI::newDelegate(this, &ControlsListener::consoleCommand));
	mConsole->registerConsoleDelegate("clear", MyGUI::newDelegate(this, &ControlsListener::consoleCommand));
}

bool ControlsListener::frameStarted(const FrameEvent &evt) {
	mGUI->injectFrameEntered(evt.timeSinceLastFrame);

	return mContinueRendering;
}

bool ControlsListener::frameRenderingQueued(const FrameEvent &evt)
{
    if(mMouse)
        mMouse->capture();
    if(mKeyboard) 
        mKeyboard->capture();

    //mCamNode->translate(mDirection * evt.timeSinceLastFrame, Node::TS_LOCAL);
	mCamera->moveRelative(mDirection * evt.timeSinceLastFrame); // TODO: translate/moveRelative: does it the same as above?

	mApp->getPhysicsTimeController()->advance(evt.timeSinceLastFrame);//1.0f/60.0f);
	mApp->getVisualDebugger()->draw();
	mApp->getVisualDebuggerNode()->needUpdate();

    return mContinueRendering;
}

bool ControlsListener::frameEnded(const Ogre::FrameEvent &evt) {
	updateStats();
	return true;
}

// MouseListener
bool ControlsListener::mouseMoved(const OIS::MouseEvent &e)
{
	if (mGuiMode)
		mGUI->injectMouseMove(e);

    if (e.state.buttonDown(OIS::MB_Right))
    {
		mCamera->yaw(Degree(-mRotate * e.state.X.rel));
        mCamera->pitch(Degree(-mRotate * e.state.Y.rel));
    }
	else /*if (e.state.buttonDown(OIS::MB_Left))*/ {
		if (!mGuiMode) {
			// move blobb taking into account camera direction
			Vector3 mouseMovement(-e.state.X.rel, 0, -e.state.Y.rel); // ground is xz-plane
			Vector3 camDir = mCamera->getDirection();
			camDir.y = 0; // only interested in xz-plane, not sure if this makes a difference though
			Quaternion rotation = Vector3::UNIT_Z.getRotationTo(camDir); //mouseMovement works correct when camera looks to positive z, so get rotation to that vector 
			mouseMovement = rotation * mouseMovement;

			//TODO!!: make configurable - Blobb move force
			mouseMovement *= 15; // length of vector determines how big the force is (-> how far blobb is moved)
			
			mApp->getBlobb1()->move(Vector2(mouseMovement.x, mouseMovement.z)); 
			if (mControlBothBlobbs)
				mApp->getBlobb2()->move(Vector2(mouseMovement.x, mouseMovement.z));
		}
	}
    return true;
}

bool ControlsListener::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	if (mGuiMode)
		mGUI->injectMousePress(e, id);
	else {
		if (id == OIS::MB_Left) {
			float force = 10000; // TODO!!: make configurable - Blobb jump force
			mApp->getBlobb1()->jump(force);
			if (mControlBothBlobbs)
				mApp->getBlobb2()->jump(force);
		}
	}
	return true;
}

bool ControlsListener::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id) { 
	mGUI->injectMouseRelease(e, id);

	return true; 
}

// KeyListener
bool ControlsListener::keyPressed(const OIS::KeyEvent &e)
{
	bool processed = mGUI->injectKeyPress(e);
	if (processed) 
		return true;

    switch (e.key)
    {
    case OIS::KC_ESCAPE: 
        mContinueRendering = false;
        break;

    case OIS::KC_UP:
    case OIS::KC_W:
        mDirection.z -= mMove;
        break;

    case OIS::KC_DOWN:
    case OIS::KC_S:
        mDirection.z += mMove;
        break;

    case OIS::KC_LEFT:
    case OIS::KC_A:
        mDirection.x -= mMove;
        break;

    case OIS::KC_RIGHT:
    case OIS::KC_D:
        mDirection.x += mMove;
        break;

    case OIS::KC_PGDOWN:
    case OIS::KC_E:
        mDirection.y -= mMove;
        break;

    case OIS::KC_PGUP:
    case OIS::KC_Q:
        mDirection.y += mMove;
        break;
	case OIS::KC_GRAVE:
		mConsole->setVisible(!mConsole->isVisible());
		break;
    }
    return true;
}

bool ControlsListener::keyReleased(const OIS::KeyEvent &e)
{
	bool processed = mGUI->injectKeyRelease(e);
	if (processed) 
		return true;

	Vector3 p;

    switch (e.key)
    {
    case OIS::KC_UP:
    case OIS::KC_W:
        mDirection.z += mMove;
        break;

    case OIS::KC_DOWN:
    case OIS::KC_S:
        mDirection.z -= mMove;
        break;

    case OIS::KC_LEFT:
    case OIS::KC_A:
        mDirection.x += mMove;
        break;

    case OIS::KC_RIGHT:
    case OIS::KC_D:
        mDirection.x -= mMove;
        break;

    case OIS::KC_PGDOWN:
    case OIS::KC_E:
        mDirection.y += mMove;
        break;

    case OIS::KC_PGUP:
    case OIS::KC_Q:
        mDirection.y -= mMove;
        break;
	case OIS::KC_R:
		mSceneDetailIndex = (mSceneDetailIndex+1)%3 ;
		switch(mSceneDetailIndex) {
			case 0 : mCamera->setPolygonMode(PM_SOLID); break;
			case 1 : mCamera->setPolygonMode(PM_WIREFRAME); break;
			case 2 : mCamera->setPolygonMode(PM_POINTS); break;
		}
		break;
	case OIS::KC_F:
		mStatsOn = !mStatsOn;
		showDebugOverlay(mStatsOn);
		break;
	case OIS::KC_B:
		mControlBothBlobbs = !mControlBothBlobbs;
		break;
	case OIS::KC_P:
		if (!mIsPhysicsVisualDebuggerOn)
			mApp->getVisualDebugger()->setVisualisationMode(NxOgre::Enums::VisualDebugger_ShowAll);
		else
			mApp->getVisualDebugger()->setVisualisationMode(NxOgre::Enums::VisualDebugger_ShowNone);
		mIsPhysicsVisualDebuggerOn = !mIsPhysicsVisualDebuggerOn;
		break;
	case OIS::KC_1:
		mApp->getBall()->reset();
		break;		
	case OIS::KC_SPACE:
		mGuiMode ? mGUI->hidePointer() : mGUI->showPointer();
		mGuiMode = !mGuiMode;
		mDebugText = "";
		break;
    } // switch
    return true;
}


void ControlsListener::updateStats(void)
{
	static String currFps = "Current FPS: ";
	static String avgFps = "Average FPS: ";
	static String bestFps = "Best FPS: ";
	static String worstFps = "Worst FPS: ";
	static String tris = "Triangle Count: ";
	static String batches = "Batch Count: ";

	// update stats when necessary
	try {
		OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
		OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
		OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
		OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

		const RenderTarget::FrameStats& stats = mWindow->getStatistics();
		guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
		guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
		guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS)
			+" "+StringConverter::toString(stats.bestFrameTime)+" ms");
		guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS)
			+" "+StringConverter::toString(stats.worstFrameTime)+" ms");

		OverlayElement* guiTris = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
		guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));

		OverlayElement* guiBatches = OverlayManager::getSingleton().getOverlayElement("Core/NumBatches");
		guiBatches->setCaption(batches + StringConverter::toString(stats.batchCount));

		OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
		guiDbg->setCaption(mDebugText);
	}
	catch(...) { /* ignore */ }
}


//Adjust mouse clipping area
void ControlsListener::windowResized(RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void ControlsListener::windowClosed(RenderWindow* rw)
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

void ControlsListener::showDebugOverlay(bool show)
{
	if (mDebugOverlay)
	{
		if (show)
			mDebugOverlay->show();
		else
			mDebugOverlay->hide();
	}
}

void ControlsListener::consoleCommand(const Ogre::UTFString & key, const Ogre::UTFString & value) {
	//mConsole->addToConsole(key + "," + value);
    //mConsole->addToConsole("1", "2", "3");
	if (key == "clear")
		mConsole->clearConsole();
	else if (key == "config") {
		//mApp->mConfig.setSetting("MyNewSetting", Vector3(1.2, 2.3, 4.2)); //NOTE: literal Strings are interpreted as booleans!! cast to Ogre::String always
		if (value.size() == 0) { // show all values
			ImprovedConfigFile::SettingsIterator sit = mApp->getConfig().getSettingsIterator();
			while (sit.hasMoreElements()) {
				mConsole->addToConsole(sit.peekNextKey() + " = "+sit.peekNextValue());
				sit.moveNext();
			}
		}
		else {
			std::vector<String> values = StringUtil::split(value, " ", 1);
			if (values.size() == 1) { // only a setting name -> print the value of the setting
				String setting = mApp->getConfig().getSetting(values[0]);
				if (setting.size() == 0)
					mConsole->addToConsole(mConsole->getConsoleStringError() + "No such setting!");
				else
					mConsole->addToConsole(values[0] + ": " + setting);
			}
			else { // setting name + new value -> save new value
				mApp->getConfig().setSetting(values[0], values[1]);
				mConsole->addToConsole(mConsole->getConsoleStringSuccess()+" "+values[0] + ": "+values[1]);
			}
		}
	}
	else if (key == "config_save") {
		//TODO!!: save config - give filename
		mApp->getConfig().save();
	}
	else if (key == "config_load") { //TODO!!: Console - implement reloading config file, more commands for applying (only certain?) commands
		mApp->getConfig();
	}
	else if (key == "help") {
		mConsole->addToConsole("Commands:\nconfig [[<setting>] <value>]\n\
\t\tno arguments: show all settings\n\
\t\t\<setting> given: show value of given setting\n\
\t\t<setting> and <value> given: set new value.\n\
\t\tIf several values (e.g. Vector3): space-separated list.\n\
config_save [<filename>]\n\t\tdefault file: ../BV3D.cfg\n\
config_load [<filename>]\n\t\tdefault file: ../BV3D.cfg\n\
clear\n\t\tclear console");
	}
}