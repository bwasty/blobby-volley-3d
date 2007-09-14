#ifndef BV3D_REFEREE
#define BV3D_REFEREE

#include "Constants.h"
#include <vrs/sharedobj.h>

namespace BV3D
{
	class Game;
	class HUD;

	/** \brief The Referee class is an abstract class for implementation of volleyball rules.

		It defines an interface for the usage of referee subclasses.
		It also already implements some getters/setters and elementary mehtods for score keeping and such.
		Referee-Classes are responsible for:
 			- score keeping
			- identification of faults 
 			- taking actions if faults occur(starting a new rally(results in new serve), ending the game,...)
	*/
	class Referee : public VRS::SharedObj
	{
	public:
		Referee(VRS::SO<BV3D::Game> game);
		virtual ~Referee(void){};
		
		virtual void ballOnBlobb(BV3D::TEAM team) = 0;
		virtual void ballOnField(BV3D::TEAM team) = 0;	

		VRS::SO<BV3D::Game> getGame()			{ return mGame;}
		int getCurrentScore(BV3D::TEAM team);
		void setCurrentScore(BV3D::TEAM team, int score) {mScore[team] = score;}
		int getCurrentContacts(BV3D::TEAM team);
		void setCurrentContacts(BV3D::TEAM team, int contacts) {mContacts[team] = contacts;}
		/**
		*	Returns the score needed to win the game
		*/
		int getWinningScore()			{ return mWinningScore;}
		/**
		*	Returns the maximum number of consecutive contatcts one team might have without causing a fault
		*/
		int getMaximumContacts()		{ return mMaxContacts;}
		/**
		*	Returns the minumum difference between teams, needed to win the game
		*/
		int getMinimumDifference()		{ return mMinDifference;}
		BV3D::BV3D_ERROR setWinningScore(int winningScore);
		BV3D::BV3D_ERROR setMaximumContacts(int maxContacts);
		BV3D::BV3D_ERROR setMinimumDifference(int minDifference);
		void startNewGame();
		BV3D::TEAM getServingTeam()		{ return mServingTeam;}
		void setServingTeam(BV3D::TEAM team) {mServingTeam = team;}
		void setActive(bool b)		{mActive = b;}
		bool getActive()			{return mActive;}
		void setHUD(VRS::SO<BV3D::HUD> hud);
		void gameOver(BV3D::TEAM winner);
		bool isGameOver();

	protected:
		int increaseScore(BV3D::TEAM team);
		int increaseContacts(BV3D::TEAM team);
		void resetContacts(BV3D::TEAM team);
		int getTeamIndex(BV3D::TEAM team);
		BV3D::TEAM getOpponent(BV3D::TEAM team);
		void newServe(BV3D::TEAM servingTeam);
		
	private:
		int mScore[2], mContacts[2];
		int mWinningScore, mMaxContacts, mMinDifference;
		VRS::SO<BV3D::HUD>	mHud;

	protected:
		VRS::SO<BV3D::Game> mGame;
		BV3D::TEAM mServingTeam;
		bool mActive;

	};
}

#endif // #ifndef BV3D_REFEREE
