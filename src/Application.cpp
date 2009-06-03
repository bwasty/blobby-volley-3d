#include "Application.h"
#include "Blobb.h"
#include "Controls.h"

#include "Constants.h"
#include <OgrePrerequisites.h>
#include <OgreViewport.h>
#include <OgreLight.h>
#include <OgreRoot.h>
#include <OgreEntity.h>
#include <OgreRenderWindow.h>
#include <OgreMeshManager.h>

//TODO!: project settings -> NXOGRE_DIR -> local url to Folder....
Application::Application() : mConfig() {
	const String pathToConfig = "../BV3D.cfg"; // TODO: working directory-independent solution?
	mConfig.load(pathToConfig, "=\t:", true);

	/* dump of current default config
	ARENA_EXTENT=16.0 30.0 7.0
	BALL_RADIUS=0.9;
	LOOK_FROM=0.0 9.0 -16.0
	LOOK_TO=0.0 4.0 0.0
	NET_HEIGHT=4.2;
	*/

}


void Application::createFrameListener() {
	mListener = new ControlsListener(mRoot->getAutoCreatedWindow(), mCamera, mSceneMgr, this);
    mRoot->addFrameListener(mListener);
}

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
	mCamera->setPosition(mConfig.getSettingVector3("LOOK_FROM"));
	mCamera->lookAt(mConfig.getSettingVector3("LOOK_TO"));

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
	light->setDirection(-Vector3::UNIT_Y+Vector3::UNIT_X*0.2);
	light->setDiffuseColour(1.0, 1.0, 1.0);
    light->setSpecularColour(1.0, 1.0, 1.0);

	// blobbs
	Vector3 arenaExtent = mConfig.getSettingVector3("ARENA_EXTENT");
	mBlobb1 = new Blobb(mSceneMgr, mNxScene, Vector3(-arenaExtent[0]/2,1.0,0.0), BV3D::TEAM1);
	mBlobb2 = new Blobb(mSceneMgr, mNxScene, Vector3(arenaExtent[0]/2,1.0,0.0), BV3D::TEAM2, ColourValue::Green);

	// create ball
	Real ballRadius = mConfig.getSettingReal("BALL_RADIUS");
	ent = mSceneMgr->createEntity("Ball", "Ball.mesh");
    SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode("BallNode");
	node->scale(Vector3(ballRadius / 1.7));
	Vector3 position = Vector3(-arenaExtent[0]/2+0.4,6.0,0.0);
	node->translate(position);
	node->attachObject(ent);

	NxOgre::NodeRenderableParams nrp;
	nrp.setToDefault();
	nrp.mIdentifierUsage = NxOgre::NodeRenderableParams::IU_Use;
	nrp.mIdentifier = node->getName();

	NxOgre::Material* ballMaterial = mNxScene->createMaterial("ball_material");
	ballMaterial->setRestitution(1.3);
	NxOgre::ShapeParams sp;
	sp.setToDefault();
	sp.mMaterial = ballMaterial->getMaterialIndex();

	mBallActor = mNxScene->createBody<NxOgre::Body>("ball_body", new NxOgre::Sphere(ballRadius, /*sp*/"material: ball_material"), position, nrp, "mass:1");
	mBallActor->putToSleep();

	// create net
	/* net1.3ds
		width: 12.0		height: 2.1		depth: 0.14
	*/
	Real netHeight = mConfig.getSettingReal("NET_HEIGHT");
	ent = mSceneMgr->createEntity("Net_a", "Net2a.mesh"); //TODO: fix netz-befestigungen in Blender (some triangles face to the wrong side)
	Entity *ent2 = mSceneMgr->createEntity("Net_b", "Net2b.mesh");
    SceneNode* netNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	//node->scale(Vector3(BV3D::BALL_RADIUS / 1.7));
	//node->translate(Vector3(-BV3D::ARENA_EXTENT[0]/2,6.0,0.0));
	netNode->rotate(Vector3::UNIT_Y, Radian(Degree(90)));
	netNode->translate(Vector3(0, netHeight, 6)); //TODO: something wrong with z translation from constants -> scaling(VRS::Vector((BV3D::ARENA_EXTENT[2] + (2*poleOffset))/width3ds, 1.0, 1.0));
	netNode->attachObject(ent);
	netNode->attachObject(ent2);

	// try prefab shapes for flag poles
	ent = mSceneMgr->createEntity("net pole 1", SceneManager::PT_CUBE);
	ent2 = mSceneMgr->createEntity("net pole 2", SceneManager::PT_CUBE);
	SceneNode* scaleNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	SceneNode* poleNode1 = scaleNode->createChildSceneNode();
	poleNode1->attachObject(ent);
	SceneNode* poleNode2 = scaleNode->createChildSceneNode();
	poleNode2->attachObject(ent2);

	//scaleNode->attachObject(ent);
	scaleNode->scale(Vector3(0.002));
	scaleNode->scale(1, 44, 1);
	scaleNode->translate(0.0, 2.0, 0); // TODO: look in Constants.h...

	poleNode1->translate(0,0,6, SceneNode::TS_WORLD); //TODO: why TS_WORLD??
	poleNode2->translate(0,0,-6, SceneNode::TS_WORLD);


	// TODO!!: create physical net and "cage"
	netNode->_updateBounds();
	AxisAlignedBox bb = netNode->_getWorldAABB();
	Vector3 physNetSize = bb.getSize();
	physNetSize.y = netHeight + 2.1; //2.1 is half? the height of the net model
	NxOgre::ActorParams ap;
	ap.setToDefault();
	ap.mMass = 0.0;
	mNxScene->createActor("netShape", new NxOgre::Cube(physNetSize), NxOgre::Pose(Vector3(0.0, (netHeight+2.1)/2, 0.0)), ap);
	
	//TODO!!!: problem cube is solid, create cage-walls separately...
	//mNxScene->createActor("cageShape", new NxOgre::Cube(BV3D::ARENA_EXTENT), NxOgre::Pose(Vector3(0.0, (netHeight+2.1)/2, 0.0)), ap);


}