#ifndef BV3D_MENU
#define BV3D_MENU

#include <vrs/sharedobj.h>
#include <vrs/color.h>

namespace VRS {
	template<class T> class SO;
	class SceneThing;
	class Selector;
	class RayRequest;
	class Font;
	class ShapeMaterialGL;
	class PointLight;
}

namespace BV3D
{
	class Game;

	class Menu : public VRS::SharedObj
	{
	public:
		enum PLACE {BEACH, BEACH_AGAIN, MAX_PLACES};
		enum RULES {CLASSIC, TIE_BREAK, MAX_RULES};
		enum COLOR {RED, GREEN, BLUE, YELLOW, CORAL, CYAN, MAX_COLORS};
		enum CONTROLS {KB_ARROWS, KB_WASD, MOUSE, AI, MAX_CONTROLS};
	private:
		enum MENUSCREEN {MAIN,CREDITS,OPTIONS,GAME,P1,P2};
		enum FULLSCREEN {OFF,ON};
	public:
		Menu(VRS::SO<Game> game);
		virtual ~Menu() {}
		VRS::SO<VRS::SceneThing> getScene() {return mScene;}
		VRS::SO<VRS::Selector> getSelector() {return mSelector;}
		void showMainMenu(bool paused);

	protected:
		void select();
		void addText(const char* label, const char* command = 0);
		void showMenu(MENUSCREEN screen);
		static const char** places() {static const char* placez[] = {"Beach", "Beach Again"}; return placez;}
		static const char** rules() {static const char* rulez[] = {"Classic", "Tie Break"}; return rulez;}
		static const char** fullscreen() {static const char* full[] = {"Off", "On"}; return full;}
		static const VRS::Color* colors() {
			static const VRS::Color colorz[] = {VRS::Color::red, VRS::Color::green, VRS::Color::blue,
				VRS::Color::yellow, VRS::Color::coral, VRS::Color::cyan}; return colorz;}
		static const char** controls() {
			static const char* controlz[] = {"Arrows + Space", "WASD+Shift", "Mouse", "Ai"}; return controlz;}

	private:	// settings
		MENUSCREEN	mCurrentScreen;
		bool		mGameIsPaused;
		PLACE		mPlace;
		RULES		mRules;
		FULLSCREEN	mFullscreen;
		COLOR		mP1Color, mP2Color;
		CONTROLS	mP1Controls, mP2Controls;

	private:
		VRS::SO<Game>	mGame;
		VRS::SO<VRS::SceneThing>	mScene;
		VRS::SO<VRS::SceneThing>	mCurrentMenuScene;
		VRS::SO<VRS::SceneThing>	mBallScene;
		VRS::SO<VRS::Selector>		mSelector;
		VRS::SO<VRS::RayRequest>	mRayRequest;
		VRS::SO<VRS::Font>			mFont;
		VRS::SO<VRS::ShapeMaterialGL>	mMenuTextMaterial;
		VRS::SO<VRS::ShapeMaterialGL>	mMenuHitBoxMaterial;
		VRS::SO<VRS::PointLight>	mLight;
		VRS::SO<VRS::SceneThing>	mBlobb;
		VRS::SO<VRS::ShapeMaterialGL>	mBlobbMaterial;
	};
}

#endif	// BV3D_MENU