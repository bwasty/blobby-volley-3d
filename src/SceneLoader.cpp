#include "SceneLoader.h"
#include <vrs/sg/scenething.h>
#include <vrs/io/threedsreader.h>
#include <vrs/disc.h>
#include <vrs/opengl/shapematerialgl.h>

#include "Constants.h"

BV3D::SceneLoader::SceneLoader(void)
{
}

BV3D::SceneLoader::~SceneLoader(void)
{
}

VRS::SO<VRS::SceneThing> BV3D::SceneLoader::loadBeach()
{
	VRS::SO<VRS::SceneThing> beachScene = new VRS::SceneThing();

	double scalYucca2, scalPalm;
	scalYucca2 = scalPalm = 0.02;
	VRS::Matrix vrsMatrix, scalMatrix;

	//ground plane
	VRS::SO<VRS::SceneThing> groundPlane = new VRS::SceneThing();
	groundPlane->append(new VRS::ShapeMaterialGL(VRS::Color::yellow));
	groundPlane->append(new VRS::Disc(VRS::Vector(0.0, 0.0, 0.0), VRS::Vector(0.0, 1.0, 0.0), 10.0));
	beachScene->append(groundPlane);

	//palm trees
	VRS::SO<VRS::SceneThing> palmTrees = new VRS::SceneThing();
	VRS::ThreeDSReader::setMaterialMode(VRS::ThreeDSReader::ALL_BUT_CULLING_MATERIAL);	
	VRS::ThreeDSReader::setOptimizations(VRS::ThreeDSReader::EXCLUSIVE_OPENGL);
	vrsMatrix = VRS::Matrix::translation(VRS::Vector(-5.0, 0.0, -6.0));
	vrsMatrix = vrsMatrix * VRS::Matrix::scaling(VRS::Vector(scalPalm, scalPalm, scalPalm));
	palmTrees->setLocalMatrix(vrsMatrix);
	palmTrees->append(VRS::ThreeDSReader::readScene(BV3D::threeDSPath + "palmtrees1_mrealms.3ds", "", false, false));
	beachScene->append(palmTrees);

	//yucca plants
	VRS::SO<VRS::SceneThing> yucca2 = new VRS::SceneThing();
	vrsMatrix = VRS::Matrix::translation(VRS::Vector(0.0, 0.0, 7.0));
	vrsMatrix = vrsMatrix * VRS::Matrix::scaling(VRS::Vector(scalYucca2, scalYucca2, scalYucca2));
	yucca2->setLocalMatrix(vrsMatrix);
	yucca2->append(VRS::ThreeDSReader::readScene(BV3D::threeDSPath + "yucca2_mrealms.3ds", "", false, false));
	beachScene->append(yucca2);

	//garlic plant
	VRS::SO<VRS::SceneThing> garlic1 = new VRS::SceneThing();
	vrsMatrix = VRS::Matrix::translation(VRS::Vector(18.0, 0.0, 0.0));
	vrsMatrix = vrsMatrix * VRS::Matrix::scaling(VRS::Vector(0.1, 0.1, 0.1));
	garlic1->setLocalMatrix(vrsMatrix);
	VRS::ThreeDSReader::setMaterialMode(VRS::ThreeDSReader::ALL_BUT_CULLING_MATERIAL);
	VRS::ThreeDSReader::setTextureMode(VRS::ThreeDSReader::TRY_TEXTURES);
	VRS::ThreeDSReader::setOptimizations(VRS::ThreeDSReader::NO_OPTIMIZATIONS);
	garlic1->append(VRS::ThreeDSReader::readScene(BV3D::threeDSPath + "garlic_mrealms.3ds", "", false, false));
	beachScene->append(garlic1);

	printf("Beach loaded\n");
	return beachScene;
}