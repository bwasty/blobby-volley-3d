#ifndef BV3D_HUD
#define BV3D_HUD

#include <vrs/sharedobj.h>
#include <vrs/color.h>

namespace VRS {
	template<class T> class SO;
	class SceneThing;
}

namespace BV3D
{
	enum TEAM;

	class HUD : public VRS::SharedObj
	{
	public:
		HUD(VRS::Color colorPlayer1 = VRS::Color::red, VRS::Color colorPlayer2 = VRS::Color::green);
		virtual ~HUD() {}
		VRS::SO<VRS::SceneThing> getScene() {return mScene;}
		void setScore(int scoreP1, int scoreP2, TEAM servingTeam);
		void setColors(VRS::Color colorPlayer1, VRS::Color colorPlayer2);
		void showWinner(TEAM winner);
	private:
		int		mScoreP1Index;
		int		mScoreP2Index;
		int		mServingIndex;
		int		mWinnerIndex;
		VRS::SO<VRS::SceneThing>	mScene;
		VRS::Color	mP1Color;
		VRS::Color	mP2Color;
	};
}

#endif	// #ifndef BV3D_HUD