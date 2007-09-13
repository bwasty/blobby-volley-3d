#ifndef BV3D_REFEREE
#define BV3D_REFEREE

#include <vrs/sharedobj.h>

namespace BV3D
{
	class Game;
	class HUD;

	/*! \brief The Referee class is an abstract class for implementation of volleyball rules.

		It defines an interface for the usage of referee subclasses.
		It also already implements some getters/setters and elementary mehtods for score keeping and such.
		Referee-Classes are responsible for:
 			- score keeping
			- identification of faults 
 			- taking actions if faults occur(starting a new rally(results in new serve), ending the game,...)
	*/
	class Referee
	{
	public:
		Referee(VRS::SO<Game> game);
		virtual ~Referee(void){};
		
		virtual void ballOnBlobb(TEAM team) = 0;
		virtual void ballOnField(TEAM team) = 0;	

		VRS::SO<Game> getGame()		{ return mGame;}
		int getCurrentScore(TEAM team);
		void setCurrentScore(TEAM team, int score) {mScore[team] = score;}
		int getCurrentContacts(TEAM team);
		void setCurrentContacts(TEAM team, int contacts) {mContacts[team] = contacts;}
		int getWinningScore()		{ return mWinningScore;}
		int getMaximumContacts()	{ return mMaxContacts;}
		int getMinimumDifference()	{ return mMinDifference;}
		BV3D_ERROR setWinningScore(int winningScore);
		BV3D_ERROR setMaximumContacts(int maxContacts);
		BV3D_ERROR setMinimumDifference(int minDifference);
		void startNewGame();
		TEAM getServingTeam()		{ return mServingTeam;}
		void setServingTeam(TEAM team) {mServingTeam = team;}
		void setActive(bool b)		{mActive = b;}
		bool getActive()			{return mActive;}
		void setHUD(VRS::SO<HUD> hud);
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
		VRS::SO<Game> mGame;
		TEAM mServingTeam;
		bool mActive;

	};
}

#endif // #ifndef BV3D_REFEREE
