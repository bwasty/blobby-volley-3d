/**
 * ModelOptimizer is responsible for loading and preparing(optimizing) model files to 
 */
#ifndef BV3D_MODELOPTIMIZER
#define BV3D_MODELOPTIMIZER

#include <vrs/sharedobj.h>

namespace VRS{
	class SceneThing;
	template<class T> class SO;
	class ShapeMaterialGL;
}

namespace BV3D
{
	class ModelOptimizer : public VRS::SharedObj
	{
	public:
		ModelOptimizer(void);
		~ModelOptimizer(void);

		enum ReadMode{
				MATERIAL_AND_TEXTURES,
				ONLY_MATERIAL,
				NO_MATERIAL_NO_TEXTURES,
				ALL_AND_NO_OPTIMIZATIONS
		};

		VRS::SO<VRS::SceneThing> get3dsModel(const std::string &fileName, bool useStaticLighting = true, ReadMode readMode=ONLY_MATERIAL);
		VRS::SO<VRS::SceneThing> getWavefrontModel(const std::string &fileName);

	protected:
		bool	mStaticLighting;
		VRS::SO<VRS::SceneThing> optimizeModel(VRS::SO<VRS::SceneThing> model);
		VRS::SO<VRS::ShapeMaterialGL> getMaterial(VRS::SO<VRS::SceneThing> model);
	};
}

#endif	// #ifndef BV3D_MODELOPTIMIZER