/*
 * The ClassicReferee class is a subclass of Referee and implements the traditional volleyball rules.
 * Traditional rules means, that only the team that served can get a point. If the serving team caused a fault the opponent team gets the serve, but no points.
 * Teams can touch the ball only 3 times before causing a fault and the game goes until one team reaches a score of 15 or more with 2 or more points difference to the other team.
 */

#include "Constants.h"
#include "ClassicReferee.h"
#include "Game.h"

using namespace BV3D;

/*
 *	Sets up a Referee for classic volleyball rules, with a winning score of 15, a minimum difference between teams of 2 and a maximum of 3 contacts with the ball.
 */
ClassicReferee::ClassicReferee(SO<Game> game) : Referee(game)
{
	setWinningScore(15);
	setMaximumContacts(3);
	setMinimumDifference(2);
	//game->newServe(true);
}

ClassicReferee::~ClassicReferee(void)
{
}


/*
 *	Handles the case when the ball collides with a blobb from a team.
 *  Increases the teams contact-counter and checks whether a fault occured.
 *  If that is the case it increases the opponents score(if that team served the ball) and checks whether the game is now over.
 *	If not a new rally is started with the opponent team serving.
 */
void ClassicReferee::ballOnBlobb(TEAM team)
{
	if (mActive) {
		TEAM opponent = getOpponent(team);
		resetContacts(opponent);
		if (increaseContacts(team) > getMaximumContacts())
		{
			mGame->playSoundWhistle();

			if (mServingTeam != team)
			{

				increaseScore(mServingTeam);

				if (isGameOver())					//only opponent could have scored since last test
				{
					gameOver(mServingTeam);	//if game over then opponent must be winner
					mActive = false;
				}
				else
				{
					mGame->scheduleNewServe();

					resetContacts(team);

					mActive = false;
				}
			}
			else
			{	
				mServingTeam = opponent;
				mGame->scheduleNewServe();

				resetContacts(team);

				mActive = false;
			}
			resetContacts(team);
		}
	}
}

/*
 *	Handles the case when the ball touches the ground on a teams side.
 *	If the opponent team served the ball their score is increased. In that case a check is done whether the game is now over.
 *	The opponent team serves the new ball if the game is not over yet and all contact-counters are reset.
 */
void ClassicReferee::ballOnField(TEAM team)
{
	if (mActive) {
		mGame->playSoundWhistle();

		if (mServingTeam != team)
		{
			increaseScore(mServingTeam);
			if (isGameOver())					//only servingTeam could have scored since last test
			{
				gameOver(mServingTeam);	//if game over then servingTeam must be winner
				mActive = false;
			}
			else
			{	
				mGame->scheduleNewServe();

				resetContacts(team);

				mActive = false;
			}
		}
		else
		{
			mServingTeam = getOpponent(team);
			mGame->scheduleNewServe();

			resetContacts(team);

			mActive = false;
		}
		resetContacts(TEAM1);
		resetContacts(TEAM2);
	}
}

/*
 *	Resets all values for a new game to start.
 */
void ClassicReferee::startNewGame()
{
	Referee::startNewGame();
	//game->newServe(true);
}