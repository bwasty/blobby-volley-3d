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

	/**
	 * \brief Menu implements the menu for Blobby Volley 3D
	 */
	class Menu : public VRS::SharedObj
	{
	public:
		/**
		 * listing of possible environments
		 */
		enum PLACE {BEACH, HEAVEN, ARENA, CITY, MAX_PLACES};

		/**
		 * listing of possible scoring rules
		 */
		enum RULES {CLASSIC, TIE_BREAK, MAX_RULES};

		/**
		 * listing of possible Blobb colors
		 */
		enum COLOR {RED, GREEN, BLUE, YELLOW, CORAL, CYAN, MAX_COLORS};

		/**
		 * listing of possible Blobb control types
		 */
		enum CONTROLS {KB_ARROWS, KB_WASDQ, MOUSE, AI, MAX_CONTROLS};

	private:
		/**
		 * listing of menu screens
		 */
		enum MENUSCREEN {MAIN,CREDITS,OPTIONS,GAME,P1,P2};

		/**
		 * listing of possible fullscreen states
		 */
		enum FULLSCREEN {OFF,ON};

	public:
		Menu(VRS::SO<Game> game);

		/**
		 * \dtor
		 */
		virtual ~Menu() {}

		/**
		 * returns the menu scene
		 */
		VRS::SO<VRS::SceneThing> getScene() {return mScene;}

		/**
		 * returns a node for selecting menu items
		 * the node needs to be installed in the canvas by the Game
		 */
		VRS::SO<VRS::PickingCallback> getPickingCallback() {return mPickingCallback;}

		void showMainMenu(bool paused);

	public:
		/**
		 * \param team indicates the Blobb
		 * \return a symbol indicating the controls type for the corresponding Blobb
		 */
		CONTROLS getControls(BV3D::TEAM team) {return (team) ? mP2Controls : mP1Controls;}

		VRS::Color getColor(BV3D::TEAM team);

		/**
		 * \return a symbol indicating the scoring rules type
		 */
		RULES getRules() {return mRules;}

		/**
		 * \return a symbol indicating the environment (game background)
		 */
		PLACE getPlace() {return mPlace;}

	protected:
		void select(VRS::IntersectionInfo* info);
		void addText(const char* label, const char* command = 0);
		void showMenu(MENUSCREEN screen);

		static const char** places() {static const char* placez[] = {"Beach", "Heaven","Arena", "City"}; return placez;}
		static const char** rules() {static const char* rulez[] = {"Classic", "Tie Break"}; return rulez;}
		static const char** fullscreen() {static const char* full[] = {"Off", "On"}; return full;}
		static const char** controls() {
			static const char* controlz[] = {"Arrows+Space", "WASD+Q", "Mouse", "Computer"}; return controlz;}
		/**<
		 * places(), rules(), fullscreen(), colors() and controls() return references to static arrays of strings
		 * holding the text for the menu items for the correspong options
		 */

		/**
		 * static array of VRS color constants for Blobb color
		 */
		static const VRS::Color* colors() {
			static const VRS::Color colorz[] = {VRS::Color::red, VRS::Color::green, VRS::Color::blue,
				VRS::Color::yellow, VRS::Color::coral, VRS::Color::cyan}; return colorz;}

	private:
		bool		mGameIsPaused;		// indicates whether the game is over or just paused
		PLACE		mPlace;				// holds the currently selected environment
		RULES		mRules;				// holds the currently selected scoring rules type
		FULLSCREEN	mFullscreen;		// indicates whether the game is run in fullscreen or not
		COLOR		mP1Color, mP2Color;	// hold the currently selected colors for the Blobbs
		CONTROLS	mP1Controls, mP2Controls;	// hold the currently selected controls for the Blobbs

	private:
		VRS::SO<Game>	mGame;							// the associated Game
		VRS::SO<ModelOptimizer>		mOptimizer;			// the optimizer for external 3D models
		VRS::SO<VRS::SceneThing>	mScene;				// the local menu scene
		VRS::SO<VRS::SceneThing>	mCurrentMenuScene;	// a sub-scene holding just the menu items
		VRS::SO<VRS::SceneThing>	mBallScene;			// a sub-scene holding just the ball in the background
		VRS::SO<VRS::Font>			mFont;				// the Font used for the menu items
		VRS::SO<VRS::ShapeMaterialGL>	mMenuTextMaterial;	// material settings for the menu items
		VRS::SO<VRS::ShapeMaterialGL>	mMenuHitBoxMaterial;	// material settings for the menu items hitboxes
		VRS::SO<VRS::PointLight>	mLight;				// light to shine on the menu scene
		VRS::SO<VRS::SceneThing>	mBlobb;				// sub-scene for the Blobb shown with the color menu item
		VRS::SO<VRS::ShapeMaterialGL>	mBlobbMaterial;	// material settings for the Blobb
		VRS::SO<VRS::PickingCallback>	mPickingCallback;	// used to capture menu item activation (mouse clicks)
	};
}

#endif	// BV3D_MENU