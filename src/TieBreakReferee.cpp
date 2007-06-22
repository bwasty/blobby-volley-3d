#include "Constants.h"
#include "TieBreakReferee.h"

using namespace BV3D;

TieBreakReferee::TieBreakReferee(SO<Game> game) : Referee(game)
{
	setWinningScore(25);
	setMaximumContacts(3);
	setMinimumDifference(2);
	//game->newServe(true);		//left team has first serve
}

TieBreakReferee::~TieBreakReferee(void)
{
}

void TieBreakReferee::ballOnBlobb(BV3D_TEAM team)
{
	BV3D_TEAM opponent = getOpponent(team);
	resetContacts(opponent);
	if (increaseContacts(team) > getMaximumContacts())
	{
		increaseScore(opponent);
		if (isGameOver())					//only opponent could have scored since last test
		{	//m_game->gameOver(opponent);	//if game over then opponent must be winner
		}
		else
		{	//m_game->newServe(opponent);
			resetContacts(team);
		}
	}
}

void TieBreakReferee::ballOnField(BV3D_TEAM team)
{
	BV3D_TEAM opponent = getOpponent(team);
	increaseScore(opponent);	//opponent of 'isInLeftField' scores
	if (isGameOver())					//only opponent could have scored since last test
	{	//m_game->gameOver(opponent);	//if game over then opponent must be winner
	}
	else
	{	//m_game->newServe(opponent);
		resetContacts(team);		//reset current contact counters
		resetContacts(opponent);
	}
}

void TieBreakReferee::startNewGame()
{
	Referee::startNewGame();
	//game->newServe(BV3D_TEAM1);
}