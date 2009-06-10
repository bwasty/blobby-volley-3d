#include "Constants.h"
#include <OgrePrerequisites.h>
#include <OgreViewport.h>
#include <OgreLight.h>
#include <OgreRoot.h>
#include <OgreEntity.h>
#include <OgreRenderWindow.h>
#include <OgreMeshManager.h>

#include <NxOgre.h>
#include <OGRE3DRenderSystem.h>
#include "OGRE3DBody.h"

#include "Blobb.h"
#include "Ball.h"
#include "Controls.h"
#include "Application.h"


using namespace Ogre;


void Application::createFrameListener() {
	mListener = new ControlsListener(mRoot->getAutoCreatedWindow(), mCamera, mSceneMgr, this);
    mRoot->addFrameListener(mListener);
}


void Application::fillScene()
{
	// floor plane
	Plane plane(Vector3::UNIT_Y, 0);
	MeshManager::getSingleton().createPlane("ground",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
		500,500,20,20,true,1,5,5,Vector3::UNIT_Z);

	Entity* ent = mSceneMgr->createEntity("GroundEntity", "ground");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
	ent->setMaterialName("sand_yellow");
	ent->setCastShadows(false);

	// physical ground plane
	mPhysicsScene->createSceneGeometry(new NxOgre::PlaneGeometry(0, NxOgre::Real3(0, 1, 0)));

	// Sky 
	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 4, 8, 5000, true);

	// light
	mSceneMgr->setAmbientLight( ColourValue( 0.4, 0.4, 0.4 ) );

	Light *light = mSceneMgr->createLight("Light1");
	light->setType(Light::LT_DIRECTIONAL);
	light->setDirection(-Vector3::UNIT_Y+Vector3::UNIT_X*0.2);
	light->setDiffuseColour(1.0, 1.0, 1.0);
	light->setSpecularColour(1.0, 1.0, 1.0);

	// blobbs
	Vector3 arenaExtent = mConfig.getSettingVector3("ARENA_EXTENT");
	mBlobb1 = new Blobb(this, mSceneMgr, mPhysicsScene, Vector3(-arenaExtent[0]/4,1.0,0.0), BV3D::TEAM1);
	mBlobb2 = new Blobb(this, mSceneMgr, mPhysicsScene, Vector3(arenaExtent[0]/4,1.0,0.0), BV3D::TEAM2, ColourValue::Green);

	////TODO!!: refactor Ball into own class
	//// create ball
	//Real ballRadius = mConfig.getSettingReal("BALL_RADIUS");

	//NxOgre::Sphere* ballSphere = new NxOgre::Sphere(ballRadius);
	////TODO!!!:PhysX material creation seems to fail
	////NxOgre::Material* ball_mat = mPhysicsScene->createMaterial();
	////ball_mat->setRestitution(1.3);
	////ballSphere->setMaterial(ball_mat->getIdentifier()); //TODO!!!: doesn't work, null pointer, NxOgre bug?
	//Vector3 position = Vector3(-arenaExtent[0]/4,6.0,0.0);
	//mBallBody = mPhysicsRenderSystem->createBody(ballSphere, NxOgre::Real3(position.x, position.y, position.z), "Ball.mesh");
	//mBallBody->setMass(1);
	//mBallBody->getSceneNode()->scale(Vector3(ballRadius / 1.7));
	//mBallBody->putToSleep();
	mBall = new Ball(this, Vector3(-arenaExtent[0]/4,6.0,0.0));


	//old Bleeding code
	//ent = mSceneMgr->createEntity("Ball", "Ball.mesh");
	//   SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode("BallNode");
	//node->scale(Vector3(ballRadius / 1.7));
	////Vector3 position = Vector3(-arenaExtent[0]/2+0.4,6.0,0.0);
	//node->translate(position);
	//node->attachObject(ent);

	//NxOgre::NodeRenderableParams nrp;
	//nrp.setToDefault();
	//nrp.mIdentifierUsage = NxOgre::NodeRenderableParams::IU_Use;
	//nrp.mIdentifier = node->getName();

	//NxOgre::Material* ballMaterial = mPhysicsScene->createMaterial("ball_material");
	//ballMaterial->setRestitution(1.3);
	//NxOgre::ShapeParams sp;
	//sp.setToDefault();
	//sp.mMaterial = ballMaterial->getMaterialIndex();

	//mBallActor = mPhysicsScene->createBody<NxOgre::Body>("ball_body", new NxOgre::Sphere(ballRadius, /*sp*/"material: ball_material"), position, nrp, "mass:1");
	//mBallActor->putToSleep();

	// create net
	/* net1.3ds
	width: 12.0		height: 2.1		depth: 0.14
	*/
	Real netHeight = mConfig.getSettingReal("NET_HEIGHT");
	ent = mSceneMgr->createEntity("Net_a", "Net2a.mesh"); //TODO!: fix netz-befestigungen in Blender (some triangles face to the wrong side)
	Entity *ent2 = mSceneMgr->createEntity("Net_b", "Net2b.mesh");
	SceneNode* netNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	//node->scale(Vector3(BV3D::BALL_RADIUS / 1.7));
	//node->translate(Vector3(-BV3D::ARENA_EXTENT[0]/2,6.0,0.0));
	netNode->rotate(Vector3::UNIT_Y, Radian(Degree(90)));
	netNode->translate(Vector3(0, netHeight, 6)); //TODO: something wrong with z translation from constants -> scaling(VRS::Vector((BV3D::ARENA_EXTENT[2] + (2*poleOffset))/width3ds, 1.0, 1.0));
	netNode->attachObject(ent);
	netNode->attachObject(ent2);

	// use prefab shapes for flag poles
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
	scaleNode->translate(0.0, 2.0, 0); // TODO!!: make configurable - net size/position

	poleNode1->translate(0,0,6, SceneNode::TS_WORLD); //TODO: why TS_WORLD??
	poleNode2->translate(0,0,-6, SceneNode::TS_WORLD);


	// compute size for physical net
	netNode->_updateBounds();
	AxisAlignedBox bb = netNode->_getWorldAABB();
	//TODO!: proper netsize -> free space below net
	Vector3 physNetSize = bb.getSize();
	physNetSize.y = netHeight + 2.1; //2.1 is half? the height of the net model

	Vector3 p(0.0, (netHeight+2.1)/2, 0.0);
	mPhysicsScene->createSceneGeometry(new NxOgre::Box(physNetSize.x, physNetSize.y, physNetSize.z), NxOgre::Matrix44(NxOgre::Real3(p.x, p.y, p.z))); 


	// create physical "cage" (walls)
	//TODO!: visual representation of Walls (transparent/shadows/splatting)
	NxOgre::Shapes wallPlanes;
	wallPlanes.insert(new NxOgre::PlaneGeometry(-arenaExtent.x/2, NxOgre::Real3(1,0,0)));
	wallPlanes.insert(new NxOgre::PlaneGeometry(-arenaExtent.x/2, NxOgre::Real3(-1,0,0)));
	wallPlanes.insert(new NxOgre::PlaneGeometry(-arenaExtent.z/2, NxOgre::Real3(0,0,1)));
	wallPlanes.insert(new NxOgre::PlaneGeometry(-arenaExtent.z/2, NxOgre::Real3(0,0,-1)));

	mPhysicsScene->createSceneGeometry(wallPlanes[0]); //TODO!!!: fix Shapes thing
	mPhysicsScene->createSceneGeometry(wallPlanes[1]);
	mPhysicsScene->createSceneGeometry(wallPlanes[2]);
	mPhysicsScene->createSceneGeometry(wallPlanes[3]);
}