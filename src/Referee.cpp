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
	m_game = game;
	m_score[0] = m_score[1] = m_contacts[0] = m_contacts[1] = 0; //set start values
	//std::cout << m_contacts[1] << std::endl;
	m_winningScore = m_maxContacts = m_minDifference = 1;		//set dummy values
	m_ServingTeam = BV3D::BV3D_TEAM1;
	m_Active = true;
	mHud = 0;
}

SO<Game> Referee::getGame()
{
	return m_game;
}

/*
 *	Returns current score for individual teams
 */
int Referee::getCurrentScore(BV3D_TEAM team)
{
	return m_score[getTeamIndex(team)];
}

/*
 *	Returns current number of consecutive contacts with the ball for individual teams
 */
int Referee::getCurrentContacts(BV3D_TEAM team)
{
	return m_contacts[getTeamIndex(team)];
}

/*
 *	Returns the score needed to win the game
 */
int Referee::getWinningScore()
{
	return m_winningScore;
}

/*
 *	Returns the maximum number of consecutive contatcts one team might have without causing a fault
 */
int Referee::getMaximumContacts()
{
	return m_maxContacts;
}

/*
 *	Returns the minumum difference between teams needed to win the game
 */
int Referee::getMinimumDifference()
{
	return m_minDifference;
}

/*
 *	Sets the score needed to win the game
 *	Must not be less than 1
 */
BV3D_ERROR Referee::setWinningScore(int winningScore)
{
	if (winningScore > 0)
	{
		m_winningScore = winningScore;
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
		m_maxContacts = maxContacts;
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
		m_minDifference = minDifference;
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
	m_score[0] = m_score[1] = m_contacts[0] = m_contacts[1] = 0;
	m_ServingTeam = BV3D_TEAM1;
	if(mHud)
		mHud->setScore(0,0,BV3D_TEAM1);
}

/*
 * Increases score of 'team' by 1
 */
int Referee::increaseScore(BV3D_TEAM team)
{
	m_score[getTeamIndex(team)]++;
	if(mHud)
		mHud->setScore(m_score[BV3D_TEAM1],m_score[BV3D_TEAM2],team);
	return m_score[getTeamIndex(team)];
}

/*
 * Increases contacts-counter of 'team' by 1
 */
int Referee::increaseContacts(BV3D_TEAM team)
{
	return m_contacts[getTeamIndex(team)]++;
}

/*
 * Resets contacts-counter of 'team' to 0
 */
void Referee::resetContacts(BV3D_TEAM team)
{
	m_contacts[getTeamIndex(team)] = 0;
}

/*
 * Returns whether one team has won the game
 */
bool Referee::isGameOver()
{
	int a = 0;
	int b = 1;
	if (m_score[a] == m_score[b])
		return false;
	
	else if (m_score[a] < m_score[b]) // make sure that 'a' is the one with highest score
	{
		int temp = a;
		a = b;
		b = temp;
	}
	return ( ((m_score[a] - m_score[b]) >= getMinimumDifference()) && (m_score[a] >= getWinningScore()) );
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