#include "ModelOptimizer.h"

#include <vrs/io/threedsreader.h>
#include <vrs/io/wavefrontreader.h>
#include <vrs/io/filedataresource.h>
#include <vrs/sg/scenegraphanalyzer.h>
#include <vrs/opengl/shapematerialgl.h>
#include <vrs/cache.h>

/**
 * Loads a 3ds model with the given options and optimizes it
 */
VRS::SO<VRS::SceneThing> BV3D::ModelOptimizer::get3dsModel(const std::string &fileName, bool useStaticLighting, ReadMode readMode)
{
	mStaticLighting = useStaticLighting;
	switch(readMode)		//setup 3dsReader according to readMode
	{
		case MATERIAL_AND_TEXTURES:
			VRS::ThreeDSReader::setMaterialMode(VRS::ThreeDSReader::ALL_BUT_CULLING_MATERIAL);
			VRS::ThreeDSReader::setTextureMode(VRS::ThreeDSReader::TRY_TEXTURES);
			break;
		case ONLY_MATERIAL:
			VRS::ThreeDSReader::setMaterialMode(VRS::ThreeDSReader::ALL_BUT_CULLING_MATERIAL);
			VRS::ThreeDSReader::setTextureMode(VRS::ThreeDSReader::NO_TEXTURES);
			break;
		case NO_MATERIAL_NO_TEXTURES:
			VRS::ThreeDSReader::setMaterialMode(VRS::ThreeDSReader::NO_MATERIAL);
			VRS::ThreeDSReader::setTextureMode(VRS::ThreeDSReader::NO_TEXTURES);
			break;
		case ALL_AND_NO_OPTIMIZATIONS:
			VRS::ThreeDSReader::setMaterialMode(VRS::ThreeDSReader::COMPLETE_MATERIAL);
			VRS::ThreeDSReader::setTextureMode(VRS::ThreeDSReader::FORCE_TEXTURES);
			VRS::ThreeDSReader::setOptimizations(VRS::ThreeDSReader::NO_OPTIMIZATIONS);
			return VRS::ThreeDSReader::readObject(fileName);
			break;
	}
	VRS::ThreeDSReader::setOptimizations(VRS::ThreeDSReader::EXCLUSIVE_OPENGL);
	return optimizeModel(VRS::ThreeDSReader::readObject(fileName));
}

/**
 * Loads an obj model and optimizes it
 */
VRS::SO<VRS::SceneThing> BV3D::ModelOptimizer::getWavefrontModel(const std::string &fileName)
{
	VRS::ID id = VRS::ID("something");
	VRS::WavefrontReader reader = VRS::WavefrontReader();
	VRS::SO<VRS::FileDataResource> file = new VRS::FileDataResource(fileName);
	VRS::SO<VRS::SceneThing> model = VRS_Cast(VRS::SceneThing, reader.read(file, id));
	return optimizeModel(model);
}

/**
 * Optimizes the given Scenegraph and returns a new one.
 */
VRS::SO<VRS::SceneThing> BV3D::ModelOptimizer::optimizeModel(VRS::SO<VRS::SceneThing> origModel)
{
	VRS::SceneGraphAnalyzer analyzer = VRS::SceneGraphAnalyzer();
	VRS::SO<VRS::SceneThing> newModel, model;
	newModel = analyzer.createOptimizedScene(origModel, true, mStaticLighting);
	if(!newModel)
		newModel = origModel;

	mStaticLighting = false;
	//We wanted to use analyzer.collapseScene followed by tool.mergePolygonSets,
	//but collapseScene removes all material-information from the graph.
	//This would cause all loaded objects to appear grey, so unfortunately it is of no use to us.
	return newModel;
}