#ifndef BV3D_REFEREE
#define BV3D_REFEREE

#include <vrs/sharedobj.h>
//#include "Constants.h"
/*#include <vrs/so.h>
#include <vrs/id.h>
#include <cassert>
#include <vrs/io/document.h>
//class SharedObj;
*/
using namespace VRS;

namespace BV3D
{
	class Game;
	class HUD;

	class Referee
	{
	public:
		Referee(SO<Game> game);
		virtual ~Referee(void){};
		
		virtual void ballOnBlobb(BV3D_TEAM team) = 0;
		virtual void ballOnField(BV3D_TEAM team) = 0;	

		SO<Game> getGame();
		int getCurrentScore(BV3D_TEAM team);
		int getCurrentContacts(BV3D_TEAM team);
		int getWinningScore();
		int getMaximumContacts();
		int getMinimumDifference();
		BV3D_ERROR setWinningScore(int winningScore);
		BV3D_ERROR setMaximumContacts(int maxContacts);
		BV3D_ERROR setMinimumDifference(int minDifference);
		void startNewGame();
		BV3D_TEAM getServingTeam() {return mServingTeam;}
		void setActive(bool b) {mActive = b;}
		bool getActive() {return mActive;}
		void setHUD(VRS::SO<HUD> hud) {mHud = hud;}
		void gameOver(BV3D_TEAM winner);
		bool isGameOver();

	protected:
		int increaseScore(BV3D_TEAM team);
		int increaseContacts(BV3D_TEAM team);
		void resetContacts(BV3D_TEAM team);
		int getTeamIndex(BV3D_TEAM team);
		BV3D_TEAM getOpponent(BV3D_TEAM team);

	private:
		int mScore[2], mContacts[2];
		int mWinningScore, mMaxContacts, mMinDifference;
		VRS::SO<HUD>	mHud;
		
	protected:
		SO<Game> mGame;
		BV3D_TEAM mServingTeam;
		bool mActive;

	};
}

#endif // #ifndef BV3D_REFEREE
