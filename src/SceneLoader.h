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
	class Light;
	class Image;
}

namespace BV3D
{
	class SceneLoader :	public VRS::SharedObj
	{
	public:
		SceneLoader(void);
		~SceneLoader(void);
		VRS::SO<VRS::SceneThing> loadBeach();

		VRS::SO<VRS::SceneThing> loadDesertSkybox();
		VRS::SO<VRS::SceneThing> loadWaterscapeSkybox();
		VRS::SO<VRS::SceneThing> loadIslandsSkybox();
		VRS::SO<VRS::SceneThing> loadHotDesertSkybox();
		VRS::SO<VRS::SceneThing> loadBrightDaySkybox();
		VRS::SO<VRS::SceneThing> loadZolsky6Skybox();

	};
}

#endif	// #ifndef BV3D_CONTROLS