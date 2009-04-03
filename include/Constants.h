#pragma once

#include <Ogre.h> //TODO: remove

namespace BV3D
{
		enum TEAM { TEAM1, TEAM2 };
		
		//enum BV3D_ERROR	{
		//	NO_ERROR,
		//	INVALID_PARAMETER
		//};

		enum CAMERAPOSITION	{
			CLASSIC_CAMERA,
			TEAM1_BASECAMERA,
			REVERSE_CAMERA,
			TEAM2_BASECAMERA,
			MAX_CAMERAS
		};

		static const Vector3 ARENA_EXTENT = Vector3(16.0,30.0,7.0);
		static const double	NET_HEIGHT = 4.2;
		static const double LINES_WIDTH = 0.2;
		static const double BALL_RADIUS = 0.9;
		static const double BLOBB_HEIGHT = 2.6;
		static const double BLOBB_ALPHA = 0.7;
		static const double BLOBB_JUMP_SPEED = 14.0;
		static const double BLOBB_MAX_JUMP_HEIGHT = 2.0;
		static const double BLOBB_MOVEMENT_STEP_DISTANCE = 12.0;	// blobb step distance
		static const double BLOBB_GRAVITY_MULTIPLIER = 3.5;			// the GRAVITY for the blobbs is stronger, because they should fall down faster than the ball
		//static const std::string MODELS_PATH = "../data/models/";
		//static const std::string GRAFICS_PATH = "../data/grafics/";
		//static const std::string SOUNDS_PATH = "../data/sounds/";
		static const Vector3 LOOK_TO = Vector3(0.0, 4.0, 0.0);
		static const Vector3 LOOK_FROM = Vector3(0.0, 9.0, -16.0);
		static const float MAX_BALL_VELOCITY = 20.0;
		static const int MOUSE_MAX_DELTA = 20;	// maximum accepted mouse movement in pixels (see MouseControls)

		static const double SHADOW_HEIGHT = 0.01;
		static const double SHADOW_MAX_HEIGHT = 20.0;

		static const double GRAVITY = 21.0;
		static const double MAX_FPS = 32.0;

		// data per blobb:
		//	lower vertical offset/radius, lower horizontal radius
		//	upper vertical offset, upper horizontal radius, upper vertical radius
		static const float BLOBB_SHAPE_DATA[5][5] = {
			{1.0f+0.1f, 1.0f, 1.5f, 0.9f, 1.1f},
			{0.9f+0.1f, 1.01f, 1.35f, 0.9f, 1.1f},
			{0.8f+0.1f, 1.05f, 1.2f, 0.9f, 1.1f},
			{0.65f+0.1f, 1.1f, 1.1f, 0.9f, 1.0f},
			{0.5f+0.1f, 1.2f, 1.0f, 0.9f, 0.9f} };
};

