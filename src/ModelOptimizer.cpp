#include "ModelOptimizer.h"

#include <vrs/io/threedsreader.h>
#include <vrs/io/wavefrontreader.h>
#include <vrs/io/filedataresource.h>
#include <vrs/sg/scenegraphanalyzer.h>
#include <vrs/polygonsettool.h>
#include <vrs/polygonset.h>
#include <vrs/opengl/shapematerialgl.h>

BV3D::ModelOptimizer::ModelOptimizer(void)
{
	mStaticLighting = false;
}

BV3D::ModelOptimizer::~ModelOptimizer(void)
{
}

/*
 *
 */
VRS::SO<VRS::SceneThing> BV3D::ModelOptimizer::get3dsModel(const std::string &fileName, bool useStaticLighting, ReadMode readMode)
{
	mStaticLighting = useStaticLighting;
	switch(readMode)
	{
		case ReadMode::MATERIAL_AND_TEXTURES:
			VRS::ThreeDSReader::setMaterialMode(VRS::ThreeDSReader::ALL_BUT_CULLING_MATERIAL);
			VRS::ThreeDSReader::setTextureMode(VRS::ThreeDSReader::TRY_TEXTURES);
			break;
		case ReadMode::ONLY_MATERIAL:
			VRS::ThreeDSReader::setMaterialMode(VRS::ThreeDSReader::ALL_BUT_CULLING_MATERIAL);
			VRS::ThreeDSReader::setTextureMode(VRS::ThreeDSReader::NO_TEXTURES);
			break;
		case ReadMode::NO_MATERIAL_NO_TEXTURES:
			VRS::ThreeDSReader::setMaterialMode(VRS::ThreeDSReader::NO_MATERIAL);
			VRS::ThreeDSReader::setTextureMode(VRS::ThreeDSReader::NO_TEXTURES);
			break;
		case ReadMode::ALL_AND_NO_OPTIMIZATIONS:
			VRS::ThreeDSReader::setMaterialMode(VRS::ThreeDSReader::COMPLETE_MATERIAL);
			VRS::ThreeDSReader::setTextureMode(VRS::ThreeDSReader::FORCE_TEXTURES);
			VRS::ThreeDSReader::setOptimizations(VRS::ThreeDSReader::NO_OPTIMIZATIONS);
			return VRS::ThreeDSReader::readObject(fileName);
			break;
	}
	VRS::ThreeDSReader::setOptimizations(VRS::ThreeDSReader::EXCLUSIVE_OPENGL);// || VRS::ThreeDSReader::DISCARD_UNUSED_DATA);
	return optimizeModel(VRS::ThreeDSReader::readObject(fileName));
		
}

VRS::SO<VRS::SceneThing> BV3D::ModelOptimizer::getWavefrontModel(const std::string &fileName)
{
	VRS::ID id = VRS::ID("ball");
	VRS::WavefrontReader reader = VRS::WavefrontReader();
	VRS::SO<VRS::FileDataResource> file = new VRS::FileDataResource(fileName);
	VRS::SO<VRS::SceneThing> model = new VRS::SceneThing();
	model->append(reader.read(file, id));
	return optimizeModel(model);
}

VRS::SO<VRS::SceneThing> BV3D::ModelOptimizer::optimizeModel(VRS::SO<VRS::SceneThing> origModel)
{
	VRS::SceneGraphAnalyzer analyzer = VRS::SceneGraphAnalyzer();
	VRS::PolygonSetTool tool = VRS::PolygonSetTool();
	VRS::SO<VRS::SceneThing> newModel, model;
	//VRS::SO<VRS::Iterator<VRS::SO<VRS::PolygonSet>>> setIt;
	VRS::SO<VRS::PolygonSet> set;
	//VRS::SO<VRS::ShapeMaterialGL> material;
	//VRS::SceneGraphAnalyzer::Result result = analyzer.findObjects(VRS::ID("ShapeMaterialGL"), origModel);
	//material = VRS_Cast(VRS::ShapeMaterialGL, result.objects->get(0));

	newModel = analyzer.createOptimizedScene(origModel, true, mStaticLighting);
	if(!newModel)
	newModel = origModel;

	/*set = tool.mergePolygonSets(analyzer.collapseScene(newModel, true));
	tool.computeSmoothingNormals(set);
	model = new VRS::SceneThing();
	model->append(material);
	model->append(set);*/
	mStaticLighting = false;
	return newModel;
}

VRS::SO<VRS::ShapeMaterialGL> BV3D::ModelOptimizer::getMaterial(VRS::SO<VRS::SceneThing> model)
{
	
}