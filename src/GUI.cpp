#include "Constants.h"
#include "Application.h"
#include "Console.h"
#include "Ball.h"
#include "Blobb.h"
#include "Gui.h"

using namespace Ogre;

GUI::GUI(Application* app, RenderWindow* window) : mApp(app), mWindow(window), mDebugText("Press SPACE to enter GUI mode") {
	mDebugOverlay = OverlayManager::getSingleton().getByName("Core/DebugOverlay");
	mDebugOverlay->remove2D(mDebugOverlay->getChild("Core/LogoPanel"));

	showDebugOverlay(true);

	// setup MyGUI
	mMyGUI = new MyGUI::Gui();
	mMyGUI->initialise(window);
	mMyGUI->hidePointer();

	// MyGUI help tooltip 
	// TODO: extra function for gui creation?
	//TODO: HelpWidget: pin help tooltip on click?
	Ogre::UTFString _text("Controls for Blobby Volley 3D:\n\
						  SPACE         switch between GUI and play mode\n\
						  W,A,S,D,Q,E   Move camera (Alternative: Cursors)\n\
						  P             Toggle PhysX Debug Renderer\n\
						  B			  Toggle control of both blobbs\n\
						  1			  Reset ball\n\
						  F			  Toggle display of frame stats\n\
						  R			  Cycle Rendering Modes: normal, wireframe, point\n\
						  ^			  Toggle Console\n\
						  ESC			 Quit");
	MyGUI::EditPtr text = nullptr;
	MyGUI::WidgetPtr panel = mMyGUI->createWidget<MyGUI::Widget>("PanelSmall", mMyGUI->getViewWidth(), -128, 500, 200, MyGUI::Align::Default, "Statistic");
	text = panel->createWidget<MyGUI::Edit>("WordWrapSimple", 10, 10, 500, 240, MyGUI::Align::Default);
	//text->setTextColour(MyGUI::Colour(0, 1, 0, 1));
	MyGUI::StaticImagePtr image = panel->createWidget<MyGUI::StaticImage>(MyGUI::WidgetStyle::Popup, "StaticImage", MyGUI::IntCoord(mMyGUI->getViewWidth()-48, 0, 48, 48), MyGUI::Align::Default, "Back");
	image->setItemResource("pic_CoreMessageIcon");
	image->setItemGroup("Icons");
	image->setItemName("Quest");

	MyGUI::ControllerEdgeHide * controller = new MyGUI::ControllerEdgeHide(0.5);
	MyGUI::ControllerManager::getInstance().addItem(panel, controller);
	text->setCaption(_text);

	// create StaticText to show score
	mScoreDisplay = mMyGUI->createWidget<MyGUI::StaticText>("StaticText" , mMyGUI->getViewWidth()/2-20, 0, 200, 100, MyGUI::Align::Default, "Main");
	mScoreDisplay->setCaption("0 : 0!");

	// create console
	mConsole = new Console();
	//mConsole->setVisible(false);

	mConsole->registerConsoleDelegate("help", MyGUI::newDelegate(this, &GUI::consoleCommand));
	mConsole->registerConsoleDelegate("config", MyGUI::newDelegate(this, &GUI::consoleCommand));
	mConsole->registerConsoleDelegate("config_save", MyGUI::newDelegate(this, &GUI::consoleCommand));
	mConsole->registerConsoleDelegate("config_load", MyGUI::newDelegate(this, &GUI::consoleCommand));
	mConsole->registerConsoleDelegate("clear", MyGUI::newDelegate(this, &GUI::consoleCommand));
}

GUI::~GUI() {
	delete mConsole;
}


void GUI::consoleCommand(const Ogre::UTFString & key, const Ogre::UTFString & value) {
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
		mApp->getConfig().save(value); // if no value given, then it's saved to the originally loaded file
		mConsole->addToConsole(mConsole->getConsoleStringSuccess(), key, value);
	}
	else if (key == "config_load") {
		try {
			mApp->getConfig().load(value.length()==0 ? PATH_TO_CONFIG : value, "=\t:", true);
			mConsole->addToConsole(mConsole->getConsoleStringSuccess(), key, value);

			// TODO!!: make all settings dynamically changeable or document in good place which are not
			mApp->loadSettings();
			mApp->getBall()->loadSettings();
			mApp->getBlobb1()->loadSettings();
			mApp->getBlobb2()->loadSettings();
		}
		catch (FileNotFoundException e) {
			mConsole->addToConsole(mConsole->getConsoleStringError() + e.getDescription(), key, value);
		}
	}
	else if (key == "help") {
		mConsole->addToConsole("Commands:\nconfig [[<setting>] <value>]\n\
							   \t\tno arguments: show all settings\n\
							   \t\t<setting> given: show value of given setting\n\
							   \t\t<setting> and <value> given: set new value.\n\
							   \t\tIf several values (e.g. Vector3): space-separated list.\n\
							   config_save [<filename>]\n\t\tdefault file: "+PATH_TO_CONFIG+"\n\
							   config_load [<filename>]\n\t\tdefault file: "+PATH_TO_CONFIG+"\n\
							   clear\n\t\tclear console");
	}
}

void GUI::showDebugOverlay(bool show)
{
	if (mDebugOverlay)
	{
		if (show)
			mDebugOverlay->show();
		else
			mDebugOverlay->hide();
	}
}


void GUI::updateStats(void)
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

//void GUI::injectFrameEntered(Real timeSinceLastFrame) {
//	mMyGUI->injectFrameEntered(timeSinceLastFrame);
//}

void GUI::updateScoreDisplay(int scoreTeam1, int scoreTeam2, TEAM nowServing) {
	String score;
	if (nowServing == TEAM2)
		score.append("!");
	score.append(StringConverter::toString(scoreTeam2)).append(":").append(StringConverter::toString(scoreTeam1));
	if (nowServing == TEAM1)
		score.append("!");

	mScoreDisplay->setCaption(score);
}