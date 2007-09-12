#include "SceneLoader.h"
#include "Constants.h"

#include <vrs/sg/scenething.h>
#include <vrs/io/threedsreader.h>
#include <vrs/disc.h>
#include <vrs/opengl/shapematerialgl.h>
#include <vrs/image/image.h>
#include <vrs/bumpmap.h>
#include <vrs/sky.h>
#include <vrs/distantlight.h>
#include <vrs/matrix.h>
#include <vrs/opengl/imagecubemaptexturegl.h>
#include <vrs/io/pngreader.h>
#include <vrs/io/jpegreader.h>
#include <vrs/opengl/backgroundgl.h>
#include <vrs/fog.h>
#include <vrs/box.h>
#include <vrs/translation.h>
#include <vrs/sg/cullingscenething.h>
#include <vrs/sg/canvas.h>
#include <vrs/opengl/imagetexture2dgl.h>
#include <vrs/cylinder.h>
#include <vrs/facestyle.h>
//#include <vrs/opengl/texturegl.h>


#include "ModelOptimizer.h"

BV3D::SceneLoader::SceneLoader(VRS::SO<VRS::Canvas> canvas)
{
	mCanvas = canvas;
	mOptimizer = new ModelOptimizer();
}

BV3D::SceneLoader::~SceneLoader(void)
{
}

/**
 * Returns a scene graph, which includes white lines(base- and sidelines) that fit the extent of the gamefield.
 */
VRS::SO<VRS::SceneThing> BV3D::SceneLoader::getFieldLines()
{
	VRS::Vector ext = BV3D::ARENA_EXTENT;
	double width = BV3D::LINES_WIDTH;
	VRS::SO<VRS::SceneThing> lines = new VRS::SceneThing();
	lines->append(new VRS::ShapeMaterialGL(VRS::Color(1.0)));
	//front sideline
	lines->append(new VRS::Box(VRS::Vector(-ext[0]/2, 0, -ext[2]/2), VRS::Vector(ext[0]/2, 0.01, -ext[2]/2 + width)));
	//back sideline
	lines->append(new VRS::Box(VRS::Vector(-ext[0]/2, 0, ext[2]/2), VRS::Vector(ext[0]/2, 0.01, ext[2]/2 - width)));
	//left baseline
	lines->append(new VRS::Box(VRS::Vector(-ext[0]/2, 0, -ext[2]/2), VRS::Vector(-ext[0]/2 + width, 0.01, ext[2]/2)));
	//right baseline
	lines->append(new VRS::Box(VRS::Vector(ext[0]/2, 0, -ext[2]/2), VRS::Vector(ext[0]/2 - width, 0.01, ext[2]/2)));
	return lines;
}

/**
 * Returns a scene graph, which includes a complete scenerie of a beach.
 */
VRS::SO<VRS::SceneThing> BV3D::SceneLoader::loadBeach()
{
	VRS::SO<VRS::SceneThing> beachScene = new VRS::SceneThing();
	VRS::Vector extent = BV3D::ARENA_EXTENT;
	double scalYucca2, scalPalm;
	scalYucca2 = scalPalm = 0.02;
	VRS::Matrix vrsMatrix, scalMatrix;

	//background
	VRS::SO<VRS::SceneThing> background = new VRS::SceneThing(beachScene);
	background->append(getZolsky8Skybox());//getBrightDaySkybox());

	//ground plane with sand bumpmap
	VRS::SO<VRS::SceneThing> groundPlane = new VRS::SceneThing(beachScene);
	VRS::SO<VRS::DistantLight> light = new VRS::DistantLight(VRS::Vector(0.0, 1.0, 1.0), VRS::Color(1.0));
	VRS::SO<VRS::Image> img1 = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "2329-v2.jpg");
	VRS::SO<VRS::Bumpmap> bumpmap = new VRS::Bumpmap(img1, light, 4.0); 
	groundPlane->append(new VRS::Translation(VRS::Vector(0.0, 0.0, -bumpmapOffset)));
    groundPlane->append(bumpmap);
	groundPlane->append(new VRS::ShapeMaterialGL(VRS::Color(0.13, 0.29, 0.39), VRS::Color(0.0), 128.0));
	groundPlane->append(new VRS::Disc(VRS::Vector(0.0, -0.5, 0.0), VRS::Vector(0.0, 1.0, 0.0), 300.0));
	groundPlane->append(new VRS::ShapeMaterialGL(VRS::Color(0.6, 0.5, 0.2), VRS::Color(0.0), 128.0));
	groundPlane->append(new VRS::Disc(VRS::Vector(0.0, 0.0, 0.0), VRS::Vector(0.0, 1.0, 0.0), 50.0));
	groundPlane->append(new VRS::Translation(VRS::Vector(0.0, 0.0, bumpmapOffset)));
	groundPlane->append(getFieldLines());

	//palm trees
	VRS::SO<VRS::SceneThing> palmModel = new VRS::SceneThing();
	palmModel->setLocalMatrix(VRS::Matrix::scaling(VRS::Vector(scalPalm, scalPalm, scalPalm)));
	palmModel->append(mOptimizer->get3dsModel(BV3D::MODELS_PATH + "palmtrees1_mrealms.3ds"));
	VRS::SO<VRS::CullingSceneThing> palmScene = new VRS::CullingSceneThing(palmModel->boundingBox(mCanvas->engine()));
	palmScene->append(palmModel);

	VRS::SO<VRS::SceneThing> palmTree = new VRS::SceneThing(beachScene);
	vrsMatrix = VRS::Matrix::translation(VRS::Vector(-extent[0]/2, 0.0, extent[2] + 10));
	palmTree->setLocalMatrix(vrsMatrix);
	palmTree->append(palmScene);

	palmTree = new VRS::SceneThing(beachScene);
	vrsMatrix = VRS::Matrix::translation(VRS::Vector(-extent[0]/2, 0.0, extent[2] + 5));
	vrsMatrix = vrsMatrix * VRS::Matrix::rotation(VRS::Vector(0.0, 1.0, 0.0), VRS::Vector(0.0, 0.0, 0.0), 140.0);
	palmTree->setLocalMatrix(vrsMatrix);
	palmTree->append(palmScene);	

	palmTree = new VRS::SceneThing(beachScene);
	vrsMatrix = VRS::Matrix::translation(VRS::Vector(-extent[0]/3, 0.0, extent[2] + 8));
	vrsMatrix = vrsMatrix * VRS::Matrix::rotation(VRS::Vector(0.0, 1.0, 0.0), VRS::Vector(0.0, 0.0, 0.0), 70.0);
	palmTree->setLocalMatrix(vrsMatrix);
	palmTree->append(palmScene);

	palmTree = new VRS::SceneThing(beachScene);
	vrsMatrix = VRS::Matrix::translation(VRS::Vector(-extent[0]*2, 0.0, -(extent[2] + 8)));
	vrsMatrix = vrsMatrix * VRS::Matrix::rotation(VRS::Vector(0.0, 1.0, 0.0), VRS::Vector(0.0, 0.0, 0.0), 70.0);
	palmTree->setLocalMatrix(vrsMatrix);
	palmTree->append(palmScene);

	//yucca plants
	VRS::SO<VRS::SceneThing> yuccaModel = new VRS::SceneThing();
	yuccaModel->setLocalMatrix(VRS::Matrix::scaling(VRS::Vector(scalYucca2, scalYucca2, scalYucca2)));
	yuccaModel->append(mOptimizer->get3dsModel(BV3D::MODELS_PATH + "yucca_mrealms.3ds"));
	VRS::SO<VRS::CullingSceneThing> yuccaScene = new VRS::CullingSceneThing(yuccaModel->boundingBox(mCanvas->engine()));
	yuccaScene->append(yuccaModel);

	VRS::SO<VRS::SceneThing> yucca = new VRS::SceneThing(beachScene);
	yucca->setLocalMatrix(VRS::Matrix::translation(VRS::Vector(extent[0] / 2, 0.0, (extent[2] + 2))));
	yucca->append(yuccaScene);

	yucca = new VRS::SceneThing(beachScene);
	yucca->setLocalMatrix(VRS::Matrix::translation(VRS::Vector(-extent[0], 0.0, -2.0)));
	yucca->append(yuccaScene);

	//yucca 2
	VRS::SO<VRS::SceneThing> yucca2Model = new VRS::SceneThing();
	yucca2Model->setLocalMatrix(VRS::Matrix::scaling(VRS::Vector(scalYucca2, scalYucca2, scalYucca2)));
	yucca2Model->append(mOptimizer->get3dsModel(BV3D::MODELS_PATH + "yucca2_mrealms.3ds"));
	VRS::SO<VRS::CullingSceneThing> yucca2Scene = new VRS::CullingSceneThing(yucca2Model->boundingBox(mCanvas->engine()));
	yucca2Scene->append(yucca2Model);

	VRS::SO<VRS::SceneThing> yucca2 = new VRS::SceneThing();
	vrsMatrix = VRS::Matrix::translation(VRS::Vector(0.0, 0.0, -(extent[2] + 4)));
	yucca2->setLocalMatrix(vrsMatrix);
	yucca2->append(yucca2Scene);
	beachScene->append(yucca2);

	//garlic plant
	VRS::SO<VRS::SceneThing> garlicModel = new VRS::SceneThing();
	garlicModel->setLocalMatrix(VRS::Matrix::scaling(VRS::Vector(0.1, 0.1, 0.1)));
	garlicModel->append(mOptimizer->get3dsModel(BV3D::MODELS_PATH + "garlic_mrealms.3ds", true, ModelOptimizer::ALL_AND_NO_OPTIMIZATIONS));
	VRS::SO<VRS::CullingSceneThing> garlicScene = new VRS::CullingSceneThing(garlicModel->boundingBox(mCanvas->engine()));
	garlicScene->append(garlicModel);

	VRS::SO<VRS::SceneThing> garlic = new VRS::SceneThing(beachScene);
	vrsMatrix = VRS::Matrix::translation(VRS::Vector(20.0, 0.0, 0.0));
	garlic->setLocalMatrix(vrsMatrix);
	garlic->append(garlicScene);

	return beachScene;
}

/**
 * Returns a scene graph, including the scenerie of a kind of playground with a fenced-in field.
 */
VRS::SO<VRS::SceneThing> BV3D::SceneLoader::loadArena()
{
	VRS::SO<VRS::SceneThing> arenaScene = new VRS::SceneThing();
	VRS::Vector extent = BV3D::ARENA_EXTENT;
	double scalFenceBaseline = ARENA_EXTENT[2]/210.0;	//with this factor, fence has the depth of arena
	double scalFenceSideline = (ARENA_EXTENT[0]/2)/210.0;
	double scalFenceHeight = (2 * ARENA_EXTENT[2])/210.0;
	double scalSwing = 0.03;
	double scalStands = 0.015;
	double scalTex = 1.0;
	double groundRadius = 100.0;
	VRS::Matrix vrsMatrix;

	//background
	VRS::SO<VRS::SceneThing> background = new VRS::SceneThing(arenaScene);
	background->append(getZolsky9Skybox());

	//ground plane
	VRS::SO<VRS::SceneThing> groundPlane = new VRS::SceneThing(arenaScene);
	VRS::SO<VRS::DistantLight> light = new VRS::DistantLight(VRS::Vector(0.0, 1.0, 1.0), VRS::Color(1.0));
	VRS::SO<VRS::Image> img1 = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "orange.pgm");
	VRS::SO<VRS::Bumpmap> bumpmap = new VRS::Bumpmap(img1, light, 1.0);  
	groundPlane->append(new VRS::Translation(VRS::Vector(0.0, 0.0, -bumpmapOffset)));
    groundPlane->append(bumpmap);
	groundPlane->append(new VRS::ShapeMaterialGL(VRS::Color(0.2, 0.2, 0.2), VRS::Color(0.0), 128.0));
	groundPlane->append(new VRS::Disc(VRS::Vector(0.0, 0.0, 0.0), VRS::Vector(0.0, 1.0, 0.0), groundRadius + 10.0));
	groundPlane->append(new VRS::Translation(VRS::Vector(0.0, 0.0, bumpmapOffset)));
	groundPlane->append(getFieldLines());

	//wall
	VRS::SO<VRS::SceneThing> wall = new VRS::SceneThing(arenaScene);
	VRS::Matrix texMatrix = VRS::Matrix::scaling(VRS::Vector(4*groundRadius, 4.0, 4*groundRadius));
	//VRS::Matrix texMatrix = VRS::Matrix();
	wall->append(new VRS::FaceStyle(VRS::FaceStyle::Filled, VRS::FaceStyle::Filled));
	wall->append(new VRS::ImageTexture2DGL(VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "BricksLgMortar.jpg"),
		VRS::TextureGL::REPEAT, VRS::TextureGL::REPEAT, VRS::TextureGL::NEAREST, VRS::TextureGL::NEAREST, 1.0, 
		texMatrix));
	wall->append(new VRS::Cylinder(VRS::Vector(0.0, 8.0, 0.0), VRS::Vector(0.0, -0.5, 0.0), groundRadius, 360.0));
	
	//chainlink
	VRS::SO<VRS::SceneThing> fenceModelBase = new VRS::SceneThing();
	fenceModelBase->setLocalMatrix(VRS::Matrix::scaling(VRS::Vector(scalFenceBaseline, scalFenceHeight, scalFenceBaseline)));
	fenceModelBase->append(mOptimizer->get3dsModel(BV3D::MODELS_PATH + "chainlink_mrealms.3ds"));
	VRS::SO<VRS::CullingSceneThing> fenceSceneBase = new VRS::CullingSceneThing(fenceModelBase->boundingBox(mCanvas->engine()));
	fenceSceneBase->append(fenceModelBase);

	VRS::SO<VRS::SceneThing> fenceModelSide = new VRS::SceneThing();
	fenceModelSide->setLocalMatrix(VRS::Matrix::scaling(VRS::Vector(scalFenceSideline, scalFenceHeight, scalFenceSideline)));
	fenceModelSide->append(mOptimizer->get3dsModel(BV3D::MODELS_PATH + "chainlink_mrealms.3ds"));
	VRS::SO<VRS::CullingSceneThing> fenceSceneSide = new VRS::CullingSceneThing(fenceModelSide->boundingBox(mCanvas->engine()));
	fenceSceneSide->append(fenceModelSide);

	VRS::SO<VRS::SceneThing> fence1 = new VRS::SceneThing(arenaScene);
	
	VRS::SO<VRS::SceneThing> fence = new VRS::SceneThing(fence1);
	fence->setLocalMatrix(VRS::Matrix::translation(VRS::Vector(2*extent[0]/4, 0.0, extent[2])));
	fence->append(fenceSceneSide);

	fence = new VRS::SceneThing(fence1);
	fence->setLocalMatrix(VRS::Matrix::translation(VRS::Vector(-(2*extent[0]/4), 0.0, extent[2])));
	fence->append(fenceSceneSide);

	fence = new VRS::SceneThing(fence1);
	vrsMatrix = VRS::Matrix::translation(VRS::Vector(-(3*extent[0]/4), 0.0, extent[2]/2));
	vrsMatrix = vrsMatrix * VRS::Matrix::rotation(VRS::Vector(0.0, 1.0, 0.0), VRS::Vector(0.0, 0.0, 0.0), -90.0);
	fence->setLocalMatrix(vrsMatrix);
	fence->append(fenceSceneBase);

	fence = new VRS::SceneThing(fence1);
	vrsMatrix = VRS::Matrix::translation(VRS::Vector(-(3*extent[0]/4), 0.0, -(extent[2]/2)));
	vrsMatrix = vrsMatrix * VRS::Matrix::rotation(VRS::Vector(0.0, 1.0, 0.0), VRS::Vector(0.0, 0.0, 0.0), -90.0);
	fence->setLocalMatrix(vrsMatrix);
	fence->append(fenceSceneBase);

	VRS::SO<VRS::SceneThing> fence2 = new VRS::SceneThing(arenaScene);
	fence2->setLocalMatrix(VRS::Matrix::rotation(VRS::Vector(0.0, 1.0, 0.0), VRS::Vector(0.0, 0.0, 0.0), 180.0));
	fence2->append(fence1);

	fence = new VRS::SceneThing(arenaScene);
	fence->setLocalMatrix(VRS::Matrix::translation(VRS::Vector(0.0, 0.0, extent[2])));
	fence->append(fenceSceneSide);

	//swingset
	//VRS::SO<VRS::SceneThing> swingset = new VRS::SceneThing();
	//vrsMatrix = VRS::Matrix::translation(VRS::Vector(extent[0]-5.0, 2.0, extent[2] * 3));
	//vrsMatrix = vrsMatrix * VRS::Matrix::scaling(VRS::Vector(scalSwing, scalSwing, scalSwing));
	//swingset->setLocalMatrix(vrsMatrix);
	//swingset->append(mOptimizer->get3dsModel(BV3D::MODELS_PATH + "swingset_mrealms.3ds"));
	//VRS::SO<VRS::CullingSceneThing> swingsetScene = new VRS::CullingSceneThing(swingset->boundingBox(mCanvas->engine()));
	//swingsetScene->append(swingset);
	//arenaScene->append(swingsetScene);
	
	//metal stands
	VRS::SO<VRS::SceneThing> standsModel = new VRS::SceneThing();
	vrsMatrix = VRS::Matrix::scaling(VRS::Vector(scalStands, scalStands, scalStands));
	standsModel->setLocalMatrix(vrsMatrix);
	standsModel->append(new VRS::ShapeMaterialGL(VRS::Color(0.9, 0.9, 0.9)));
	standsModel->append(mOptimizer->get3dsModel(BV3D::MODELS_PATH + "metal_stands_mrealms.3ds", true, ModelOptimizer::NO_MATERIAL_NO_TEXTURES));
	VRS::SO<VRS::CullingSceneThing> standsScene = new VRS::CullingSceneThing(standsModel->boundingBox(mCanvas->engine()));
	standsScene->append(standsModel);

	VRS::SO<VRS::SceneThing> stands = new VRS::SceneThing(arenaScene);
	vrsMatrix = VRS::Matrix::translation(VRS::Vector(0, 2.5, 2*extent[2]));
	vrsMatrix = vrsMatrix * VRS::Matrix::rotation(VRS::Vector(0.0, 1.0, 0.0), VRS::Vector(0.0, 0.0, 0.0), 180.0);
	stands->setLocalMatrix(vrsMatrix);
	stands->append(standsScene);
	
	/*stands = new VRS::SceneThing(arenaScene);
	vrsMatrix = VRS::Matrix::translation(VRS::Vector(0, 2.5, -2*extent[2]));
	stands->setLocalMatrix(vrsMatrix);
	stands->append(standsScene);*/

	stands = new VRS::SceneThing(arenaScene);
	vrsMatrix = VRS::Matrix::translation(VRS::Vector(-5*extent[0]/4, 2.5, 0));
	vrsMatrix = vrsMatrix * VRS::Matrix::rotation(VRS::Vector(0.0, 1.0, 0.0), VRS::Vector(0.0, 0.0, 0.0), 90.0);
	stands->setLocalMatrix(vrsMatrix);
	stands->append(standsScene);

	stands = new VRS::SceneThing(arenaScene);
	vrsMatrix = VRS::Matrix::translation(VRS::Vector(5*extent[0]/4, 2.5, 0));
	vrsMatrix = vrsMatrix * VRS::Matrix::rotation(VRS::Vector(0.0, 1.0, 0.0), VRS::Vector(0.0, 0.0, 0.0), -90.0);
	stands->setLocalMatrix(vrsMatrix);
	stands->append(standsScene);

	return arenaScene;
}

/**
 * Returns a minimalistic scenerie of a closed carpet of clouds for the blobbs to play on.
 */
VRS::SO<VRS::SceneThing> BV3D::SceneLoader::loadHeaven()
{
	VRS::SO<VRS::SceneThing> heavenScene = new VRS::SceneThing();
	VRS::Vector extent = BV3D::ARENA_EXTENT;
	VRS::Matrix vrsMatrix;

	//sky
	VRS::SO<VRS::SceneThing> sky = new VRS::SceneThing(heavenScene);
	sky->append(new VRS::Sky());

	//ground plane with heaven bumpmap
	VRS::SO<VRS::SceneThing> groundPlane = new VRS::SceneThing(heavenScene);
	VRS::SO<VRS::DistantLight> light = new VRS::DistantLight(VRS::Vector(0.0, 1.0, 1.0), VRS::Color(1.0));
	VRS::SO<VRS::Image> img1 = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "orange.pgm");
	VRS::SO<VRS::Bumpmap> bumpmap = new VRS::Bumpmap(img1, light, -4.0);  
	groundPlane->append(new VRS::Translation(VRS::Vector(0.0, 0.0, -bumpmapOffset)));
    groundPlane->append(bumpmap);
	groundPlane->append(new VRS::ShapeMaterialGL(VRS::Color(0.6, 0.6, 0.6), VRS::Color(0.0), 128.0));
	groundPlane->append(new VRS::Disc(VRS::Vector(0.0, 0.0, 0.0), VRS::Vector(0.0, 1.0, 0.0), 60.0));
	groundPlane->append(new VRS::Translation(VRS::Vector(0.0, 0.0, bumpmapOffset)));
	groundPlane->append(getFieldLines());

	return heavenScene;
}

/**
 *	Low-Res(256x256) Skybox of Water with a few hills.
 */
VRS::SO<VRS::SceneThing> BV3D::SceneLoader::getWaterscapeSkybox()
{
	VRS::SO<VRS::Array<VRS::SO<VRS::Image> > > cubemapImages = new VRS::Array<VRS::SO<VRS::Image> >(6);
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Right] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "waterscape_cubemap/waterscape_posx.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Left] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "waterscape_cubemap/waterscape_negx.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Top] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "waterscape_cubemap/waterscape_posy.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Bottom] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "waterscape_cubemap/waterscape_negy.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Front] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "waterscape_cubemap/waterscape_posz.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Back] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "waterscape_cubemap/waterscape_negz.png");
	VRS::SO<VRS::ImageCubeMapTextureGL> cubemap = new VRS::ImageCubeMapTextureGL(cubemapImages->newIterator());
	VRS::SO<VRS::BackgroundGL> background = new VRS::BackgroundGL(cubemap);
	VRS::SO<VRS::SceneThing> scene = new VRS::SceneThing();
	scene->append(background);
	return scene;
}

/**
 *	768x768 skybox of nice water, rising sund and hills
 */
VRS::SO<VRS::SceneThing> BV3D::SceneLoader::getIslandsSkybox()
{
	VRS::SO<VRS::Array<VRS::SO<VRS::Image> > > cubemapImages = new VRS::Array<VRS::SO<VRS::Image> >(6);
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Right] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "islands/rt.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Left] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "islands/lt.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Top] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "islands/up.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Bottom] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "islands/dn.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Front] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "islands/ft.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Back] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "islands/bk.jpg");
	VRS::SO<VRS::ImageCubeMapTextureGL> cubemap = new VRS::ImageCubeMapTextureGL(cubemapImages->newIterator());
	VRS::SO<VRS::BackgroundGL> background = new VRS::BackgroundGL(cubemap);
	VRS::SO<VRS::SceneThing> scene = new VRS::SceneThing();
	scene->append(background);
	return scene;
}

/**
 *	Low-Res(256x256) skybox of desert with sun
 */
VRS::SO<VRS::SceneThing> BV3D::SceneLoader::getHotDesertSkybox()
{
	VRS::SO<VRS::Array<VRS::SO<VRS::Image> > > cubemapImages = new VRS::Array<VRS::SO<VRS::Image> >(6);
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Right] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "hotdesert_cubemap/hotdesert_posx.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Left] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "hotdesert_cubemap/hotdesert_negx.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Top] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "hotdesert_cubemap/hotdesert_posy.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Bottom] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "hotdesert_cubemap/hotdesert_negy.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Front] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "hotdesert_cubemap/hotdesert_posz.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Back] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "hotdesert_cubemap/hotdesert_negz.png");
	VRS::SO<VRS::ImageCubeMapTextureGL> cubemap = new VRS::ImageCubeMapTextureGL(cubemapImages->newIterator());
	VRS::SO<VRS::BackgroundGL> background = new VRS::BackgroundGL(cubemap);
	VRS::SO<VRS::SceneThing> scene = new VRS::SceneThing();
	scene->append(background);
	return scene;
}

/**
 *	512x512 skybox of prairie with blue sky and clouds
 */
VRS::SO<VRS::SceneThing> BV3D::SceneLoader::getBrightDaySkybox()
{
	VRS::SO<VRS::Array<VRS::SO<VRS::Image> > > cubemapImages = new VRS::Array<VRS::SO<VRS::Image> >(6);
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Right] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "brightday_cubemap/brightday1_posx.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Left] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "brightday_cubemap/brightday1_negx.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Top] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "brightday_cubemap/brightday1_posy.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Bottom] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "brightday_cubemap/brightday1_negy.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Front] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "brightday_cubemap/brightday1_posz.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Back] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "brightday_cubemap/brightday1_negz.png");
	VRS::SO<VRS::ImageCubeMapTextureGL> cubemap = new VRS::ImageCubeMapTextureGL(cubemapImages->newIterator());
	VRS::SO<VRS::BackgroundGL> background = new VRS::BackgroundGL(cubemap);
	VRS::SO<VRS::SceneThing> scene = new VRS::SceneThing();
	scene->append(background);
	return scene;
}

/**
 *	512x512 skybox of white desert
 */
VRS::SO<VRS::SceneThing> BV3D::SceneLoader::getZolsky6Skybox()
{
	VRS::SO<VRS::Array<VRS::SO<VRS::Image> > > cubemapImages = new VRS::Array<VRS::SO<VRS::Image> >(6);
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Right] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "zolsky6/zolsky06_r.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Left] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "zolsky6/zolsky06_l.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Top] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "zolsky6/zolsky06_up.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Bottom] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "zolsky6/zolsky06_dn.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Front] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "zolsky6/zolsky06_fr.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Back] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "zolsky6/zolsky06_b.jpg");
	VRS::SO<VRS::ImageCubeMapTextureGL> cubemap = new VRS::ImageCubeMapTextureGL(cubemapImages->newIterator());
	VRS::SO<VRS::BackgroundGL> background = new VRS::BackgroundGL(cubemap);
	VRS::SO<VRS::SceneThing> scene = new VRS::SceneThing();
	scene->append(background);
	return scene;
}

/**
 *	512x512 skybox of setting sun on water
 */
VRS::SO<VRS::SceneThing> BV3D::SceneLoader::getZolsky8Skybox()
{
	VRS::SO<VRS::Array<VRS::SO<VRS::Image> > > cubemapImages = new VRS::Array<VRS::SO<VRS::Image> >(6);
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Right] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "zolsky8/zolsky08_rt.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Left] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "zolsky8/zolsky08_lf.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Top] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "zolsky8/zolsky08_up.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Bottom] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "zolsky8/zolsky08_dn.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Front] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "zolsky8/zolsky08_fr.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Back] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "zolsky8/zolsky08_bk.jpg");
	VRS::SO<VRS::ImageCubeMapTextureGL> cubemap = new VRS::ImageCubeMapTextureGL(cubemapImages->newIterator());
	VRS::SO<VRS::BackgroundGL> background = new VRS::BackgroundGL(cubemap);
	VRS::SO<VRS::SceneThing> scene = new VRS::SceneThing();
	scene->append(background);
	return scene;
}

/**
 *	512x512 skybox of city
 */
VRS::SO<VRS::SceneThing> BV3D::SceneLoader::getZolsky9Skybox()
{
	VRS::SO<VRS::Array<VRS::SO<VRS::Image> > > cubemapImages = new VRS::Array<VRS::SO<VRS::Image> >(6);
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Right] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "zolsky9/zolsky09_rt.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Left] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "zolsky9/zolsky09_lf.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Top] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "zolsky9/zolsky09_up.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Bottom] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "zolsky9/zolsky09_dn.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Front] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "zolsky9/zolsky09_fr.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Back] = VRS_GuardedLoadObject(VRS::Image, BV3D::GRAFICS_PATH + "zolsky9/zolsky09_bk.jpg");
	VRS::SO<VRS::ImageCubeMapTextureGL> cubemap = new VRS::ImageCubeMapTextureGL(cubemapImages->newIterator());
	VRS::SO<VRS::BackgroundGL> background = new VRS::BackgroundGL(cubemap);
	VRS::SO<VRS::SceneThing> scene = new VRS::SceneThing();
	scene->append(background);
	return scene;
}