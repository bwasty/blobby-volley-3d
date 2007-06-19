#include "ClassicReferee.h"

using namespace BV3D;

ClassicReferee::ClassicReferee(SO<Game> game) : Referee(game)
{
	setWinningScore(15);
	setMaximumContacts(3);
	setMinimumDifference(2);
	//game->newServe(true);
	m_isLeftSideServing = true;
}

ClassicReferee::~ClassicReferee(void)
{
}

void ClassicReferee::ballOnBlobb(bool isInLeftField)
{
	resetContacts(!isInLeftField);
	if (increaseContacts(isInLeftField) > getMaximumContacts())
	{
		if (m_isLeftSideServing != isInLeftField)	//if left side served and right side made fault + vice versa
			increaseScore(!isInLeftField);	//opponent of 'isInLeftField' scores
		if (isGameOver())					//only opponent could have scored since last test
		{	//m_game->gameOver(! isInLeftField);	//if game over then opponent must be winner
		}
		else
		{	//m_game->newServe(!isInLeftField);
			m_isLeftSideServing = !isInLeftField;
			resetContacts(isInLeftField);
		}
	}
}

void ClassicReferee::ballOnField(bool isInLeftField)
{
	if (m_isLeftSideServing != isInLeftField)	//if left side served and right side made fault + vice versa
	increaseScore(! isInLeftField);	//opponent of 'isInLeftField' scores
	if (isGameOver())					//only opponent could have scored since last test
	{	//m_game->gameOver(!isInLeftField);	//if game over then opponent must be winner
	}
	else
	{	//m_game->newServe(!isInLeftField);
		m_isLeftSideServing = !isInLeftField;
		resetContacts(true);		//reset current contact counters
		resetContacts(false);
	}
}

void ClassicReferee::startNewGame()
{
	Referee::startNewGame();
	//game->newServe(true);
	m_isLeftSideServing = true;
}