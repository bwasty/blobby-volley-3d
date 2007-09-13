#ifndef BV3D_MENU
#define BV3D_MENU

#include "Constants.h"
#include <vrs/sharedobj.h>
#include <vrs/color.h>

namespace VRS {
	template<class T> class SO;
	class SceneThing;
	class Font;
	class ShapeMaterialGL;
	class PointLight;
	class PickingCallback;
	class IntersectionInfo;
}

namespace BV3D
{
	class Game;
	class ModelOptimizer;

	class Menu : public VRS::SharedObj
	{
	public:
		enum PLACE {BEACH, HEAVEN, ARENA, CITY, MAX_PLACES};
		enum RULES {CLASSIC, TIE_BREAK, MAX_RULES};
		enum COLOR {RED, GREEN, BLUE, YELLOW, CORAL, CYAN, MAX_COLORS};
		enum CONTROLS {KB_ARROWS, KB_WASDQ, MOUSE, AI, MAX_CONTROLS};

	private:
		enum MENUSCREEN {MAIN,CREDITS,OPTIONS,GAME,P1,P2};
		enum FULLSCREEN {OFF,ON};

	public:
		Menu(VRS::SO<Game> game);
		virtual ~Menu() {}
		VRS::SO<VRS::SceneThing> getScene() {return mScene;}
		VRS::SO<VRS::PickingCallback> getPickingCallback() {return mPickingCallback;}
		void showMainMenu(bool paused);

	public:
		CONTROLS getControls(BV3D::TEAM team) {return (team) ? mP2Controls : mP1Controls;}
		VRS::Color getColor(BV3D::TEAM team);
		RULES getRules() {return mRules;}
		PLACE getPlace() {return mPlace;}

	protected:
		void select(VRS::IntersectionInfo* info);
		void addText(const char* label, const char* command = 0);
		void showMenu(MENUSCREEN screen);
		static const char** places() {static const char* placez[] = {"Beach", "Heaven","Arena", "City"}; return placez;}
		static const char** rules() {static const char* rulez[] = {"Classic", "Tie Break"}; return rulez;}
		static const char** fullscreen() {static const char* full[] = {"Off", "On"}; return full;}
		static const VRS::Color* colors() {
			static const VRS::Color colorz[] = {VRS::Color::red, VRS::Color::green, VRS::Color::blue,
				VRS::Color::yellow, VRS::Color::coral, VRS::Color::cyan}; return colorz;}
		static const char** controls() {
			static const char* controlz[] = {"Arrows+Space", "WASD+Q", "Mouse", "Computer"}; return controlz;}

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
		VRS::SO<ModelOptimizer>		mOptimizer;
		VRS::SO<VRS::SceneThing>	mScene;
		VRS::SO<VRS::SceneThing>	mCurrentMenuScene;
		VRS::SO<VRS::SceneThing>	mBallScene;
		VRS::SO<VRS::Font>			mFont;
		VRS::SO<VRS::ShapeMaterialGL>	mMenuTextMaterial;
		VRS::SO<VRS::ShapeMaterialGL>	mMenuHitBoxMaterial;
		VRS::SO<VRS::PointLight>	mLight;
		VRS::SO<VRS::SceneThing>	mBlobb;
		VRS::SO<VRS::ShapeMaterialGL>	mBlobbMaterial;
		VRS::SO<VRS::PickingCallback>	mPickingCallback;
	};
}

#endif	// BV3D_MENU