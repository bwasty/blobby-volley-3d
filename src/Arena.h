#ifndef BV3D_ARENA
#define BV3D_ARENA

#include <vrs/bounds.h>

using namespace VRS;

namespace BV3D {
	class Arena {
	public:
		Arena();
		~Arena();
		void setExtent(Vector extent);
		Bounds getTeamBounds(int nTeam);
	protected:
		Bounds	m_Bounds;
	};
}

#endif	//BV3D_ARENA