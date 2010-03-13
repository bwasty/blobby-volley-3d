//#include <OgrePrerequisites.h>
#include <MyGUI_Widget.h>
#include "Constants.h"

namespace MyGUI {
	class Gui;
}

class Console;
class Application;


class GUI {
public:
	GUI(Application* app, Ogre::RenderWindow* window);
	~GUI();

	inline MyGUI::Gui* getMyGui() { return mMyGUI; }
	inline Console* getConsole() { return mConsole; }
	inline void setDebugText(Ogre::String text) {mDebugText = text; }
	void consoleCommand(const Ogre::String & _key, const Ogre::String & _value);
	void showDebugOverlay(bool show);
	void updateStats();

	void updateScoreDisplay(int scoreTeam1, int scoreTeam2, TEAM nowServing);

	std::string mDebugText;

private:
	Application* mApp;
	Ogre::RenderWindow* mWindow;
	MyGUI::Gui* mMyGUI;

	Ogre::Overlay* mDebugOverlay;
	

	Console* mConsole;

	MyGUI::WidgetPtr mScoreDisplay;
};