#ifndef BV3D_TIEBREAKREFEREE
#define BV3D_TIEBREAKREFEREE

#include <vrs/sharedobj.h>
#include "Referee.h"


namespace BV3D
{
	/*
	* \brief Implements the newer, tournament volleyball rules.

	* The game goes until one team reaches a score of 25 or more with 2 or more points difference to the other team.
	* Teams can touch the ball only 3 times before causing a fault each fault causes the other team to score a point and get to serve the ball.
	*/
	class TieBreakReferee :	public SharedObj, public Referee
	{
	public:
		TieBreakReferee(SO<Game> game);
		~TieBreakReferee(void);
		
		void ballOnBlobb(TEAM team);
		void ballOnField(TEAM team);
		void startNewGame();
	};
}

#endif //  #ifndef BV3D_TIEBREAKREFEREE