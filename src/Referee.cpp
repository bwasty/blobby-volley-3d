#include "Constants.h"
#include "Referee.h"

using namespace BV3D;

Referee::Referee(SO<Game> game)
{
	m_game = game;
	m_score[0] = m_score[1] = m_contacts[0] = m_contacts[1] = 0; //set start values
	m_winningScore = m_maxContacts = m_minDifference = 1;		//set dummy values
}

SO<Game> Referee::getGame()
{
	return m_game;
}

int Referee::getCurrentScore(BV3D_TEAM team)
{
	return m_score[getTeamIndex(team)];
}

int Referee::getCurrentContacts(BV3D_TEAM team)
{
	return m_contacts[getTeamIndex(team)];
}

int Referee::getWinningScore()
{
	return m_winningScore;
}

int Referee::getMaximumContacts()
{
	return m_maxContacts;
}

int Referee::getMinimumDifference()
{
	return m_minDifference;
}

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

void Referee::startNewGame()
{
	m_score[0] = m_score[1] = m_contacts[0] = m_contacts[1] = 0;
}

int Referee::increaseScore(BV3D_TEAM team)
{
	return m_score[getTeamIndex(team)]++;
}

int Referee::increaseContacts(BV3D_TEAM team)
{
	return m_contacts[getTeamIndex(team)]++;
}

void Referee::resetContacts(BV3D_TEAM team)
{
	m_contacts[getTeamIndex(team)] = 0;
}

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