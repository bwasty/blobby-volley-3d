#ifndef BV3D_ARENA
#define BV3D_ARENA

#include <vrs/bounds.h>

using namespace VRS;

namespace BV3D {
	class Game;		// prototype

	class Arena {
	public:
		Arena(Game* game);
		~Arena();
		void setExtent(Vector extent);
		Bounds getTeamBounds(int nTeam);
	protected:
		Arena();
		Game*	m_Game;
		Bounds	m_Bounds;
	};
}

#endif	//BV3D_ARENA