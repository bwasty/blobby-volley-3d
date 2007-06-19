#include "Referee.h"

using namespace BV3D;

Referee::Referee(SO<Game> game)
{
	m_game = game;
	m_score[0] = m_score[1] = m_contacts[0] = m_contacts[1] = 0;
	m_winningScore = m_maxContacts = m_minDifference = 1;
}

SO<Game> Referee::getGame()
{
	return m_game;
}

int Referee::getCurrentScore(bool ofLeftField)
{
	if (ofLeftField)
		return m_score[0];
	else
		return m_score[1];
}

int Referee::getCurrentContacts(bool ofLeftField)
{
	if (ofLeftField)
		return m_contacts[0];
	else
		return m_contacts[1];
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

int Referee::setWinningScore(int winningScore)
{
	if (winningScore > 0)
		return m_winningScore = winningScore;
}

int Referee::setMaximumContacts(int maxContacts)
{
	if (maxContacts > 0)
		return m_maxContacts = maxContacts;
}

int Referee::setMinimumDifference(int minDifference)
{
	if (minDifference > 0)
		return m_minDifference = minDifference;
}

void Referee::startNewGame()
{
	m_score[0] = m_score[1] = m_contacts[0] = m_contacts[1] = 0;
}

int Referee::increaseScore(bool ofLeftField)
{
	if (ofLeftField)
		return m_score[0]++;
	else
		return m_score[1]++;
}

int Referee::increaseContacts(bool ofLeftField)
{
	if (ofLeftField)
		return m_contacts[0]++;
	else
		return m_contacts[1]++;
}

int Referee::resetContacts(bool ofLeftField)
{
	if (ofLeftField)
		return m_contacts[0] = 0;
	else
		return m_contacts[1] = 0;
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
	return ( ((m_score[a] - m_score[b]) >= m_minDifference) && (m_score[a] >= m_winningScore) );
}