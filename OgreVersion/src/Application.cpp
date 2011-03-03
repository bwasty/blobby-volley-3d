// TODO!!: check all destructors (->effective c++), replace pointers with local members / refs?
// TODO!!: FileWatcher/key instead of manual config load with console?
// TODO: precompiled headers?
#include "Application.h"

//#include "Constants.h"
//#include <OgrePrerequisites.h>

//#include <OgreViewport.h>
//#include <OgreLight.h>
//#include <OgreRoot.h>
//#include <OgreEntity.h>
//#include <OgreRenderWindow.h>
//#include <OgreMeshManager.h>

//#include <NxOgre.h>
#include <OGRE3DRenderSystem.h>
#include <OGRE3DBody.h>

//#include <MyGUI.h>
#include <Hydrax/Hydrax.h>

#include "Blobb.h"
#include "Ball.h"
#include "Controls.h"
#include "GUI.h"
#include "Console.h"
#include "GameLogic.h"


using namespace Ogre;

// rnd_ and createPalms copied from Hydrax Demo
/** Just to locate palmiers with a pseudo-random algoritm
 */
float seed_ = 801;
float rnd_(const float& min, const float& max)
{
	seed_ += Ogre::Math::PI*2.8574f + seed_*(0.3424f - 0.12434f + 0.452345f);
	if (seed_ > 10000000000) seed_ -= 10000000000;
	return ((max-min)*Ogre::Math::Abs(Ogre::Math::Sin(Ogre::Radian(seed_))) + min);
}

void createPalms(Ogre::SceneManager *mSceneMgr)
{
	const int NumberOfPalms = 12;

	Ogre::SceneNode* mPalmsSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		
	for (int k = 0; k < NumberOfPalms; k++)
	{
		Ogre::Vector3 RandomPos = Ogre::Vector3(rnd_(500,2500),
			0,
			rnd_(500,2500));

		Ogre::RaySceneQuery * raySceneQuery = mSceneMgr->
			createRayQuery(Ogre::Ray(RandomPos + Ogre::Vector3(0,1000000,0), 
			Ogre::Vector3::NEGATIVE_UNIT_Y));

		Ogre::RaySceneQueryResult& qryResult = raySceneQuery->execute();
		Ogre::RaySceneQueryResult::iterator i = qryResult.begin();

		if (i != qryResult.end() && i->worldFragment)
		{
			if (i->worldFragment->singleIntersection.y>105 || i->worldFragment->singleIntersection.y<20)
			{
				k--;
				continue;
			}

			RandomPos.y = i->worldFragment->singleIntersection.y;
		}
		else
		{
			k--;
			continue;
		}

		Ogre::Entity *mPalmEnt = mSceneMgr->createEntity("Palm"+Ogre::StringConverter::toString(k), "Palm.mesh");
		Ogre::SceneNode *mPalmSN = mPalmsSceneNode->createChildSceneNode();

		mPalmSN->rotate(Ogre::Vector3(-1,0,rnd_(-0.3,0.3)), Ogre::Degree(90));
		mPalmSN->attachObject(mPalmEnt);
		Ogre::Real Scale = rnd_(50,75);
		mPalmSN->scale(Scale,Scale,Scale);
		mPalmSN->setPosition(RandomPos);
	}
}

Application::Application() : BaseApplication() {
	mArenaTransform = Matrix4();
	mArenaInverseTransform = Matrix4();

	Vector3 position = getConfig().getSettingVector3("arenaPosition");
	Real scale = getConfig().getSettingReal("arenaScale");
	Real rotation = getConfig().getSettingReal("arenaRotation");
	Quaternion rotQuat = Quaternion(Radian(Degree(rotation)), Vector3(0,1,0));

	mArenaTransform.makeTransform(position, Vector3(scale), rotQuat);
	mArenaInverseTransform.makeInverseTransform(position, Vector3(scale), rotQuat);


}

void Application::setupInputSystem() {
	BaseApplication::setupInputSystem();
}

void Application::setupGUI() { 
	mGUI = new GUI(this, mWindow); 
}

void Application::setupListener() {
	mRoot->addFrameListener(this);

	mControls = new ControlsListener(this, mCamera, mSceneMgr, mKeyboard, mMouse, mGUI);
}

void Application::fillScene()
{
	mGameLogic = new GameLogic(this); //TODO!!: right place? mGameLogic = new GameLogic(this);
	
	// camera position and orientation
	mCamera->setPosition(getArenaTransform() * mConfig.getSettingVector3("LOOK_FROM"));
	mCamera->lookAt(getArenaTransform() * mConfig.getSettingVector3("LOOK_TO"));

	// create physical materials
	mFloorPhysicsMaterial = mPhysicsScene->createMaterial();
	mWallPhysicsMaterial = mPhysicsScene->createMaterial();
	mBlobbPhysicsMaterial = mPhysicsScene->createMaterial();
	mBallPhysicsMaterial = mPhysicsScene->createMaterial();
	mNetPhysicsMaterial = mPhysicsScene->createMaterial();

	loadSettings();
	
	Entity* ent;
	//// floor plane
	//Plane plane(Vector3::UNIT_Y, 0);
	//MeshManager::getSingleton().createPlane("ground",
	//	ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
	//	500,500,20,20,true,1,5,5,Vector3::UNIT_Z);

	//ent = mSceneMgr->createEntity("GroundEntity", "ground");
	//mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
	//ent->setMaterialName("sand_yellow");
	//ent->setCastShadows(false);

	// Load island and palms - copied from Hydrax Demo
	mSceneMgr->setWorldGeometry("Island.cfg");
	
	mHydrax->getMaterialManager()->addDepthTechnique(
		static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("Island"))
		->createTechnique());

	createPalms(mSceneMgr);


	// physical ground plane
	NxOgre::PlaneGeometry* pplane = new NxOgre::PlaneGeometry((mArenaTransform * Vector3(0.0)).y, NxOgre::Vec3(0, 1, 0));
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
	mBlobb1 = new Blobb(this, mArenaTransform * Vector3(-arenaExtent[0]/4,1.0,0.0), TEAM1);
	mBlobb2 = new Blobb(this, mArenaTransform * Vector3(arenaExtent[0]/4,1.0,0.0), TEAM2, ColourValue::Green);

	//ball
	mBall = new Ball(this, mGameLogic->getServePointTeam1());

	// create net
	/* net1.3ds
	width: 12.0		height: 2.1		depth: 0.14
	*/
	SceneNode* completeNetNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	Real netHeight = mConfig.getSettingReal("NET_HEIGHT");
	ent = mSceneMgr->createEntity("Net_a", "Net2a.mesh"); //TODO!: fix netz-befestigungen in Blender (some triangles face to the wrong side)
	Entity *ent2 = mSceneMgr->createEntity("Net_b", "Net2b.mesh");
	SceneNode* netNode = completeNetNode->createChildSceneNode();
	//node->scale(Vector3(BALL_RADIUS / 1.7));
	//node->translate(Vector3(-ARENA_EXTENT[0]/2,6.0,0.0));
	netNode->rotate(Vector3::UNIT_Y, Radian(Degree(90)));
	netNode->translate(Vector3(0, netHeight, 6)); //TODO: something wrong with z translation from constants -> scaling(VRS::Vector((ARENA_EXTENT[2] + (2*poleOffset))/width3ds, 1.0, 1.0));
	netNode->attachObject(ent);
	netNode->attachObject(ent2);

	// use prefab shapes for flag poles
	ent = mSceneMgr->createEntity("net pole 1", SceneManager::PT_CUBE);
	ent2 = mSceneMgr->createEntity("net pole 2", SceneManager::PT_CUBE);
	SceneNode* scaleNode = completeNetNode->createChildSceneNode();
	SceneNode* poleNode1 = scaleNode->createChildSceneNode();
	poleNode1->attachObject(ent);
	SceneNode* poleNode2 = scaleNode->createChildSceneNode();
	poleNode2->attachObject(ent2);

	scaleNode->scale(Vector3(0.002)); // TODO!!: why 2x scale? (net creation)
	scaleNode->scale(1, 44, 1);
	scaleNode->translate(0.0, 2.0, 0); // TODO!: make configurable - net size/position

	poleNode1->translate(0,0,6, SceneNode::TS_WORLD); //TODO: why TS_WORLD??
	poleNode2->translate(0,0,-6, SceneNode::TS_WORLD);

	// compute size for physical net
	netNode->_updateBounds();
	AxisAlignedBox netBounds = netNode->_getWorldAABB();

	// global arena transform - can't set complete matrix for SceneNode, so...
	// TODO!!: dirty? (arena transform of net...)
	completeNetNode->scale(Vector3(getConfig().getSettingReal("arenaScale")));
	completeNetNode->rotate(Vector3::UNIT_Y, Radian(Degree(getConfig().getSettingReal("arenaRotation"))), SceneNode::TS_WORLD);
	completeNetNode->translate(mArenaTransform.getTrans());


	//// compute size for physical net
	//TODO!: proper netsize -> free space below net
	//TODO!!!: scaling physical net doesn't work right (arenaTransform)
	Vector3 physNetSize = netBounds.getSize();
	physNetSize.y = netHeight + 2.1; //2.1 is half? the height of the net model
	NxOgre::Box* pbox = new NxOgre::Box(physNetSize.x, physNetSize.y, physNetSize.z);
	pbox->setMaterial(mNetPhysicsMaterial->getIdentifier());
	NxOgre::Matrix44 pose = fromMatrix4(getArenaTransform() * Matrix4::getTrans(0.0, (netHeight+2.1)/2, 0.0));
	mPhysicsScene->createSceneGeometry(pbox, pose); 


	// create physical "cage" (walls)
	//TODO!!: visual representation of Walls (transparent/shadows/splatting)
	// TODO!!!!: when scaled, blobb doesn't collide with net/walls
	NxOgre::Shapes wallPlanes;

	Real arenaScale = getConfig().getSettingReal("arenaScale");

	/// tmp test code
	Vector3 position = getConfig().getSettingVector3("arenaPosition");
	//Real scale = getConfig().getSettingReal("arenaScale");
	Real rotation = getConfig().getSettingReal("arenaRotation");
	Quaternion rotQuat = Quaternion(Radian(Degree(rotation)), Vector3(0,1,0));

	Matrix4 tm;
	tm.makeTransform(position, Vector3(1), rotQuat);
	////

	NxOgre::ShapeBlueprint* sbp = new NxOgre::ShapeBlueprint();
	sbp->mLocalPose = fromMatrix4(getArenaTransform() * Matrix4::getTrans(-arenaExtent.x/2, arenaExtent.y/2, 0));
	// sbp->mSize = NxOgre::Vec4(20,20,20,1); this doesn't really scale
	wallPlanes.insert(new NxOgre::Box(arenaScale * NxOgre::Vec3(0.2, arenaExtent.y, arenaExtent.z), sbp)); // (from cameraposition) right wall

	sbp = new NxOgre::ShapeBlueprint();
	sbp->mLocalPose = fromMatrix4(getArenaTransform() * Matrix4::getTrans(arenaExtent.x/2, arenaExtent.y/2, 0));
	wallPlanes.insert(new NxOgre::Box(arenaScale * NxOgre::Vec3(0.2, arenaExtent.y, arenaExtent.z), sbp)); // left wall

	sbp = new NxOgre::ShapeBlueprint();
	sbp->mLocalPose = fromMatrix4(getArenaTransform() * Matrix4::getTrans(0, arenaExtent.y/2, -arenaExtent.z/2));
	wallPlanes.insert(new NxOgre::Box(arenaScale * NxOgre::Vec3(arenaExtent.x, arenaExtent.y, 0.2), sbp)); // front wall

	sbp = new NxOgre::ShapeBlueprint();
	sbp->mLocalPose = fromMatrix4(getArenaTransform() * Matrix4::getTrans(0, arenaExtent.y/2, arenaExtent.z/2));
	wallPlanes.insert(new NxOgre::Box(arenaScale * NxOgre::Vec3(arenaExtent.x, arenaExtent.y, 0.2), sbp)); // back wall

	for(int i=0; i<4; ++i) wallPlanes[i]->setMaterial(mWallPhysicsMaterial->getIdentifier());

	mWallsSceneGeometry = mPhysicsScene->createSceneGeometry(wallPlanes);
}

bool Application::frameStarted(const Ogre::FrameEvent &evt) {
	return true;
}

bool Application::frameRenderingQueued(const FrameEvent &evt)
{
	mGUI->getMyGui()->injectFrameEntered(evt.timeSinceLastFrame);

	mHydrax->update(evt.timeSinceLastFrame);

	//if(mMouse) 
	mMouse->capture();
	//if(mKeyboard) 
	mKeyboard->capture();

	//mCamNode->translate(mCurrentCameraMovementPerSecond * evt.timeSinceLastFrame, Node::TS_LOCAL);
	mCamera->moveRelative(mControls->getCurrentCameraMovementPerSecond() * evt.timeSinceLastFrame); // TODO: translate/moveRelative: does it the same as above?

	mPhysicsTimeController->advance(evt.timeSinceLastFrame*mSimulationSpeed);//1.0f/60.0f);
	mVisualDebugger->draw();
	mVisualDebuggerNode->needUpdate();

	mGameLogic->update();

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
	delete mGUI;
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



