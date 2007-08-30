/**
 * SceneLoader is responsible for loading and setting up different surroundings
 */
#ifndef BV3D_SCENELOADER
#define BV3D_SCENELOADER

#include <vrs/sharedobj.h>

namespace VRS{
	class SceneThing;
	template<class T> class SO;
	class SceneThing;
	class ThreeDSReader;
}

namespace BV3D
{
	class SceneLoader :	public VRS::SharedObj
	{
	public:
		SceneLoader(void);
		~SceneLoader(void);
		VRS::SO<VRS::SceneThing> loadBeach();
	};
}

#endif	// #ifndef BV3D_CONTROLS