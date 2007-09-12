#include "Constants.h"
#include "TieBreakReferee.h"
#include "Game.h"

using namespace BV3D;

/**
 *	Sets up a Referee for new tournament volleyball rules, with a winning score of 25, a minimum difference between teams of 2 and a maximum of 3 contacts with the ball.
 */
TieBreakReferee::TieBreakReferee(SO<Game> game) : Referee(game)
{
	setWinningScore(25);
	setMaximumContacts(3);
	setMinimumDifference(2);
}

TieBreakReferee::~TieBreakReferee(void)
{
}

/**
 *	Handles the case when the ball collides with a blobb from a team.
 *  Increases the teams contact-counter and checks whether a fault occured.
 *  If that is the case it increases the opponents score and checks whether the game is now over.
 *	If not a new rally is started with the opponent team serving.
 */
void TieBreakReferee::ballOnBlobb(TEAM team)
{
	if (mActive) 
	{
		TEAM opponent = getOpponent(team);
		resetContacts(opponent);
		if (increaseContacts(team) > getMaximumContacts())
		{
			mGame->playSoundWhistle();

			increaseScore(opponent);

			if (isGameOver())			//only opponent could have scored since last test
			{
				gameOver(opponent);		//if game over then opponent must be winner
				mActive = false;
			}
			else
			{	
				newServe(opponent);
				resetContacts(team);

				mActive = false;
			}
		}
	}
}

/**
 *	Handles the case when the ball touches the ground on a teams side.
 *	The opponent teams score is increased and a check is done whether the game is now over.
 *	The opponent team serves the new ball if the game is not over yet and all contact-counters are reset.
 */
void TieBreakReferee::ballOnField(TEAM team)
{
	if (mActive) 
	{
		mGame->playSoundWhistle();

		TEAM opponent = getOpponent(team);
		increaseScore(opponent);		//opponent of 'team scores'
		if (isGameOver())				//only opponent could have scored since last test
		{
			gameOver(opponent);			//if game over then opponent must be winner
			mActive = false;
		}
		else
		{	
			newServe(opponent);
			resetContacts(team);		//reset current contact counters
			resetContacts(opponent);

			mActive = false;
		}
	}
}

/**
 *	Resets all values for a new game to start.
 */
void TieBreakReferee::startNewGame()
{
	Referee::startNewGame();
}