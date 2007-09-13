#ifndef BV3D_TIEBREAKREFEREE
#define BV3D_TIEBREAKREFEREE

#include <vrs/sharedobj.h>
#include "Referee.h"

namespace BV3D
{
	/**
	* \brief Implements the newer, tournament volleyball rules.

	* The game goes until one team reaches a score of 25 or more with 2 or more points difference to the other team.
	* Teams can touch the ball only 3 times before causing a fault each fault causes the other team to score a point and get to serve the ball.
	*/
	class TieBreakReferee :	public VRS::SharedObj, public BV3D::Referee
	{
	public:
		TieBreakReferee(VRS::SO<BV3D::Game> game);
		~TieBreakReferee(void){}
		
		void ballOnBlobb(BV3D::TEAM team);
		void ballOnField(BV3D::TEAM team);
	};
}

#endif //  #ifndef BV3D_TIEBREAKREFEREE