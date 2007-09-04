#ifndef BV3D_MENU
#define BV3D_MENU

#include <vrs/sharedobj.h>

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
	private:
		enum MENUSCREEN {MAIN,OPTIONS,P1,P2,ENV};
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

	private:	// settings
		bool	mGameIsPaused;

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
	};
}

#endif	// BV3D_MENU