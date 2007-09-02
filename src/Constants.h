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

		static const VRS::Vector arenaExtent = VRS::Vector(12.0,10.0,6.0);
		static const double netHeight = 4.0;
		static const double ballRadius = 0.9;
		static const double blobbHeight = 2.6;
		static const std::string threeDSPath = "../Modelle/3ds/";
		static const std::string cubemapsPath = "../Modelle/cubemaps/";
		static const VRS::Vector lookTo = VRS::Vector(0.0, 3.0, 0.0);

};

#endif	// BV3D_CONSTANTS