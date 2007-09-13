#include "ClassicReferee.h"
#include "Game.h"

/**
 *	Sets up a Referee for classic volleyball rules, with a winning score of 15, a minimum difference between teams of 2 and a maximum of 3 contacts with the ball.
 */
BV3D::ClassicReferee::ClassicReferee(VRS::SO<BV3D::Game> game) : BV3D::Referee(game)
{
	setWinningScore(15);
	setMaximumContacts(3);
	setMinimumDifference(2);
}

/**
 *	Handles the case when the ball collides with a blobb from a team.
 *  Increases the teams contact-counter and checks whether a fault occured.
 *  If that is the case it increases the opponents score(if that team served the ball) and checks whether the game is now over.
 *	If not a new rally is started with the opponent team serving.
 */
void BV3D::ClassicReferee::ballOnBlobb(BV3D::TEAM team)
{
	if (mActive) 
	{
		BV3D::TEAM opponent = getOpponent(team);
		resetContacts(opponent);
		if (increaseContacts(team) > getMaximumContacts())
		{
			mGame->playSoundWhistle();
			resetContacts(team);

			if (mServingTeam != team)
			{
				increaseScore(mServingTeam);

				if (isGameOver())				//only opponent could have scored since last test
				{
					gameOver(mServingTeam);		//if game over then opponent must be winner
					mActive = false;
				}
				else
				{
					newServe(mServingTeam);
					mActive = false;
				}
			}
			else
			{	
				newServe(opponent);
				mActive = false;
			}
		}
	}
}

/**
 *	Handles the case when the ball touches the ground on a teams side.
 *	If the opponent team served the ball their score is increased. In that case a check is done whether the game is now over.
 *	The opponent team serves the new ball if the game is not over yet and all contact-counters are reset.
 */
void BV3D::ClassicReferee::ballOnField(BV3D::TEAM team)
{
	if (mActive) {
		mGame->playSoundWhistle();
		resetContacts(BV3D::TEAM1);
		resetContacts(BV3D::TEAM2);

		if (mServingTeam != team)
		{
			increaseScore(mServingTeam);
			if (isGameOver())				//only servingTeam could have scored since last test
			{
				gameOver(mServingTeam);		//if game over then servingTeam must be winner
				mActive = false;
			}
			else
			{	
				newServe(mServingTeam);
				mActive = false;
			}
		}
		else
		{
			newServe(getOpponent(team));
			mActive = false;
		}
	}
}