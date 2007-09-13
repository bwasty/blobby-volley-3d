#include "TieBreakReferee.h"
#include "Game.h"

/**
 *	Sets up a Referee for new tournament volleyball rules, with a winning score of 25, a minimum difference between teams of 2 and a maximum of 3 contacts with the ball.
 */
BV3D::TieBreakReferee::TieBreakReferee(VRS::SO<BV3D::Game> game) : BV3D::Referee(game)
{
	setWinningScore(25);
	setMaximumContacts(3);
	setMinimumDifference(2);
}

/**
 *	Handles the case when the ball collides with a blobb from a team.
 *  Increases the teams contact-counter and checks whether a fault occured.
 *  If that is the case it increases the opponents score and checks whether the game is now over.
 *	If not a new rally is started with the opponent team serving.
 */
void BV3D::TieBreakReferee::ballOnBlobb(BV3D::TEAM team)
{
	if (mActive) 
	{
		BV3D::TEAM opponent = getOpponent(team);
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
void BV3D::TieBreakReferee::ballOnField(BV3D::TEAM team)
{
	if (mActive) 
	{
		mGame->playSoundWhistle();

		BV3D::TEAM opponent = getOpponent(team);
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