/**
 * SceneLoader is responsible for loading and setting up different surroundings
 */
#ifndef SCENELOADER
#define SCENELOADER

#include <vrs/sharedobj.h>

namespace VRS{
	class SceneThing;
	template<class T> class SO;
	class SceneThing;
	class ThreeDSReader;
	class Light;
	class Image;
	class Canvas;
}

static const double			bumpmapOffset = 10.0;

namespace BV3D
{
	class ModelOptimizer;

	class SceneLoader :	public VRS::SharedObj
	{
	public:
		SceneLoader(VRS::SO<VRS::Canvas> canvas);
		~SceneLoader(void);
		
		VRS::SO<VRS::SceneThing>	loadBeach();
		VRS::SO<VRS::SceneThing>	loadArena();
		VRS::SO<VRS::SceneThing>	loadHeaven();

		VRS::SO<VRS::SceneThing>	getDesertSkybox();
		VRS::SO<VRS::SceneThing>	getWaterscapeSkybox();
		VRS::SO<VRS::SceneThing>	getIslandsSkybox();
		VRS::SO<VRS::SceneThing>	getHotDesertSkybox();
		VRS::SO<VRS::SceneThing>	getBrightDaySkybox();
		VRS::SO<VRS::SceneThing>	getZolsky6Skybox();
		VRS::SO<VRS::SceneThing>	getZolsky9Skybox();

	protected:
		VRS::SO<VRS::Canvas>		mCanvas;
		VRS::SO<ModelOptimizer>		mOptimizer;
		VRS::SO<VRS::SceneThing>	getFieldLines();
	};
}

#endif	// #ifndef BV3D_CONTROLS