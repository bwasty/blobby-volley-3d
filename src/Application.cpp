#include "Application.h"

#include "Constants.h"

void Application::setupScene() {
	mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "Default SceneManager");
	setupPhysics(); // TODO: create scenemgr before and move to BaseApplicatin::go() !?!
	mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

    mCamera = mSceneMgr->createCamera("Camera");
	mCamera->setNearClipDistance(1.0);

    Viewport *vp = mRoot->getAutoCreatedWindow()->addViewport(mCamera);
	// Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));

	vp->setBackgroundColour(ColourValue(0.1,0.1,0.1));

	// camera position and orientation
	mCamera->setPosition(BV3D::LOOK_FROM);
	mCamera->lookAt(BV3D::LOOK_TO);

	// floor plane
	Plane plane(Vector3::UNIT_Y, 0);
	MeshManager::getSingleton().createPlane("ground",
       ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
       500,500,20,20,true,1,5,5,Vector3::UNIT_Z);

	Entity* ent = mSceneMgr->createEntity("GroundEntity", "ground");
    mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
	ent->setMaterialName("sand_yellow");
    ent->setCastShadows(false);

	// Sky 
	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 4, 8, 5000, true);
	
	// light
	mSceneMgr->setAmbientLight( ColourValue( 0.4, 0.4, 0.4 ) );

	Light *light = mSceneMgr->createLight("Light1");
    light->setType(Light::LT_DIRECTIONAL);
    //light->setPosition(Vector3(-BV3D::ARENA_EXTENT[0]/2, BV3D::ARENA_EXTENT[1]/2, -BV3D::ARENA_EXTENT[2]/2));
	light->setDirection(-Vector3::UNIT_Y+Vector3::UNIT_X);
	light->setDiffuseColour(1.0, 1.0, 1.0);
    light->setSpecularColour(1.0, 1.0, 1.0);

	//light = mSceneMgr->createLight("Light2");
 //   light->setType(Light::LT_POINT);
 //   light->setPosition(Vector3(BV3D::ARENA_EXTENT[0]/2, BV3D::ARENA_EXTENT[1]/4, -BV3D::ARENA_EXTENT[2]/2));
	//light->setDiffuseColour(1.0, 1.0, 1.0);
 //   light->setSpecularColour(1.0, 1.0, 1.0);

	//TODO: blobbs, ball, net
	ent = mSceneMgr->createEntity("Blobb1", "Blobb.mesh");
    SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	Vector3 blobb1_position = Vector3(-BV3D::ARENA_EXTENT[0]/2,1.0,0.0);
	node->translate(blobb1_position);
	node->attachObject(ent);

	NxOgre::NodeRenderableParams nrp;
	nrp.setToDefault();
	nrp.mIdentifierUsage = NxOgre::NodeRenderableParams::IU_Use;
	nrp.mIdentifier = node->getName();

	//NxOgre::Material* ballMaterial = mNxScene->createMaterial("ball_material");
	//ballMaterial->setRestitution(1.1);
	//NxOgre::ShapeParams sp;
	//sp.setToDefault();
	//sp.mMaterial = ballMaterial->getMaterialIndex();

	NxOgre::Resources::ResourceSystem::getSingleton()->addMeshAs("file://../../media/blobb.nxs", "blobb.nxs"); //TODO: workaround, -> ogre resource system??
	mNxScene->createBody<NxOgre::Body>("blobb1", /*new NxOgre::TriangleMesh("blobb.nxs")*/
		new NxOgre::TriangleMesh(NxOgre::Resources::ResourceSystem::getSingleton()->getMesh("blobb.nxs")), 
		blobb1_position, nrp, "mass:10");


	ent = mSceneMgr->createEntity("Blobb2", "Blobb.mesh");
	ent->setMaterialName("blobb_green");
    node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	node->translate(Vector3(BV3D::ARENA_EXTENT[0]/2,1.0,0.0));
	node->attachObject(ent);


	ent = mSceneMgr->createEntity("Ball", "Ball.mesh"); //TODO!: re-export ball centered
    node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	node->scale(Vector3(BV3D::BALL_RADIUS / 1.7));
	Vector3 position = Vector3(-BV3D::ARENA_EXTENT[0]/2,6.0,0.0);
	node->translate(position);
	node->attachObject(ent);

	//NxOgre::NodeRenderableParams nrp;
	nrp.setToDefault();
	nrp.mIdentifierUsage = NxOgre::NodeRenderableParams::IU_Use;
	nrp.mIdentifier = node->getName();

	NxOgre::Material* ballMaterial = mNxScene->createMaterial("ball_material");
	ballMaterial->setRestitution(1.1);
	NxOgre::ShapeParams sp;
	sp.setToDefault();
	sp.mMaterial = ballMaterial->getMaterialIndex();

	NxOgre::Actor *actor = mNxScene->createBody<NxOgre::Body>("ball_body", new NxOgre::Sphere(BV3D::BALL_RADIUS, /*sp*/"material: ball_material"), position, nrp, "mass:1");


	ent = mSceneMgr->createEntity("Net_a", "Net2a.mesh");
	Entity *ent2 = mSceneMgr->createEntity("Net_b", "Net2b.mesh");
    node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	//node->scale(Vector3(BV3D::BALL_RADIUS / 1.7));
	//node->translate(Vector3(-BV3D::ARENA_EXTENT[0]/2,6.0,0.0));
	node->rotate(Vector3::UNIT_Y, Radian(Degree(90)));
	node->translate(Vector3(0,BV3D::NET_HEIGHT,BV3D::ARENA_EXTENT[2]));
	node->attachObject(ent);
	node->attachObject(ent2);


	//TODO:!! little NxOgre experiments
	//NxOgre::Actor* actor = mNxScene->createActor("box", new NxOgre::Cube(1), Vector3(0,5,0), "mass:10");


}