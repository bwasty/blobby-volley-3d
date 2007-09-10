/**
 * The Referee class is an abstract class for implementation of volleyball rules.
 * It defines an interface for the usage of referee subclasses.
 * It also already implements some getters/setters and elementary mehtods for score keeping and such.
 * Referee-Classes are responsible for:
 *	- score keeping
 *  - identification of faults 
 *	- taking actions if faults occur(starting a new rally(results in new serve), ending the game,...)
 *
 */
#include "Constants.h"
#include "Referee.h"
#include "Hud.h"

using namespace BV3D;

Referee::Referee(SO<Game> game)
{
	mGame = game;
	mScore[0] = mScore[1] = mContacts[0] = mContacts[1] = 0; //set start values
	//std::cout << mContacts[1] << std::endl;
	mWinningScore = mMaxContacts = mMinDifference = 1;		//set dummy values
	mServingTeam = BV3D::BV3D_TEAM1;
	mActive = true;
	mHud = 0;
}

SO<Game> Referee::getGame()
{
	return mGame;
}

/*
 *	Returns current score for individual teams
 */
int Referee::getCurrentScore(BV3D_TEAM team)
{
	return mScore[getTeamIndex(team)];
}

/*
 *	Returns current number of consecutive contacts with the ball for individual teams
 */
int Referee::getCurrentContacts(BV3D_TEAM team)
{
	return mContacts[getTeamIndex(team)];
}

/*
 *	Returns the score needed to win the game
 */
int Referee::getWinningScore()
{
	return mWinningScore;
}

/*
 *	Returns the maximum number of consecutive contatcts one team might have without causing a fault
 */
int Referee::getMaximumContacts()
{
	return mMaxContacts;
}

/*
 *	Returns the minumum difference between teams needed to win the game
 */
int Referee::getMinimumDifference()
{
	return mMinDifference;
}

/*
 *	Sets the score needed to win the game
 *	Must not be less than 1
 */
BV3D_ERROR Referee::setWinningScore(int winningScore)
{
	if (winningScore > 0)
	{
		mWinningScore = winningScore;
		return BV3D_NO_ERROR;
	}
	else
		return BV3D_INVALID_PARAMETER;
}

/*
 *	Sets the maximum number of consecutive contatcts one team might have without causing a fault
 *  Must not be less than 1
 */
BV3D_ERROR Referee::setMaximumContacts(int maxContacts)
{
	if (maxContacts > 0)
	{
		mMaxContacts = maxContacts;
		return BV3D_NO_ERROR;
	}
	else
		return BV3D_INVALID_PARAMETER;
}

/*
 *	Sets the minimum difference between teams needed to win the game
 *	Must not be less than 1
 */
BV3D_ERROR Referee::setMinimumDifference(int minDifference)
{
	if (minDifference > 0)
	{
		mMinDifference = minDifference;
		return BV3D_NO_ERROR;
	}
	else
		return BV3D_INVALID_PARAMETER;
}

/*
 * Resets all counters to 0 for a new game to start
 */
void Referee::startNewGame()
{
	mScore[0] = mScore[1] = mContacts[0] = mContacts[1] = 0;
	mServingTeam = BV3D_TEAM1;
	if(mHud)
		mHud->setScore(0,0,BV3D_TEAM1);
}

/*
 * Increases score of 'team' by 1
 */
int Referee::increaseScore(BV3D_TEAM team)
{
	mScore[getTeamIndex(team)]++;
	if(mHud)
		mHud->setScore(mScore[BV3D_TEAM1],mScore[BV3D_TEAM2],team);
	return mScore[getTeamIndex(team)];
}

/*
 * Increases contacts-counter of 'team' by 1
 */
int Referee::increaseContacts(BV3D_TEAM team)
{
	return mContacts[getTeamIndex(team)]++;
}

/*
 * Resets contacts-counter of 'team' to 0
 */
void Referee::resetContacts(BV3D_TEAM team)
{
	mContacts[getTeamIndex(team)] = 0;
}

/*
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

/*
 * Returns an index corresponding to a teams constant
 */
int Referee::getTeamIndex(BV3D_TEAM team)
{
	if(team == BV3D_TEAM1)
		return 0;
	else if(team == BV3D_TEAM2)
		return 1;
	else
	{
		printf("ERROR: Unknown team occured!");
		return -1;		//TODO: throw error
	}
}

/*
 * Returns the constant for opponent of 'team'
 */
BV3D_TEAM Referee::getOpponent(BV3D_TEAM team)
{
	if(team == BV3D_TEAM1)
		return BV3D_TEAM2;
	else if (team == BV3D_TEAM2)
		return BV3D_TEAM1;
	else
	{
		printf("ERROR: Unknown team occured!");
		return BV3D_TEAM1;		//TODO: throw error 
	}
}

void BV3D::Referee::gameOver(BV3D::BV3D_TEAM winner) {
	if(mHud)
		mHud->showWinner(winner);
}