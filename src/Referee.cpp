#include "Referee.h"
#include "Hud.h"
#include "Game.h"

BV3D::Referee::Referee(VRS::SO<BV3D::Game> game)
{
	mGame = game;
	mScore[0] = mScore[1] = mContacts[0] = mContacts[1] = 0; //set start values
	mWinningScore = mMaxContacts = mMinDifference = 1;		//set dummy values
	mServingTeam = BV3D::TEAM1;
	mActive = true;
	mHud = 0;
}

/**
 *	Returns current score for individual teams
 */
int BV3D::Referee::getCurrentScore(BV3D::TEAM team)
{
	return mScore[getTeamIndex(team)];
}

/**
 *	Returns current number of consecutive contacts with the ball for individual teams
 */
int BV3D::Referee::getCurrentContacts(BV3D::TEAM team)
{
	return mContacts[getTeamIndex(team)];
}

/**
 *	Sets the score needed to win the game
 *	Must not be less than 1
 */
BV3D::BV3D_ERROR BV3D::Referee::setWinningScore(int winningScore)
{
	if (winningScore > 0)
	{
		mWinningScore = winningScore;
		return BV3D::NO_ERROR;
	}
	else
		return BV3D::INVALID_PARAMETER;
}

/**
 *	Sets the maximum number of consecutive contatcts one team might have without causing a fault
 *  Must not be less than 1
 */
BV3D::BV3D_ERROR BV3D::Referee::setMaximumContacts(int maxContacts)
{
	if (maxContacts > 0)
	{
		mMaxContacts = maxContacts;
		return BV3D::NO_ERROR;
	}
	else
		return BV3D::INVALID_PARAMETER;
}

/**
 *	Sets the minimum difference between teams needed to win the game
 *	Must not be less than 1
 */
BV3D::BV3D_ERROR BV3D::Referee::setMinimumDifference(int minDifference)
{
	if (minDifference > 0)
	{
		mMinDifference = minDifference;
		return BV3D::NO_ERROR;
	}
	else
		return BV3D::INVALID_PARAMETER;
}

/**
 * Resets all counters to 0 for a new game to start
 */
void BV3D::Referee::startNewGame()
{
	mScore[0] = mScore[1] = mContacts[0] = mContacts[1] = 0;
	mServingTeam = BV3D::TEAM1;
	if(mHud)
		mHud->setScore(0,0,BV3D::TEAM1);
}

/**
 * associates a HUD with the Referee and refreshes the HUD display
 * \param hud is the associated HUD
 */
void BV3D::Referee::setHUD(VRS::SO<BV3D::HUD> hud)
{
	mHud = hud;
	mHud->setScore(mScore[0],mScore[1],mServingTeam);
}


/**
 * Increases score of 'team' by 1
 */
int BV3D::Referee::increaseScore(BV3D::TEAM team)
{
	mScore[getTeamIndex(team)]++;
	if(mHud)
		mHud->setScore(mScore[BV3D::TEAM1],mScore[BV3D::TEAM2],team);
	return mScore[getTeamIndex(team)];
}

/**
 * Increases contacts-counter of 'team' by 1
 */
int BV3D::Referee::increaseContacts(BV3D::TEAM team)
{
	return ++mContacts[getTeamIndex(team)];
}

/**
 * Resets contacts-counter of 'team' to 0
 */
void BV3D::Referee::resetContacts(BV3D::TEAM team)
{
	mContacts[getTeamIndex(team)] = 0;
}

/**
 * Returns whether one team has won the game
 */
bool BV3D::Referee::isGameOver()
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
int BV3D::Referee::getTeamIndex(BV3D::TEAM team)
{
	if(team == BV3D::TEAM1)
		return 0;
	else if(team == BV3D::TEAM2)
		return 1;
	else
	{
		printf("ERROR: Unknown team occured!");
		return -1;
	}
}

/**
 * Returns the constant for opponent of 'team'
 */
BV3D::TEAM BV3D::Referee::getOpponent(BV3D::TEAM team)
{
	if(team == BV3D::TEAM1)
		return BV3D::TEAM2;
	else if (team == BV3D::TEAM2)
		return BV3D::TEAM1;
	else
	{
		printf("ERROR: Unknown team occured!");
		return BV3D::TEAM1;
	}
}

/**
 * Triggers game over routine.
 */
void BV3D::Referee::gameOver(BV3D::TEAM winner)
{
	if(mHud)
		mHud->showWinner(winner);
}

/**
 * Starts a new rally with 'team' having the serve.
 */
void BV3D::Referee::newServe(BV3D::TEAM servingTeam)
{
	mServingTeam = servingTeam;
	mGame->scheduleNewServe();
	if(mHud)
		mHud->setScore(mScore[BV3D::TEAM1],mScore[BV3D::TEAM2],servingTeam);
}