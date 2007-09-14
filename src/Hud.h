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

	/**
	 * \brief HUD represents the heads-up-display
	 * it is responsible for displaying the score, indicating the server and showing a win-message
	 */
	class HUD : public VRS::SharedObj
	{
	public:
		HUD(VRS::Color colorPlayer1 = VRS::Color::red, VRS::Color colorPlayer2 = VRS::Color::green);

		/**
		 * \dtor
		 */
		virtual ~HUD() {}

		/**
		 * \return the hud scene
		 */
		VRS::SO<VRS::SceneThing> getScene() {return mScene;}

		void setScore(int scoreP1, int scoreP2, TEAM servingTeam);
		void setColors(VRS::Color colorPlayer1, VRS::Color colorPlayer2);
		void showWinner(TEAM winner);

	private:
		int		mScoreP1Index;	// overlay-index of player 1 score
		int		mScoreP2Index;	// overlay-index of player 2 score
		int		mServingIndex;	// overlay-index of serving-indicator
		int		mWinnerIndex;	// overlay-index of win-message
		VRS::SO<VRS::SceneThing>	mScene;	// local hud scene
		VRS::Color	mP1Color;		// player 1 color
		VRS::Color	mP2Color;		// player 2 color
	};
}

#endif	// #ifndef BV3D_HUD