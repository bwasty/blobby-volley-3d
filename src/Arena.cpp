
#include "Arena.h"
#include <time.h>
//#include <vrs/so.h>
#include <vrs/opengl/shapematerialgl.h>
#include <vrs/container/fixedsizeiterator.h>
#include <vrs/polygonset.h>
#include <vrs/sg/scenething.h>
#include <vrs/facestyle.h>
#include <vrs/cylinder.h>
#include <vrs/cone.h>
#include <vrs/translation.h>
#include <vrs/box.h>
#include <vrs/io/threedsreader.h>
//#include <vrs/container/array.h>

#include <Newton.h>
#include "Ball.h"
#include "Referee.h"
#include "Game.h"
#include "Blobb.h"

struct CollisionData {
	int material1;
	int material2;
	VRS::SO<BV3D::Game> game;
	VRS::SO<BV3D::Arena> arena;
	VRS::SO<BV3D::Ball> ball;
	VRS::SO<BV3D::Referee> referee;
	VRS::SO<BV3D::Blobb> currentBlobb;
	NewtonWorld* world;
	int delayStartFrame;
};

/**
 * sets up an Arena with a predefined extent
 */
BV3D::Arena::Arena() {

	m_Scene = new VRS::SceneThing();

	m_Net = new VRS::SceneThing();
	m_Scene->append(m_Net);

	m_WallsVertices = new VRS::FixedSizeIterator<VRS::Vector>(10);

	m_Material = new VRS::ShapeMaterialGL(VRS::Color(0.9,0.9,0.9,0.3));
	m_Scene->append(m_Material);

	m_Scene->append(new VRS::FaceStyle(VRS::FaceStyle::Filled,VRS::FaceStyle::Culled));

	m_Walls = new VRS::PolygonSet(VRS::PolygonSet::QuadStrip,m_WallsVertices);
	m_Scene->append(m_Walls);

	// Physics setup
	m_World = NewtonCreate(0,0);
	createMaterials();
	m_Gravity = 20.81f;
	m_Floor = 0;
	m_Body = 0;

	setExtent(BV3D::arenaExtent);//VRS::Vector(1.0,1.0,1.0));
}

BV3D::Arena::~Arena() {
	if(m_Body) {
		NewtonDestroyBody(m_World, m_Body);
		NewtonDestroyBody(m_World, m_Floor);
	}
	NewtonDestroy(m_World);
}

void BV3D::Arena::setExtent(VRS::Vector extent) {
	VRS::Vector llf(-extent[0]/2, 0.0, -extent[2]/2);
	VRS::Vector urb( extent[0]/2, extent[1], extent[2]/2);

	m_Bounds = VRS::Bounds(llf,urb);

	// set up walls' vertices
	m_WallsVertices->set(0, VRS::Vector(urb[0],llf[1],urb[2]));
	m_WallsVertices->set(1, VRS::Vector(urb[0],urb[1],urb[2]));
	m_WallsVertices->set(2, VRS::Vector(llf[0],llf[1],urb[2]));
	m_WallsVertices->set(3, VRS::Vector(llf[0],urb[1],urb[2]));

	m_WallsVertices->set(4, VRS::Vector(llf[0],llf[1],llf[2]));
	m_WallsVertices->set(5, VRS::Vector(llf[0],urb[1],llf[2]));

	m_WallsVertices->set(6, VRS::Vector(urb[0],llf[1],llf[2]));
	m_WallsVertices->set(7, VRS::Vector(urb[0],urb[1],llf[2]));

	m_WallsVertices->set(8, VRS::Vector(urb[0],llf[1],urb[2]));
	m_WallsVertices->set(9, VRS::Vector(urb[0],urb[1],urb[2]));

	// (re)create physical arena body
	const dFloat wallDepth = 1.0f;

	// destroy old physical arena body if it exists
	if(m_Body) {
		NewtonDestroyBody(m_World, m_Body);
		NewtonDestroyBody(m_World, m_Floor);
	}

	NewtonCollision* collision[5];
	dFloat matrix[16] = {1.0,0.0,0.0,0.0, 0.0,1.0,0.0,0.0, 0.0,0.0,1.0,0.0, 0.0,0.0,0.0,1.0};

	// create floor
	matrix[12] = 0.0;
	matrix[13] = (dFloat) -(wallDepth / 2);
	matrix[14] = 0.0;
	//collision[0] = NewtonCreateBox(m_World, (dFloat)extent.get(0), wallDepth, (dFloat)extent.get(2), matrix);
	NewtonCollision* floorCollision = NewtonCreateBox(m_World, (dFloat)extent.get(0), wallDepth, (dFloat)extent.get(2), matrix);
	m_Floor = NewtonCreateBody(m_World, floorCollision);
	NewtonBodySetMaterialGroupID(m_Floor, mFloorMaterialID);
	//NewtonBodySetContinuousCollisionMode(m_Floor, 0);
	NewtonReleaseCollision(m_World, floorCollision);

	// create wall bounding the arena on the positive z axis
	matrix[12] = 0.0;
	matrix[13] = (dFloat) (extent[1] / 2);
	matrix[14] = (dFloat) (extent[2] + wallDepth) / 2;
	collision[0] = NewtonCreateBox(m_World, (dFloat)extent[0], (dFloat)extent[1], wallDepth, matrix);

	// create wall bounding the arena on the negative z axis
	matrix[12] = 0.0;
	matrix[13] = (dFloat)  (extent[1] / 2);
	matrix[14] = (dFloat) -(extent[2] + wallDepth) / 2;
	collision[1] = NewtonCreateBox(m_World, (dFloat)extent[0], (dFloat)extent[1], wallDepth, matrix);

	// create wall bounding the arena on the negative x axis
	matrix[12] = (dFloat) -(extent[0] + wallDepth) / 2;
	matrix[13] = (dFloat)  (extent[1] / 2);
	matrix[14] = 0.0;
	collision[2] = NewtonCreateBox(m_World, wallDepth, (dFloat)extent[1], (dFloat)extent[2], matrix);

	// create wall bounding the arena on the positive x axis
	matrix[12] = (dFloat) (extent[0] + wallDepth) / 2;
	matrix[13] = (dFloat) (extent[1] / 2);
	matrix[14] = 0.0;
	collision[3] = NewtonCreateBox(m_World, wallDepth, (dFloat)extent[1], (dFloat)extent[2], matrix);

	// combine walls and create physical arena body from compound collision
	NewtonCollision* compoundCollision = NewtonCreateCompoundCollision(m_World, 4, collision);
	m_Body = NewtonCreateBody(m_World, compoundCollision);
	NewtonBodySetMaterialGroupID(m_Body, mWallMaterialID);

	// release all collision references
	for(int i=0;i<4;i++)
		NewtonReleaseCollision(m_World, collision[i]);
	NewtonReleaseCollision(m_World, compoundCollision);


	// create net
	/* net1.3ds
		width: 12.0		height: 2.1		depth: 0.14
	*/
	double width3ds = 12.0;
	double height3ds = 2.1;
	VRS::SO<VRS::SceneThing> localNet = new VRS::SceneThing(m_Net);
	double netDepth = 0.03;
	double poleOffset = 1.0;
	Bounds netBoxBounds = Bounds(Vector(netDepth, BV3D::netHeight/2+0.1, BV3D::arenaExtent[2]/2+0.1), Vector(-netDepth, BV3D::netHeight-0.05, -(BV3D::arenaExtent[2]/2+0.1)));
	VRS::Matrix netMatrix = VRS::Matrix::rotation(VRS::Vector(0.0, 1.0, 0.0), VRS::Vector(0.0, 0.0, -(BV3D::arenaExtent[2]/2 + poleOffset)), -90.0);
	netMatrix = netMatrix * VRS::Matrix::translation(VRS::Vector(0.0, BV3D::netHeight - height3ds, -(BV3D::arenaExtent[2]/2 + poleOffset)));
	netMatrix = netMatrix * VRS::Matrix::scaling(VRS::Vector((arenaExtent[2] + (2*poleOffset))/width3ds, 1.0, 1.0));
	localNet->setLocalMatrix(netMatrix);
	VRS::ThreeDSReader::setMaterialMode(VRS::ThreeDSReader::ALL_BUT_CULLING_MATERIAL);
	localNet->append(VRS::ThreeDSReader::readObject(BV3D::threeDSPath + "net1.3ds"));

	double poleRadius = 0.14;
	m_Net->append(new VRS::ShapeMaterialGL(VRS::Color(0.3,0.3,0.3,1.0)));
	VRS::SO<VRS::Cylinder> cylinder = new VRS::Cylinder(Vector(0, BV3D::netHeight, 0), Vector(0,0,0), poleRadius);
	VRS::SO<VRS::Cone> cone = new VRS::Cone(Vector(0,BV3D::netHeight+0.1,0), Vector(0, BV3D::netHeight, 0), 0.0, poleRadius);
	VRS::SO<VRS::SceneThing> pole1 = new VRS::SceneThing();
	VRS::SO<VRS::SceneThing> pole2 = new VRS::SceneThing();
	m_Net->append(pole1);
	m_Net->append(pole2);
	pole1->append(new VRS::Translation(Vector(0,0,-(BV3D::arenaExtent[2]/2+poleRadius+poleOffset))));
	pole1->append(cone);
	pole1->append(cylinder);
	pole2->append(new VRS::Translation(Vector(0,0,BV3D::arenaExtent[2]/2+poleRadius+poleOffset)));
	pole2->append(cone);
	pole2->append(cylinder);

	//physical part of net
	matrix[12] = 0.0;
	matrix[13] = (dFloat)(BV3D::netHeight/2+0.1 + (netBoxBounds.getURB()[1] - netBoxBounds.getLLF()[1])/2);
	matrix[14] = 0.0;
	NewtonCollision* netCollision = NewtonCreateBox(m_World, (dFloat)(netBoxBounds.getLLF()[0] - netBoxBounds.getURB()[0]), 
															(dFloat)(netBoxBounds.getURB()[1] - netBoxBounds.getLLF()[1]),
															(dFloat)(netBoxBounds.getLLF()[2] - netBoxBounds.getURB()[2]), matrix);
	NewtonBody* netBody = NewtonCreateBody(m_World, netCollision);
	NewtonReleaseCollision(m_World, netCollision);
	NewtonBodySetMaterialGroupID(netBody, mNetMaterialID);

	// TODO: invisible Barrier (only physical, positioned under the net, lets ball through but stops blobbs)
	matrix[12] = 0.0;
	matrix[13] = (dFloat)(BV3D::arenaExtent[1]/2);//(dFloat)(netBoxBounds.getLLF()[1] - 0.1) / 2 ;
	matrix[14] = 0.0;
	NewtonCollision* invisibleBarrierCollision = NewtonCreateBox(m_World, 0.06, 
											/*(dFloat)netBoxBounds.getLLF()[1] - 0.1,*/ (dFloat)BV3D::arenaExtent[1],
															(dFloat)(netBoxBounds.getLLF()[2] - netBoxBounds.getURB()[2]), matrix);	
	NewtonBody* invisibleBarrierBody = NewtonCreateBody(m_World, invisibleBarrierCollision);
	NewtonReleaseCollision(m_World, invisibleBarrierCollision);
	NewtonBodySetMaterialGroupID(invisibleBarrierBody, mInvisibleBarrierID);
}

void BV3D::Arena::createAItrigger(BV3D::BV3D_TEAM team) {
	// TODO: parameter is currently ignored!!
	dFloat matrix[16] = {1.0,0.0,0.0,0.0, 0.0,1.0,0.0,0.0, 0.0,0.0,1.0,0.0, 0.0,0.0,0.0,1.0};

	matrix[12] = (dFloat)BV3D::arenaExtent.get(0)/4;
	matrix[13] = BV3D::blobbHeight+0.2;
	matrix[14] = 0.0;
	NewtonCollision* AITriggerCollision = NewtonCreateBox(m_World, (dFloat)BV3D::arenaExtent.get(0)/2 - 0.2, 0, (dFloat)BV3D::arenaExtent.get(2), matrix);
	NewtonBody* AITriggerBody = NewtonCreateBody(m_World, AITriggerCollision);
	NewtonBodySetMaterialGroupID(AITriggerBody, mAITriggerID);
	NewtonReleaseCollision(m_World, AITriggerCollision);
}

VRS::Bounds BV3D::Arena::getTeamBounds(BV3D::BV3D_TEAM team) {
	VRS::Vector llf = m_Bounds.getLLF();
	VRS::Vector urb = m_Bounds.getURB();

	if(team == BV3D::BV3D_TEAM2)
		return VRS::Bounds(VRS::Vector(0.0,0.0,llf[2]), urb);
	else
		return VRS::Bounds(llf, VRS::Vector(0.0,urb[1],urb[2]));
}

void BV3D::Arena::updateWorld(float timestep) {
	NewtonUpdate(m_World, (dFloat)timestep);
}

void BV3D::Arena::createMaterials() {
	// create all material IDs
	mBallMaterialID = NewtonMaterialCreateGroupID(m_World);
	mBlobbMaterialID = NewtonMaterialCreateGroupID(m_World);
	mWallMaterialID = NewtonMaterialCreateGroupID(m_World);
	mFloorMaterialID = NewtonMaterialCreateGroupID(m_World);
	mNetMaterialID = NewtonMaterialCreateGroupID(m_World);
	mInvisibleBarrierID = NewtonMaterialCreateGroupID(m_World); // invisible Wall beneath the net - lets ball through but not blobb
	mAITriggerID = NewtonMaterialCreateGroupID(m_World);
}

void BV3D::Arena::setupMaterials(BV3D::Game* game) {
	// get the default material ID
	int defaultID = NewtonMaterialGetDefaultGroupID (m_World);

	// set default material properties TODO: find appropriate values, define callbacks
	// TODO: only little bouncing? low elasticity/softness? - not working?
	//NewtonMaterialSetDefaultSoftness (m_World, defaultID, defaultID, 0.05f);
	//NewtonMaterialSetDefaultElasticity (m_World, defaultID, defaultID, 0.4f);
	//NewtonMaterialSetDefaultCollidable (m_World, defaultID, defaultID, 1);
	//NewtonMaterialSetDefaultFriction (m_World, defaultID, defaultID, 1.0f, 0.5f);
	//NewtonMaterialSetCollisionCallback (m_World, defaultID, defaultID, NULL, NULL, NULL, NULL);//contactBeginCallback, contactProcessCallback, contactEndCallback);

	// set material interaction properties and callbacks if they should differ from the default value/behaviour
	CollisionData* collData = new CollisionData();
	collData->game = game;
	collData->arena = this;
	collData->ball = game->getBall();
	collData->referee = game->getReferee();
	collData->world = m_World;

	// TODO: ball on blobb - high elasticity, game logic
	NewtonMaterialSetDefaultElasticity (m_World, mBallMaterialID, mBlobbMaterialID, 0.8f);
	NewtonMaterialSetContinuousCollisionMode(m_World, mBallMaterialID, mBlobbMaterialID, 1);
	CollisionData* collDataBallBlobb = new CollisionData(*collData);
	collDataBallBlobb->material1 = mBallMaterialID;
	collDataBallBlobb->material2 = mBlobbMaterialID;
	NewtonMaterialSetCollisionCallback (m_World, mBallMaterialID, mBlobbMaterialID, collDataBallBlobb, contactBeginCallback, contactProcessCallback, NULL);

	// TODO: ball on wall - high elasticity, special effect(energy wall)?
	NewtonMaterialSetDefaultElasticity (m_World, mBallMaterialID, mWallMaterialID, 0.9f);

	// TODO: ball on floor? - low elasticity, game logic
	NewtonMaterialSetDefaultElasticity (m_World, mBallMaterialID, mFloorMaterialID, 0.2f);
	CollisionData* collDataBallFloor = new CollisionData(*collData);
	collDataBallFloor->material1 = mBallMaterialID;
	collDataBallFloor->material2 = mFloorMaterialID;
	NewtonMaterialSetCollisionCallback (m_World, mBallMaterialID, mFloorMaterialID, collDataBallFloor, contactBeginCallback, contactProcessCallback, NULL);

	// TODO: ball on net? - middle elasticity
	NewtonMaterialSetDefaultElasticity (m_World, mBallMaterialID, mNetMaterialID, 0.6f);

	// TODO: ball on invisibleBarrier? - no collision
	CollisionData* collDataBallBarrier = new CollisionData(*collData);
	collDataBallBarrier->material1 = mBallMaterialID;
	collDataBallBarrier->material2 = mInvisibleBarrierID;
	NewtonMaterialSetCollisionCallback (m_World, mBallMaterialID, mInvisibleBarrierID, collDataBallBarrier, contactBeginCallback, contactProcessCallback, NULL);

	// TODO: blobb on invisibleBarrier? - little/high elasticity?, collidable->special callback?, use to detect when ball flies over the net?
	NewtonMaterialSetDefaultElasticity (m_World, mBlobbMaterialID, mInvisibleBarrierID, 0.5f);
	CollisionData* collDataBlobbBarrier = new CollisionData(*collData);
	collDataBlobbBarrier->material1 = mBlobbMaterialID;
	collDataBlobbBarrier->material2 = mInvisibleBarrierID;
	NewtonMaterialSetCollisionCallback (m_World, mBlobbMaterialID, mInvisibleBarrierID, collDataBlobbBarrier, contactBeginCallback, contactProcessCallback, NULL);

	// TODO: set friction for  blobb on surrounding
	NewtonMaterialSetDefaultFriction(m_World, mBlobbMaterialID, mWallMaterialID, 0, 0);
	NewtonMaterialSetDefaultFriction(m_World, mBlobbMaterialID, mFloorMaterialID, 0, 0);
	NewtonMaterialSetDefaultFriction(m_World, mBlobbMaterialID, mNetMaterialID, 0, 0);
	NewtonMaterialSetDefaultFriction(m_World, mBlobbMaterialID, mInvisibleBarrierID, 0, 0);

	// continous collisions for ball on floor?
	//NewtonMaterialSetContinuousCollisionMode(m_World, mBlobbMaterialID, mFloorMaterialID, 1);

	// TODO: Ball on AI Trigger:
	//collData->currentBlobb = game->getBlobb(2);
	srand(time(0));
	NewtonMaterialSetCollisionCallback (m_World, mBallMaterialID, mAITriggerID, collData, NULL, AICallback, NULL);

	// AI trigger shouldn't collide with blobbs
	NewtonMaterialSetDefaultCollidable(m_World, mBlobbMaterialID, mAITriggerID, 0);
}

int BV3D::Arena::contactBeginCallback(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1) {

	CollisionData* collData = (CollisionData*)NewtonMaterialGetMaterialPairUserData(material);

	if (collData->material2 == collData->arena->getBlobbMaterialID()) {
		//printf("contactBEGINCallback called with blobb involved\n"); 
		VRS::SharedObj* so = (VRS::SharedObj*)NewtonBodyGetUserData(body0);
		if (so->classNameVRS() == BV3D::Blobb::ClassNameVRS())
			collData->currentBlobb = (BV3D::Blobb*)so;
		else
			collData->currentBlobb = (BV3D::Blobb*)NewtonBodyGetUserData(body1);
	}

	return 1;
}

int BV3D::Arena::contactProcessCallback(const NewtonMaterial* material, const NewtonContact* contact) {
	//printf("contactPROCESSCallback called\n"); 

	CollisionData* collData = (CollisionData*)NewtonMaterialGetMaterialPairUserData(material);
	BV3D::BV3D_TEAM team;

	if (collData->material1 == collData->arena->getBallMaterialID() && collData->material2 == collData->arena->getBlobbMaterialID()) {
		//printf("Blobb touched Ball PROCESS\n");
		NewtonWorldUnfreezeBody(collData->arena->getWorld(), collData->ball->getBody());
		BV3D::BV3D_TEAM team = collData->currentBlobb->getTeam();

		// wait some frames after blobb touches ball before new touch gets counted
		if (collData->referee->getCurrentContacts(team) > 0 && collData->game->getFrameCount() - collData->delayStartFrame < 25) {
			collData->delayStartFrame = 0;
			//printf("prevented touch\n");
		}
		else {
			if (team == BV3D::BV3D_TEAM1)
				collData->referee->ballOnBlobb(BV3D::BV3D_TEAM1);
			else {
				collData->referee->ballOnBlobb(BV3D::BV3D_TEAM2);
			}
			collData->delayStartFrame=collData->game->getFrameCount();
			collData->game->playSoundTouch();
		}
	}
	else if (collData->material1 == collData->arena->getBallMaterialID() && collData->material2 == collData->arena->getFloorMaterialID()) {
		if (collData->ball->getPosition()[0] < 0) {
			team = BV3D::BV3D_TEAM1;
		}
		else {
			team = BV3D::BV3D_TEAM2;
		}
		collData->referee->ballOnField(team);

		// for AI
		NewtonMaterialSetDefaultCollidable(collData->world, collData->arena->getBallMaterialID(), collData->arena->getAITriggerID(), 1);

		//printf("%f, %f, %f\n", collData->ball->getPosition()[0], collData->ball->getPosition()[1], collData->ball->getPosition()[2]); // x-coordinate of the ball	
	}
	else if (collData->material1 == collData->arena->getBallMaterialID() && collData->material2 == collData->arena->getInvisibleBarrierID()) {
		// tell referee if ball passes under the net
		Vector pos = collData->ball->getPosition();
		if (pos[1] < BV3D::netHeight/2) {
			if (pos[0] < 0)
				team = BV3D::BV3D_TEAM1;
			else
				team = BV3D::BV3D_TEAM2;
			collData->referee->ballOnField(team);
		}
		return 0;
	}
	
	return 1;
}

void BV3D::Arena::contactEndCallback(const NewtonMaterial* material) {
	//printf("contactENDCallback called\n"); 
}

int BV3D::Arena::AICallback(const NewtonMaterial* material, const NewtonContact* contact) {
	CollisionData* collData = (CollisionData*)NewtonMaterialGetMaterialPairUserData(material);

	if (!collData->referee->getActive())
		return 0;

	NewtonBody* body = collData->ball->getBody();
	dFloat v[3];
	NewtonBodyGetVelocity(body, v);

	int random = rand() % 15;
	if (random < 14) {
		// move blobb to contact point
		dFloat pos[3], norm[3];
		NewtonMaterialGetContactPositionAndNormal(material, pos, norm);
		//printf("contact position: %f, %f, %f\n", pos[0], pos[1], pos[2]);
		collData->game->getBlobb(2)->setPosition(Vector(pos[0], 0, pos[2]));

		if (v[0] < 0) { // ball moves toward the net -> use normal collision
			NewtonMaterialSetContactElasticity(material, 0.8+(rand()%8)/10.0);
			collData->referee->ballOnBlobb(BV3D::BV3D_TEAM2);
			collData->game->playSoundTouch();
			return 1;
		}
		else {
			random = rand() % 10;
			//printf("%d", random);
			if (random<5 || (collData->referee->getCurrentContacts(BV3D::BV3D_TEAM2)>1 && v[0] > 0)) {
				random = (rand()%8) / 10.0;
				v[0] = - v[0]*(0.8+random);
				v[1] = - v[1]*(0.8+random);
				v[2] = - v[2]*(0.8+random);
				NewtonBodySetVelocity(body, v);
				collData->referee->ballOnBlobb(BV3D::BV3D_TEAM2);
				collData->game->playSoundTouch();
				return 0;
			}
			else if (random <10) {
				random = rand()%9;
				NewtonMaterialSetContactElasticity(material, 0.9+random/10);
				collData->referee->ballOnBlobb(BV3D::BV3D_TEAM2);
				collData->game->playSoundTouch();
				return 1;
			}
		}
	}
	else {
		NewtonMaterialSetDefaultCollidable(collData->world, collData->arena->getBallMaterialID(), collData->arena->getAITriggerID(), 0);
		return 0;
	}
}
