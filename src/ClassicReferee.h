#ifndef BV3D_CLASSICREFEREE
#define BV3D_CLASSICREFEREE

#include <vrs/sharedobj.h>
#include "Referee.h"

namespace BV3D
{
	class ClassicReferee :
		public SharedObj, public Referee
	{
	public:
		ClassicReferee(SO<Game> game);
		~ClassicReferee(void);

		void ballOnBlobb(bool isInLeftField);
		void ballOnField(bool isInLeftField);	
		void startNewGame();

	private:
		bool m_isLeftSideServing;
	};
}

#endif  // #ifndef BV3D_CLASSICREFEREE