#ifndef BV3D_CONSTANTS
#define BV3D_CONSTANTS

#include <vrs/vector.h>
//namespace VRS{
//	class Vector;
//}
namespace BV3D
{
		enum BV3D_TEAM { BV3D_TEAM1, BV3D_TEAM2 };
		
		enum BV3D_ERROR
		{
			BV3D_NO_ERROR,
			BV3D_INVALID_PARAMETER
		};

		enum BV3D_SCENE
		{
			BV3D_BEACH,
			BV3D_ARENA
		};

		static const VRS::Vector arenaExtent = VRS::Vector(8.0,10.0,6.0);
		static const double ballRadius = 1.0;
		static const double blobbHeight = 1.0;

};

#endif	// BV3D_CONSTANTS