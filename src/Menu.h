#ifndef BV3D_MENU
#define BV3D_MENU

#include <vrs/sharedobj.h>

namespace VRS {
	template<class T> class SO;
	class SceneThing;
	class Selector;
	class RayRequest;
}

namespace BV3D
{
	class Game;

	class Menu : public VRS::SharedObj
	{
	public:
		Menu(VRS::SO<Game> game);
		virtual ~Menu() {}
		VRS::SO<VRS::SceneThing> getScene() {return mScene;}
		VRS::SO<VRS::Selector> getSelector() {return mSelector;}
		bool isActive() {return mIsActive;}
		void showMainMenu();

	protected:
		void select();

	private:
		bool						mIsActive;
		VRS::SO<VRS::SceneThing>	mScene;
		VRS::SO<VRS::SceneThing>	mCurrentMenuScene;
		VRS::SO<VRS::Selector>		mSelector;
		VRS::SO<VRS::RayRequest>	mRayRequest;

	private:
		VRS::SO<Game>	mGame;
	};
}

#endif	// BV3D_MENU