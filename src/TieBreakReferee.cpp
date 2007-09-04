/*
 * The TieBreakReferee class is a subclass of Referee and implements the newer tournament volleyball rules.
 * The game goes until one team reaches a score of 25 or more with 2 or more points difference to the other team.
 * Teams can touch the ball only 3 times before causing a fault each fault causes the other team to score a point and get to serve the ball.
 */

#include "Constants.h"
#include "TieBreakReferee.h"
#include "Game.h"

using namespace BV3D;

/*
 *	Sets up a Referee for new tournament volleyball rules, with a winning score of 25, a minimum difference between teams of 2 and a maximum of 3 contacts with the ball.
 */
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

/*
 *	Handles the case when the ball collides with a blobb from a team.
 *  Increases the teams contact-counter and checks whether a fault occured.
 *  If that is the case it increases the opponents score and checks whether the game is now over.
 *	If not a new rally is started with the opponent team serving.
 */
void TieBreakReferee::ballOnBlobb(BV3D_TEAM team)
{
	if (m_Active) {

		BV3D_TEAM opponent = getOpponent(team);
		resetContacts(opponent);
		if (increaseContacts(team) > getMaximumContacts())
		{
			m_game->playSoundWhistle();

			increaseScore(opponent);

			if (isGameOver())					//only opponent could have scored since last test
			{
				gameOver(opponent);	//if game over then opponent must be winner
				m_Active = false;
			}
			else
			{	
				m_ServingTeam = opponent;
				m_game->scheduleNewServe();

				resetContacts(team);

				m_Active = false;
			}
		}
	}
}

/*
 *	Handles the case when the ball touches the ground on a teams side.
 *	The opponent teams score is increased and a check is done whether the game is now over.
 *	The opponent team serves the new ball if the game is not over yet and all contact-counters are reset.
 */
void TieBreakReferee::ballOnField(BV3D_TEAM team)
{
	if (m_Active) {
		m_game->playSoundWhistle();

		BV3D_TEAM opponent = getOpponent(team);
		increaseScore(opponent);	//opponent of 'isInLeftField' scores
		if (isGameOver())					//only opponent could have scored since last test
		{
			gameOver(opponent);	//if game over then opponent must be winner
			m_Active = false;
		}
		else
		{	//m_game->newServe(opponent);
			m_ServingTeam = opponent;

			// TODO: call after some time...
			m_game->scheduleNewServe();

			resetContacts(team);		//reset current contact counters
			resetContacts(opponent);

			m_Active = false;
		}
	}
}

/*
 *	Resets all values for a new game to start.
 */
void TieBreakReferee::startNewGame()
{
	Referee::startNewGame();
	//game->newServe(BV3D_TEAM1);
}