#include <OgrePrerequisites.h>


namespace MyGUI {
	class Gui;
}

class Console;
class BaseApplication;


class GUI {
public:
	GUI(Application* app, Ogre::RenderWindow* window);
	~GUI();

	inline MyGUI::Gui* getMyGui() { return mMyGUI; }
	inline Console* getConsole() { return mConsole; }
	inline void setDebugText(Ogre::String text) {mDebugText = text; }
	void consoleCommand(const Ogre::UTFString & _key, const Ogre::UTFString & _value);
	void showDebugOverlay(bool show);
	void updateStats();

	//void injectFrameEntered(Ogre::Real timeSinceLastFrame);
private:
	Application* mApp;
	Ogre::RenderWindow* mWindow;
	MyGUI::Gui* mMyGUI;

	Ogre::Overlay* mDebugOverlay;
	std::string mDebugText;

	Console* mConsole;
};