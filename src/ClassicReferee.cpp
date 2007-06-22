#include "Constants.h"
#include "ClassicReferee.h"

using namespace BV3D;

ClassicReferee::ClassicReferee(SO<Game> game) : Referee(game)
{
	setWinningScore(15);
	setMaximumContacts(3);
	setMinimumDifference(2);
	//game->newServe(true);
	m_servingTeam = BV3D_TEAM1;
}

ClassicReferee::~ClassicReferee(void)
{
}

void ClassicReferee::ballOnBlobb(BV3D_TEAM team)
{
	BV3D_TEAM opponent = getOpponent(team);
	resetContacts(opponent);
	if (increaseContacts(team) > getMaximumContacts())
	{
		if (m_servingTeam != team)
		{
			increaseScore(m_servingTeam);
			if (isGameOver())					//only opponent could have scored since last test
			{	//m_game->gameOver(m_servingTeam);	//if game over then opponent must be winner
			}
			else
			{	//m_game->newServe(m_servingTeam);
			}
		}
		else
		{	//m_game->newServe(opponent)	
		}
		resetContacts(team);
	}
}

void ClassicReferee::ballOnField(BV3D_TEAM team)
{
	if (m_servingTeam != team)
	{
		increaseScore(m_servingTeam);
		if (isGameOver())					//only servingTeam could have scored since last test
		{	//m_game->gameOver(m_servingTeam);	//if game over then servingTeam must be winner
		}
		else
		{	//m_game->newServe(m_servingTeam);
		}
	}
	else
	{	//m_game->newServe(getOpponent(team));	
	}
	resetContacts(BV3D_TEAM1);
	resetContacts(BV3D_TEAM2);
}

void ClassicReferee::startNewGame()
{
	Referee::startNewGame();
	//game->newServe(true);
	m_servingTeam = BV3D_TEAM1;
}