#ifndef BV3D_TIEBREAKREFEREE
#define BV3D_TIEBREAKREFEREE

#include <vrs/sharedobj.h>
#include "Referee.h"


namespace BV3D
{
	class TieBreakReferee :	public SharedObj, public Referee
	{
	public:
		TieBreakReferee(SO<Game> game);
		~TieBreakReferee(void);
		
		void ballOnBlobb(bool isInLeftField);
		void ballOnField(bool isInLeftField);
		void startNewGame();
	};
}

#endif //  #ifndef BV3D_TIEBREAKREFEREE