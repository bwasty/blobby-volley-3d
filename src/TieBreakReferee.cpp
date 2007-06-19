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

void TieBreakReferee::ballOnBlobb(bool isInLeftField)
{
	resetContacts(!isInLeftField);
	if (increaseContacts(isInLeftField) > getMaximumContacts())
	{
		increaseScore(! isInLeftField);	//opponent of 'inLeftField' scores
		if (isGameOver())					//only opponent could have scored since last test
		{	//m_game->gameOver(! isInLeftField);	//if game over then opponent must be winner
		}
		else
		{	//m_game->newServe(! isInLeftField);
			resetContacts(isInLeftField);
		}
	}
}

void TieBreakReferee::ballOnField(bool isInLeftField)
{
	increaseScore(! isInLeftField);	//opponent of 'isInLeftField' scores
	if (isGameOver())					//only opponent could have scored since last test
	{	//m_game->gameOver(! isInLeftField);	//if game over then opponent must be winner
	}
	else
	{	//m_game->newServe(! isInLeftField);
		resetContacts(true);		//reset current contact counters
		resetContacts(false);
	}
}

void TieBreakReferee::startNewGame()
{
	Referee::startNewGame();
	//game->newServe(true);
}