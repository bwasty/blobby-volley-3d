// TODO!!!: Game logic -> contact callbacks (-> file CollisionCallback(s).h/cpp or Application.cpp or GameLogic with class(es) derived from NxOgre::Callback overriding onContact (const ContactPair &), display labels or so

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

#include <MyGUI.h>

#include "Blobb.h"
#include "Ball.h"
#include "Controls.h"
#include "GUI.h"
#include "Application.h"
#include "Console.h"
#include "GameLogic.h"


using namespace Ogre;

void Application::setupInputSystem() {
	BaseApplication::setupInputSystem();
}

void Application::createFrameListener() {
	mRoot->addFrameListener(this);

	mControls = new ControlsListener(this, mCamera, mSceneMgr, mKeyboard, mMouse, mGUI); //TODO!!: creation of ControlsListener: bad place, but must be after GUI init
}

void Application::fillScene()
{
	mGameLogic = new GameLogic(this); //TODO! right place?
	
	// create physical materials
	mFloorPhysicsMaterial = mPhysicsScene->createMaterial();
	mWallPhysicsMaterial = mPhysicsScene->createMaterial();
	mBlobbPhysicsMaterial = mPhysicsScene->createMaterial();
	mBallPhysicsMaterial = mPhysicsScene->createMaterial();
	mNetPhysicsMaterial = mPhysicsScene->createMaterial();

	loadSettings();

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
	NxOgre::PlaneGeometry* pplane = new NxOgre::PlaneGeometry(0, NxOgre::Real3(0, 1, 0));
	pplane->setMaterial(mFloorPhysicsMaterial->getIdentifier());
	mFloorSceneGeometry = mPhysicsScene->createSceneGeometry(pplane);

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
	mBlobb1 = new Blobb(this, mSceneMgr, mPhysicsScene, Vector3(-arenaExtent[0]/4,1.0,0.0), TEAM1);
	mBlobb2 = new Blobb(this, mSceneMgr, mPhysicsScene, Vector3(arenaExtent[0]/4,1.0,0.0), TEAM2, ColourValue::Green);

	//ball
	mBall = new Ball(this, Vector3(-arenaExtent[0]/4,6.0,0.0));

	// create net
	/* net1.3ds
	width: 12.0		height: 2.1		depth: 0.14
	*/
	Real netHeight = mConfig.getSettingReal("NET_HEIGHT");
	ent = mSceneMgr->createEntity("Net_a", "Net2a.mesh"); //TODO!: fix netz-befestigungen in Blender (some triangles face to the wrong side)
	Entity *ent2 = mSceneMgr->createEntity("Net_b", "Net2b.mesh");
	SceneNode* netNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	//node->scale(Vector3(BALL_RADIUS / 1.7));
	//node->translate(Vector3(-ARENA_EXTENT[0]/2,6.0,0.0));
	netNode->rotate(Vector3::UNIT_Y, Radian(Degree(90)));
	netNode->translate(Vector3(0, netHeight, 6)); //TODO: something wrong with z translation from constants -> scaling(VRS::Vector((ARENA_EXTENT[2] + (2*poleOffset))/width3ds, 1.0, 1.0));
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

	scaleNode->scale(Vector3(0.002));
	scaleNode->scale(1, 44, 1);
	scaleNode->translate(0.0, 2.0, 0); // TODO!: make configurable - net size/position

	poleNode1->translate(0,0,6, SceneNode::TS_WORLD); //TODO: why TS_WORLD??
	poleNode2->translate(0,0,-6, SceneNode::TS_WORLD);


	// compute size for physical net
	netNode->_updateBounds();
	AxisAlignedBox bb = netNode->_getWorldAABB();
	//TODO!: proper netsize -> free space below net
	Vector3 physNetSize = bb.getSize();
	physNetSize.y = netHeight + 2.1; //2.1 is half? the height of the net model

	Vector3 p(0.0, (netHeight+2.1)/2, 0.0);
	NxOgre::Box* pbox = new NxOgre::Box(physNetSize.x, physNetSize.y, physNetSize.z);
	pbox->setMaterial(mNetPhysicsMaterial->getIdentifier());
	mPhysicsScene->createSceneGeometry(pbox, NxOgre::Matrix44(NxOgre::Real3(p.x, p.y, p.z))); 


	// create physical "cage" (walls)
	//TODO!!: visual representation of Walls (transparent/shadows/splatting)
	NxOgre::Shapes wallPlanes;
	wallPlanes.insert(new NxOgre::PlaneGeometry(-arenaExtent.x/2, NxOgre::Real3(1,0,0)));
	wallPlanes.insert(new NxOgre::PlaneGeometry(-arenaExtent.x/2, NxOgre::Real3(-1,0,0)));
	wallPlanes.insert(new NxOgre::PlaneGeometry(-arenaExtent.z/2, NxOgre::Real3(0,0,1)));
	wallPlanes.insert(new NxOgre::PlaneGeometry(-arenaExtent.z/2, NxOgre::Real3(0,0,-1)));

	for(int i=0; i<4; ++i) wallPlanes[i]->setMaterial(mWallPhysicsMaterial->getIdentifier());

	mWallsSceneGeometry = mPhysicsScene->createSceneGeometry(wallPlanes);
}

bool Application::frameRenderingQueued(const FrameEvent &evt)
{
	mGUI->getMyGui()->injectFrameEntered(evt.timeSinceLastFrame);

	//if(mMouse) 
	mMouse->capture();
	//if(mKeyboard) 
	mKeyboard->capture();

	//mCamNode->translate(mDirection * evt.timeSinceLastFrame, Node::TS_LOCAL);
	mCamera->moveRelative(mControls->mDirection * evt.timeSinceLastFrame); // TODO: translate/moveRelative: does it the same as above?

	mPhysicsTimeController->advance(evt.timeSinceLastFrame*mSimulationSpeed);//1.0f/60.0f);
	mVisualDebugger->draw();
	mVisualDebuggerNode->needUpdate();

	return mControls->mContinueRendering;
}

bool Application::frameEnded(const Ogre::FrameEvent &evt) {
	if (mControls->mStatsOn)
		mGUI->updateStats();
	return true;
}

Application::~Application() {
	delete mControls;
	delete mGameLogic;
	delete mBall;
	delete mBlobb1;
	delete mBlobb2;
}

void Application::loadSettings() {
	mPhysicsScene->setGravity(NxOgre::Vec3(0, getConfig().getSettingReal("gravity"), 0));

	mSimulationSpeed = getConfig().getSettingReal("simulationSpeed");

	// load all the physX material settings
	mFloorPhysicsMaterial->setDynamicFriction(mConfig.getSettingReal("floorDynamicFriction"));
	mFloorPhysicsMaterial->setStaticFriction(mConfig.getSettingReal("floorStaticFriction"));
	mFloorPhysicsMaterial->setRestitution(mConfig.getSettingReal("floorRestitution"));

	mWallPhysicsMaterial->setDynamicFriction(mConfig.getSettingReal("wallDynamicFriction"));
	mWallPhysicsMaterial->setStaticFriction(mConfig.getSettingReal("wallStaticFriction"));
	mWallPhysicsMaterial->setRestitution(mConfig.getSettingReal("wallRestitution"));

	mBlobbPhysicsMaterial->setDynamicFriction(mConfig.getSettingReal("blobbDynamicFriction"));
	mBlobbPhysicsMaterial->setStaticFriction(mConfig.getSettingReal("blobbStaticFriction"));
	mBlobbPhysicsMaterial->setRestitution(mConfig.getSettingReal("blobbRestitution"));

	mBallPhysicsMaterial->setDynamicFriction(mConfig.getSettingReal("ballDynamicFriction"));
	mBallPhysicsMaterial->setStaticFriction(mConfig.getSettingReal("ballStaticFriction"));
	mBallPhysicsMaterial->setRestitution(mConfig.getSettingReal("ballRestitution"));

	mNetPhysicsMaterial->setDynamicFriction(mConfig.getSettingReal("netDynamicFriction"));
	mNetPhysicsMaterial->setStaticFriction(mConfig.getSettingReal("netStaticFriction"));
	mNetPhysicsMaterial->setRestitution(mConfig.getSettingReal("netRestitution"));
}

void Application::addToConsole(Ogre::String string) {
	mGUI->getConsole()->addToConsole(string);
}



