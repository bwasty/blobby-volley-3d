#ifndef BV3D_CONSTANTS
#define BV3D_CONSTANTS

#include <vrs/vector.h>
//namespace VRS{
//	class Vector;
//}
namespace BV3D
{
		enum TEAM { TEAM1, TEAM2 };
		
		enum BV3D_ERROR
		{
			NO_ERROR,
			INVALID_PARAMETER
		};

		enum SCENE
		{
			BEACH,
			ARENA,
			HEAVEN
		};

		enum CAMERAPOSITION
		{
			CLASSIC_CAMERA,
			TEAM1_BASECAMERA,
			REVERSE_CAMERA,
			TEAM2_BASECAMERA,
			MAX_CAMERAS
		};

		static const VRS::Vector arenaExtent = VRS::Vector(16.0,20.0,6.0);
		static const double netHeight = 4.2;
		static const double linesWidth = 0.2;
		static const double ballRadius = 0.9;
		static const double blobbHeight = 2.6;
		static const double blobbAlpha = 0.7;
		static const std::string threeDSPath = "../Modelle/3ds/";
		static const std::string cubemapsPath = "../Modelle/cubemaps/";
		static const VRS::Vector lookTo = VRS::Vector(0.0, 4.0, 0.0);
		static const VRS::Vector lookFrom = VRS::Vector(0.0, 9.0, -16.0);
		static const float maxBallVelocity = 20.0;

		static const double shadowHeight = 0.01f;
		static const double shadowMaxHeight = 20.0f;
};

#endif	// BV3D_CONSTANTS
