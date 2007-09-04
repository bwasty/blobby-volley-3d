#ifndef BV3D_HUD
#define BV3D_HUD

#include <vrs/sharedobj.h>

namespace VRS {
	template<class T> class SO;
	class SceneThing;
}

namespace BV3D
{
	enum BV3D_TEAM;

	class HUD : public VRS::SharedObj
	{
	public:
		HUD();
		virtual ~HUD() {}
		VRS::SO<VRS::SceneThing> getScene() {return mScene;}
		void setScore(int scoreP1, int scoreP2, BV3D_TEAM servingTeam);
		void showWinner(BV3D_TEAM winner);
	private:
		int		mScoreP1Index;
		int		mScoreP2Index;
		int		mServingIndex;
		int		mWinnerIndex;
		VRS::SO<VRS::SceneThing>	mScene;
	};
}

#endif	// #ifndef BV3D_HUD