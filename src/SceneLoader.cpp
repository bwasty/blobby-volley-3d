#include "SceneLoader.h"
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
	VRS::Vector extent = BV3D::arenaExtent;
	double scalYucca2, scalPalm;
	scalYucca2 = scalPalm = 0.02;
	VRS::Matrix vrsMatrix, scalMatrix;
	VRS::ThreeDSReader::setMaterialMode(VRS::ThreeDSReader::ALL_BUT_CULLING_MATERIAL);

	
	//sky
	//VRS::SO<VRS::SceneThing> sky = new VRS::SceneThing();
	//sky->setLocalMatrix(VRS::Matrix::translation(VRS::Vector(0.0, -100.0, 0.0)));
	//sky->append(new VRS::Sky(false, VRS::Color(0.3, 0.3, 0.9), VRS::Color(1.0, 1.0, 1.0), 0.005, false, VRS::Vector(0.0, 1.0, 0.0), 3.0));
	//beachScene->append(sky);

	//fog
	/*VRS::SO<VRS::Fog> fog = new VRS::Fog(0.1, VRS::Color(0.7, 0.7, 0.7), VRS::Fog::Linear, 0., 0.2);
	fog->setDensity(0.2);
	beachScene->append(fog);*/

	//background

	VRS::SO<VRS::SceneThing> background = new VRS::SceneThing(beachScene);
	background->append(loadBrightDaySkybox());

	//ground plane with sand bumpmap
	VRS::SO<VRS::SceneThing> groundPlane = new VRS::SceneThing();

	VRS::SO<VRS::DistantLight> light = new VRS::DistantLight(VRS::Vector(0.0, 1.0, 1.0), VRS::Color(0.5));
	VRS::SO<VRS::Image> img1 = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "orange.pgm");
	VRS::SO<VRS::Bumpmap> bumpmap = new VRS::Bumpmap(img1, light, 1.0);    
    groundPlane->append(bumpmap);
	groundPlane->append(new VRS::ShapeMaterialGL(VRS::Color(0.5, 0.4, 0.1), VRS::Color(0.0), 128.0));
	groundPlane->append(new VRS::Disc(VRS::Vector(0.0, 0.0, 0.0), VRS::Vector(0.0, 1.0, 0.0), 60.0));
	beachScene->append(groundPlane);



	//palm trees
	VRS::SO<VRS::SceneThing> palmTrees = new VRS::SceneThing();
	VRS::ThreeDSReader::setMaterialMode(VRS::ThreeDSReader::ALL_BUT_CULLING_MATERIAL);	
	VRS::ThreeDSReader::setOptimizations(VRS::ThreeDSReader::EXCLUSIVE_OPENGL);
	vrsMatrix = VRS::Matrix::translation(VRS::Vector(0.0, 0.0, extent[2] + 10));
	vrsMatrix = vrsMatrix * VRS::Matrix::scaling(VRS::Vector(scalPalm, scalPalm, scalPalm));
	vrsMatrix = vrsMatrix * VRS::Matrix::rotation(VRS::Vector(0.0, 1.0, 0.0), VRS::Vector(0.0, 0.0, 0.0), 140.0);
	palmTrees->setLocalMatrix(vrsMatrix);
	palmTrees->append(VRS::ThreeDSReader::readScene(BV3D::threeDSPath + "palmtrees1_mrealms.3ds", "", false, false));
	beachScene->append(palmTrees);

	//yucca plants
	VRS::SO<VRS::SceneThing> yucca = new VRS::SceneThing();
	vrsMatrix = VRS::Matrix::translation(VRS::Vector(extent[0] / 2, 0.0, (extent[2] + 4)));
	vrsMatrix = vrsMatrix * VRS::Matrix::scaling(VRS::Vector(scalYucca2, scalYucca2, scalYucca2));
	yucca->setLocalMatrix(vrsMatrix);
	yucca->append(VRS::ThreeDSReader::readScene(BV3D::threeDSPath + "yucca_mrealms.3ds", "", false, false));
	beachScene->append(yucca);

	//yucca 2
	VRS::SO<VRS::SceneThing> yucca2 = new VRS::SceneThing();
	vrsMatrix = VRS::Matrix::translation(VRS::Vector(0.0, 0.0, -(extent[2] + 4)));
	vrsMatrix = vrsMatrix * VRS::Matrix::scaling(VRS::Vector(scalYucca2, scalYucca2, scalYucca2));
	yucca2->setLocalMatrix(vrsMatrix);
	yucca2->append(VRS::ThreeDSReader::readScene(BV3D::threeDSPath + "yucca2_mrealms.3ds", "", false, false));
	beachScene->append(yucca2);



	//garlic plant
	VRS::SO<VRS::SceneThing> garlic1 = new VRS::SceneThing();
	vrsMatrix = VRS::Matrix::translation(VRS::Vector(20.0, 0.0, 0.0));
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

/*
 *	WARNING!!! Very high resolution, use only with high-end PC!
 */
VRS::SO<VRS::SceneThing> BV3D::SceneLoader::loadDesertSkybox()
{
	printf("Loading Desert Background...\n");
	VRS::SO<VRS::Array<VRS::SO<VRS::Image> > > cubemapImages = new VRS::Array<VRS::SO<VRS::Image> >(6);
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Right] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "desert/h3desert_rt.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Left] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "desert/h3desert_lt.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Top] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "desert/h3desert_up.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Bottom] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "desert/h3desert_dn.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Front] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "desert/h3desert_ft.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Back] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "desert/h3desert_bk.jpg");
	VRS::SO<VRS::ImageCubeMapTextureGL> cubemap = new VRS::ImageCubeMapTextureGL(cubemapImages->newIterator());
	VRS::SO<VRS::BackgroundGL> background = new VRS::BackgroundGL(cubemap);
	VRS::SO<VRS::SceneThing> scene = new VRS::SceneThing();
	scene->append(background);
	return scene;
}

/*
 *	Low-Res Skybox of Water with a few hills.
 */
VRS::SO<VRS::SceneThing> BV3D::SceneLoader::loadWaterscapeSkybox()
{
	printf("Loading Waterscape Background...\n");
	VRS::SO<VRS::Array<VRS::SO<VRS::Image> > > cubemapImages = new VRS::Array<VRS::SO<VRS::Image> >(6);
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Right] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "waterscape_cubemap/waterscape_posx.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Left] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "waterscape_cubemap/waterscape_negx.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Top] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "waterscape_cubemap/waterscape_posy.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Bottom] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "waterscape_cubemap/waterscape_negy.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Front] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "waterscape_cubemap/waterscape_posz.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Back] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "waterscape_cubemap/waterscape_negz.png");
	VRS::SO<VRS::ImageCubeMapTextureGL> cubemap = new VRS::ImageCubeMapTextureGL(cubemapImages->newIterator());
	VRS::SO<VRS::BackgroundGL> background = new VRS::BackgroundGL(cubemap);
	VRS::SO<VRS::SceneThing> scene = new VRS::SceneThing();
	scene->append(background);
	return scene;
}

/*
 *
 */
VRS::SO<VRS::SceneThing> BV3D::SceneLoader::loadIslandsSkybox()
{
	printf("Loading Islands Background...\n");
	VRS::SO<VRS::Array<VRS::SO<VRS::Image> > > cubemapImages = new VRS::Array<VRS::SO<VRS::Image> >(6);
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Right] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "islands/rt.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Left] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "islands/lt.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Top] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "islands/up.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Bottom] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "islands/dn.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Front] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "islands/ft.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Back] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "islands/bk.jpg");
	VRS::SO<VRS::ImageCubeMapTextureGL> cubemap = new VRS::ImageCubeMapTextureGL(cubemapImages->newIterator());
	VRS::SO<VRS::BackgroundGL> background = new VRS::BackgroundGL(cubemap);
	VRS::SO<VRS::SceneThing> scene = new VRS::SceneThing();
	scene->append(background);
	return scene;
}

/*
 *
 */
VRS::SO<VRS::SceneThing> BV3D::SceneLoader::loadHotDesertSkybox()
{
	printf("Loading Hot Desert Background...\n");
	VRS::SO<VRS::Array<VRS::SO<VRS::Image> > > cubemapImages = new VRS::Array<VRS::SO<VRS::Image> >(6);
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Right] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "hotdesert_cubemap/hotdesert_posx.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Left] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "hotdesert_cubemap/hotdesert_negx.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Top] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "hotdesert_cubemap/hotdesert_posy.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Bottom] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "hotdesert_cubemap/hotdesert_negy.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Front] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "hotdesert_cubemap/hotdesert_posz.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Back] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "hotdesert_cubemap/hotdesert_negz.png");
	VRS::SO<VRS::ImageCubeMapTextureGL> cubemap = new VRS::ImageCubeMapTextureGL(cubemapImages->newIterator());
	VRS::SO<VRS::BackgroundGL> background = new VRS::BackgroundGL(cubemap);
	VRS::SO<VRS::SceneThing> scene = new VRS::SceneThing();
	scene->append(background);
	return scene;
}

/*
 *
 */
VRS::SO<VRS::SceneThing> BV3D::SceneLoader::loadBrightDaySkybox()
{
	printf("Loading Bright Day Background...\n");
	VRS::SO<VRS::Array<VRS::SO<VRS::Image> > > cubemapImages = new VRS::Array<VRS::SO<VRS::Image> >(6);
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Right] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "brightday_cubemap/brightday1_posx.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Left] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "brightday_cubemap/brightday1_negx.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Top] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "brightday_cubemap/brightday1_posy.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Bottom] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "brightday_cubemap/brightday1_negy.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Front] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "brightday_cubemap/brightday1_posz.png");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Back] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "brightday_cubemap/brightday1_negz.png");
	VRS::SO<VRS::ImageCubeMapTextureGL> cubemap = new VRS::ImageCubeMapTextureGL(cubemapImages->newIterator());
	VRS::SO<VRS::BackgroundGL> background = new VRS::BackgroundGL(cubemap);
	VRS::SO<VRS::SceneThing> scene = new VRS::SceneThing();
	scene->append(background);
	return scene;
}

/*
 *
 */
VRS::SO<VRS::SceneThing> BV3D::SceneLoader::loadZolsky6Skybox()
{
	printf("Loading Islands Background...\n");
	VRS::SO<VRS::Array<VRS::SO<VRS::Image> > > cubemapImages = new VRS::Array<VRS::SO<VRS::Image> >(6);
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Right] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "zolsky6/zolsky06_r.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Left] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "zolsky6/zolsky06_l.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Top] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "zolsky6/zolsky06_up.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Bottom] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "zolsky6/zolsky06_dn.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Front] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "zolsky6/zolsky06_fr.jpg");
	(*cubemapImages)[VRS::ImageCubeMapTextureGL::Back] = VRS_GuardedLoadObject(VRS::Image, BV3D::cubemapsPath + "zolsky6/zolsky06_b.jpg");
	VRS::SO<VRS::ImageCubeMapTextureGL> cubemap = new VRS::ImageCubeMapTextureGL(cubemapImages->newIterator());
	VRS::SO<VRS::BackgroundGL> background = new VRS::BackgroundGL(cubemap);
	VRS::SO<VRS::SceneThing> scene = new VRS::SceneThing();
	scene->append(background);
	return scene;
}