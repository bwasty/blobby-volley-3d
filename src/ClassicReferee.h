#ifndef BV3D_CLASSICREFEREE
#define BV3D_CLASSICREFEREE

#include <vrs/sharedobj.h>
#include "Referee.h"

namespace BV3D
{
	/*
	* \brief Implements the traditional volleyball rules.

	* Traditional rules means, that only the team that served can get a point. If the serving team caused a fault the opponent team gets the serve, but no points.
	* Teams can touch the ball only 3 times before causing a fault and the game goes until one team reaches a score of 15 or more with 2 or more points difference to the other team.
	*/
	class ClassicReferee :
		public VRS::SharedObj, public BV3D::Referee
	{
	public:
		ClassicReferee(VRS::SO<BV3D::Game> game);
		~ClassicReferee(void){}

		void ballOnBlobb(BV3D::TEAM team);
		void ballOnField(BV3D::TEAM team);	

	};
}

#endif  // #ifndef BV3D_CLASSICREFEREE