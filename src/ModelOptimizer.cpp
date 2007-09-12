#include "ModelOptimizer.h"

#include <vrs/io/threedsreader.h>
#include <vrs/io/wavefrontreader.h>
#include <vrs/io/filedataresource.h>
#include <vrs/sg/scenegraphanalyzer.h>
#include <vrs/polygonsettool.h>
#include <vrs/polygonset.h>
#include <vrs/opengl/shapematerialgl.h>
#include <vrs/cache.h>

BV3D::ModelOptimizer::ModelOptimizer(void)
{
	mStaticLighting = false;
}

BV3D::ModelOptimizer::~ModelOptimizer(void)
{
}

/**
 * loads a 3ds model with the given options and optimizes it
 */
VRS::SO<VRS::SceneThing> BV3D::ModelOptimizer::get3dsModel(const std::string &fileName, bool useStaticLighting, ReadMode readMode)
{
	VRS::SO<VRS::SceneThing> ball;
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
 * loads an obj model and optimizes it
 */
VRS::SO<VRS::SceneThing> BV3D::ModelOptimizer::getWavefrontModel(const std::string &fileName)
{
	VRS::ID id = VRS::ID("something");
	VRS::WavefrontReader reader = VRS::WavefrontReader();
	VRS::SO<VRS::FileDataResource> file = new VRS::FileDataResource(fileName);
	VRS::SO<VRS::SceneThing> model = new VRS::SceneThing();
	VRS::SO<VRS::SceneThing> ball = VRS_Cast(VRS::SceneThing, reader.read(file, id));
	//cache shape for better performance
	//printf("Objects in ball model:\n");
	ball = optimizeModel(ball);
	//for(int i=0; i< ball->objects(); i++)
	//{
	//	//printf("    %s\n", ball->object(i)->classNameVRS().text());
	//	if (ball->object(i)->isA(VRS::SceneThing::ClassNameVRS()))
	//	{
	//		VRS::SO<VRS::SceneThing> scene = VRS_Cast(VRS::SceneThing, ball->object(i));
	//		for(int j=0; j< scene->objects(); j++)
	//		{
	//			if (scene->object(j)->isA(VRS::Shape::ClassNameVRS()))
	//			{
	//				printf("        %s\n", scene->object(j)->classNameVRS().text());
	//				scene->replace(scene->object(j), new VRS::Cache(VRS_Cast(VRS::Shape, scene->object(j))));
	//			}
	//		}
	//	}
	//}
	model->append(ball);
	return model;
}

/**
 * Optimizes the given Scenegraph and returns a new one.
 */
VRS::SO<VRS::SceneThing> BV3D::ModelOptimizer::optimizeModel(VRS::SO<VRS::SceneThing> origModel)
{
	VRS::SceneGraphAnalyzer analyzer = VRS::SceneGraphAnalyzer();
	VRS::PolygonSetTool tool = VRS::PolygonSetTool();
	VRS::SO<VRS::SceneThing> newModel, model;
	VRS::SO<VRS::PolygonSet> set;
	newModel = analyzer.createOptimizedScene(origModel, true, mStaticLighting);
	if(!newModel)
		newModel = origModel;

	mStaticLighting = false;
	//We wanted to use analyzer.collapseScene followed by tool.mergePolygonSets,
	//but collapseScene removes all material-information from the graph.
	//This would cause all loaded objects to appear grey, so unfortunately it is of no use to us.
	return newModel;
}