#include "Constants.h"
#include "Referee.h"
#include "Hud.h"
#include "Game.h"

using namespace BV3D;

Referee::Referee(SO<Game> game)
{
	mGame = game;
	mScore[0] = mScore[1] = mContacts[0] = mContacts[1] = 0; //set start values
	mWinningScore = mMaxContacts = mMinDifference = 1;		//set dummy values
	mServingTeam = BV3D::TEAM1;
	mActive = true;
	mHud = 0;
}

SO<Game> Referee::getGame()
{
	return mGame;
}

/**
 *	Returns current score for individual teams
 */
int Referee::getCurrentScore(TEAM team)
{
	return mScore[getTeamIndex(team)];
}

/**
 *	Returns current number of consecutive contacts with the ball for individual teams
 */
int Referee::getCurrentContacts(TEAM team)
{
	return mContacts[getTeamIndex(team)];
}

/**
 *	Returns the score needed to win the game
 */
int Referee::getWinningScore()
{
	return mWinningScore;
}

/**
 *	Returns the maximum number of consecutive contatcts one team might have without causing a fault
 */
int Referee::getMaximumContacts()
{
	return mMaxContacts;
}

/**
 *	Returns the minumum difference between teams needed to win the game
 */
int Referee::getMinimumDifference()
{
	return mMinDifference;
}

/**
 *	Sets the score needed to win the game
 *	Must not be less than 1
 */
BV3D_ERROR Referee::setWinningScore(int winningScore)
{
	if (winningScore > 0)
	{
		mWinningScore = winningScore;
		return NO_ERROR;
	}
	else
		return INVALID_PARAMETER;
}

/**
 *	Sets the maximum number of consecutive contatcts one team might have without causing a fault
 *  Must not be less than 1
 */
BV3D_ERROR Referee::setMaximumContacts(int maxContacts)
{
	if (maxContacts > 0)
	{
		mMaxContacts = maxContacts;
		return NO_ERROR;
	}
	else
		return INVALID_PARAMETER;
}

/**
 *	Sets the minimum difference between teams needed to win the game
 *	Must not be less than 1
 */
BV3D_ERROR Referee::setMinimumDifference(int minDifference)
{
	if (minDifference > 0)
	{
		mMinDifference = minDifference;
		return NO_ERROR;
	}
	else
		return INVALID_PARAMETER;
}

/**
 * Resets all counters to 0 for a new game to start
 */
void Referee::startNewGame()
{
	mScore[0] = mScore[1] = mContacts[0] = mContacts[1] = 0;
	mServingTeam = TEAM1;
	if(mHud)
		mHud->setScore(0,0,TEAM1);
}

/**
 * associates a HUD with the Referee and refreshes the HUD display
 * \param hud is the associated HUD
 */
void BV3D::Referee::setHUD(VRS::SO<HUD> hud)
{
	mHud = hud;
	mHud->setScore(mScore[0],mScore[1],mServingTeam);
}


/**
 * Increases score of 'team' by 1
 */
int Referee::increaseScore(TEAM team)
{
	mScore[getTeamIndex(team)]++;
	if(mHud)
		mHud->setScore(mScore[TEAM1],mScore[TEAM2],team);
	return mScore[getTeamIndex(team)];
}

/**
 * Increases contacts-counter of 'team' by 1
 */
int Referee::increaseContacts(TEAM team)
{
	return ++mContacts[getTeamIndex(team)];
}

/**
 * Resets contacts-counter of 'team' to 0
 */
void Referee::resetContacts(TEAM team)
{
	mContacts[getTeamIndex(team)] = 0;
}

/**
 * Returns whether one team has won the game
 */
bool Referee::isGameOver()
{
	int a = 0;
	int b = 1;
	if (mScore[a] == mScore[b])
		return false;
	
	else if (mScore[a] < mScore[b]) // make sure that 'a' is the one with highest score
	{
		int temp = a;
		a = b;
		b = temp;
	}
	return ( ((mScore[a] - mScore[b]) >= getMinimumDifference()) && (mScore[a] >= getWinningScore()) );
}

/**
 * Returns an index corresponding to a teams constant
 */
int Referee::getTeamIndex(TEAM team)
{
	if(team == TEAM1)
		return 0;
	else if(team == TEAM2)
		return 1;
	else
	{
		printf("ERROR: Unknown team occured!");
		return -1;		//TODO: throw error
	}
}

/**
 * Returns the constant for opponent of 'team'
 */
TEAM Referee::getOpponent(TEAM team)
{
	if(team == TEAM1)
		return TEAM2;
	else if (team == TEAM2)
		return TEAM1;
	else
	{
		printf("ERROR: Unknown team occured!");
		return TEAM1;		//TODO: throw error 
	}
}

void BV3D::Referee::gameOver(BV3D::TEAM winner)
{
	if(mHud)
		mHud->showWinner(winner);
}

void BV3D::Referee::newServe(BV3D::TEAM servingTeam)
{
	mServingTeam = servingTeam;
	mGame->scheduleNewServe();
	if(mHud)
		mHud->setScore(mScore[TEAM1],mScore[TEAM2],servingTeam);
}