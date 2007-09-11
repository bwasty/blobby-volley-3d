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
		
		virtual void ballOnBlobb(TEAM team) = 0;
		virtual void ballOnField(TEAM team) = 0;	

		SO<Game> getGame();
		int getCurrentScore(TEAM team);
		void setCurrentScore(TEAM team, int score) {mScore[team] = score;}
		int getCurrentContacts(TEAM team);
		void setCurrentContacts(TEAM team, int contacts) {mContacts[team] = contacts;}
		int getWinningScore();
		int getMaximumContacts();
		int getMinimumDifference();
		BV3D_ERROR setWinningScore(int winningScore);
		BV3D_ERROR setMaximumContacts(int maxContacts);
		BV3D_ERROR setMinimumDifference(int minDifference);
		void startNewGame();
		TEAM getServingTeam() {return mServingTeam;}
		void setServingTeam(TEAM team) {mServingTeam = team;}
		void setActive(bool b) {mActive = b;}
		bool getActive() {return mActive;}
		void setHUD(VRS::SO<HUD> hud) {mHud = hud;}
		void gameOver(TEAM winner);
		bool isGameOver();

	protected:
		int increaseScore(TEAM team);
		int increaseContacts(TEAM team);
		void resetContacts(TEAM team);
		int getTeamIndex(TEAM team);
		TEAM getOpponent(TEAM team);
		void newServe(TEAM servingTeam);

	private:
		int mScore[2], mContacts[2];
		int mWinningScore, mMaxContacts, mMinDifference;
		VRS::SO<HUD>	mHud;
		
	protected:
		SO<Game> mGame;
		TEAM mServingTeam;
		bool mActive;

	};
}

#endif // #ifndef BV3D_REFEREE
