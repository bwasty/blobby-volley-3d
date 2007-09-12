#include "Arena.h"
#include <time.h>
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

#include <Newton.h>
#include "Ball.h"
#include "Referee.h"
#include "Game.h"
#include "Blobb.h"
#include "ModelOptimizer.h"
#include "AI.h"

/**
 * sets up an Arena with a predefined extent
 */
BV3D::Arena::Arena(BV3D::Game* game) {
	mGame = game;

	mScene = new VRS::SceneThing();

	mNet = new VRS::SceneThing();
	mScene->append(mNet);

	mWallsVertices = new VRS::FixedSizeIterator<VRS::Vector>(10);

	mMaterial = new VRS::ShapeMaterialGL(VRS::Color(0.9,0.9,0.9,0.2));
	mScene->append(mMaterial);

	mScene->append(new VRS::FaceStyle(VRS::FaceStyle::Filled,VRS::FaceStyle::Culled));

	mWalls = new VRS::PolygonSet(VRS::PolygonSet::QuadStrip,mWallsVertices);
	mScene->append(mWalls);

	// Physics setup
	mWorld = NewtonCreate(0,0);
	createMaterials();
	mFloor = 0;
	mBody = 0;

	setExtent(BV3D::ARENA_EXTENT);
}

BV3D::Arena::~Arena() {
	if(mBody) {
		NewtonDestroyBody(mWorld, mBody);
		NewtonDestroyBody(mWorld, mFloor);
	}
	NewtonDestroy(mWorld);
}

void BV3D::Arena::setExtent(VRS::Vector extent) {
	VRS::Vector llf(-extent[0]/2, 0.0, -extent[2]/2);
	VRS::Vector urb( extent[0]/2, extent[1], extent[2]/2);

	mBounds = VRS::Bounds(llf,urb);

	// set up walls' vertices
	mWallsVertices->set(0, VRS::Vector(urb[0],llf[1],urb[2]));
	mWallsVertices->set(1, VRS::Vector(urb[0],urb[1],urb[2]));
	mWallsVertices->set(2, VRS::Vector(llf[0],llf[1],urb[2]));
	mWallsVertices->set(3, VRS::Vector(llf[0],urb[1],urb[2]));

	mWallsVertices->set(4, VRS::Vector(llf[0],llf[1],llf[2]));
	mWallsVertices->set(5, VRS::Vector(llf[0],urb[1],llf[2]));

	mWallsVertices->set(6, VRS::Vector(urb[0],llf[1],llf[2]));
	mWallsVertices->set(7, VRS::Vector(urb[0],urb[1],llf[2]));

	mWallsVertices->set(8, VRS::Vector(urb[0],llf[1],urb[2]));
	mWallsVertices->set(9, VRS::Vector(urb[0],urb[1],urb[2]));

	// (re)create physical arena body
	const dFloat wallDepth = 1.0f;

	// destroy old physical arena body if it exists
	if(mBody) {
		NewtonDestroyBody(mWorld, mBody);
		NewtonDestroyBody(mWorld, mFloor);
	}

	NewtonCollision* collision[5];
	dFloat matrix[16] = {1.0,0.0,0.0,0.0, 0.0,1.0,0.0,0.0, 0.0,0.0,1.0,0.0, 0.0,0.0,0.0,1.0};

	// create floor
	matrix[12] = 0.0;
	matrix[13] = (dFloat) -(wallDepth / 2);
	matrix[14] = 0.0;
	//collision[0] = NewtonCreateBox(mWorld, (dFloat)extent.get(0), wallDepth, (dFloat)extent.get(2), matrix);
	NewtonCollision* floorCollision = NewtonCreateBox(mWorld, (dFloat)extent.get(0), wallDepth, (dFloat)extent.get(2), matrix);
	mFloor = NewtonCreateBody(mWorld, floorCollision);
	NewtonBodySetMaterialGroupID(mFloor, mFloorMaterialID);
	//NewtonBodySetContinuousCollisionMode(mFloor, 0);
	NewtonReleaseCollision(mWorld, floorCollision);

	// create wall bounding the arena on the positive z axis
	matrix[12] = 0.0;
	matrix[13] = (dFloat) (extent[1] / 2);
	matrix[14] = (dFloat) (extent[2] + wallDepth) / 2;
	collision[0] = NewtonCreateBox(mWorld, (dFloat)extent[0], (dFloat)extent[1], wallDepth, matrix);

	// create wall bounding the arena on the negative z axis
	matrix[12] = 0.0;
	matrix[13] = (dFloat)  (extent[1] / 2);
	matrix[14] = (dFloat) -(extent[2] + wallDepth) / 2;
	collision[1] = NewtonCreateBox(mWorld, (dFloat)extent[0], (dFloat)extent[1], wallDepth, matrix);

	// create wall bounding the arena on the negative x axis
	matrix[12] = (dFloat) -(extent[0] + wallDepth) / 2;
	matrix[13] = (dFloat)  (extent[1] / 2);
	matrix[14] = 0.0;
	collision[2] = NewtonCreateBox(mWorld, wallDepth, (dFloat)extent[1], (dFloat)extent[2], matrix);

	// create wall bounding the arena on the positive x axis
	matrix[12] = (dFloat) (extent[0] + wallDepth) / 2;
	matrix[13] = (dFloat) (extent[1] / 2);
	matrix[14] = 0.0;
	collision[3] = NewtonCreateBox(mWorld, wallDepth, (dFloat)extent[1], (dFloat)extent[2], matrix);

	// combine walls and create physical arena body from compound collision
	NewtonCollision* compoundCollision = NewtonCreateCompoundCollision(mWorld, 4, collision);
	mBody = NewtonCreateBody(mWorld, compoundCollision);
	NewtonBodySetMaterialGroupID(mBody, mWallMaterialID);

	// release all collision references
	for(int i=0;i<4;i++)
		NewtonReleaseCollision(mWorld, collision[i]);
	NewtonReleaseCollision(mWorld, compoundCollision);


	// create net
	/* net1.3ds
		width: 12.0		height: 2.1		depth: 0.14
	*/
	VRS::SO<ModelOptimizer> optimizer = new ModelOptimizer();
	double width3ds = 12.0;
	double height3ds = 2.1;
	VRS::SO<VRS::SceneThing> localNet = new VRS::SceneThing(mNet);
	double netDepth = 0.03;
	double poleOffset = 1.0;
	Bounds netBoxBounds = Bounds(Vector(netDepth, BV3D::NET_HEIGHT/2+0.1, BV3D::ARENA_EXTENT[2]/2+0.1), Vector(-netDepth, BV3D::NET_HEIGHT-0.05, -(BV3D::ARENA_EXTENT[2]/2+0.1)));
	VRS::Matrix netMatrix = VRS::Matrix::rotation(VRS::Vector(0.0, 1.0, 0.0), VRS::Vector(0.0, 0.0, -(BV3D::ARENA_EXTENT[2]/2 + poleOffset)), -90.0);
	netMatrix = netMatrix * VRS::Matrix::translation(VRS::Vector(0.0, BV3D::NET_HEIGHT - height3ds, -(BV3D::ARENA_EXTENT[2]/2 + poleOffset)));
	netMatrix = netMatrix * VRS::Matrix::scaling(VRS::Vector((ARENA_EXTENT[2] + (2*poleOffset))/width3ds, 1.0, 1.0));
	localNet->setLocalMatrix(netMatrix);
	localNet->append(optimizer->get3dsModel(BV3D::MODELS_PATH + "net1.3ds"));

	double poleRadius = 0.14;
	mNet->append(new VRS::ShapeMaterialGL(VRS::Color(0.6,0.6,0.6,1.0)));
	VRS::SO<VRS::Cylinder> cylinder = new VRS::Cylinder(Vector(0, BV3D::NET_HEIGHT, 0), Vector(0,0,0), poleRadius);
	VRS::SO<VRS::Cone> cone = new VRS::Cone(Vector(0,BV3D::NET_HEIGHT+0.1,0), Vector(0, BV3D::NET_HEIGHT, 0), 0.0, poleRadius);
	VRS::SO<VRS::SceneThing> pole1 = new VRS::SceneThing();
	VRS::SO<VRS::SceneThing> pole2 = new VRS::SceneThing();
	mNet->append(pole1);
	mNet->append(pole2);
	pole1->append(new VRS::Translation(Vector(0,0,-(BV3D::ARENA_EXTENT[2]/2+poleRadius+poleOffset))));
	pole1->append(cone);
	pole1->append(cylinder);
	pole2->append(new VRS::Translation(Vector(0,0,BV3D::ARENA_EXTENT[2]/2+poleRadius+poleOffset)));
	pole2->append(cone);
	pole2->append(cylinder);

	//physical part of net
	matrix[12] = 0.0;
	matrix[13] = (dFloat)(BV3D::NET_HEIGHT/2+0.1 + (netBoxBounds.getURB()[1] - netBoxBounds.getLLF()[1])/2);
	matrix[14] = 0.0;
	NewtonCollision* netCollision = NewtonCreateBox(mWorld, (dFloat)(netBoxBounds.getLLF()[0] - netBoxBounds.getURB()[0]), 
															(dFloat)(netBoxBounds.getURB()[1] - netBoxBounds.getLLF()[1]),
															(dFloat)(netBoxBounds.getLLF()[2] - netBoxBounds.getURB()[2]), matrix);
	NewtonBody* netBody = NewtonCreateBody(mWorld, netCollision);
	NewtonReleaseCollision(mWorld, netCollision);
	NewtonBodySetMaterialGroupID(netBody, mNetMaterialID);

	// invisible Barrier (only physical, positioned under the net, lets ball through but stops blobbs)
	matrix[12] = 0.0;
	matrix[13] = (dFloat)(BV3D::ARENA_EXTENT[1]/2);
	matrix[14] = 0.0;
	NewtonCollision* invisibleBarrierCollision = NewtonCreateBox(mWorld, 0.06f, 
											(dFloat)BV3D::ARENA_EXTENT[1],
											(dFloat)(netBoxBounds.getLLF()[2] - netBoxBounds.getURB()[2]), matrix);	
	NewtonBody* invisibleBarrierBody = NewtonCreateBody(mWorld, invisibleBarrierCollision);
	NewtonReleaseCollision(mWorld, invisibleBarrierCollision);
	NewtonBodySetMaterialGroupID(invisibleBarrierBody, mInvisibleBarrierID);
}


VRS::Bounds BV3D::Arena::getTeamBounds(BV3D::TEAM team) {
	VRS::Vector llf = mBounds.getLLF();
	VRS::Vector urb = mBounds.getURB();

	if(team == BV3D::TEAM2)
		return VRS::Bounds(VRS::Vector(0.0,0.0,llf[2]), urb);
	else
		return VRS::Bounds(llf, VRS::Vector(0.0,urb[1],urb[2]));
}

void BV3D::Arena::updateWorld(float timestep) {
	NewtonUpdate(mWorld, (dFloat)timestep);
}

void BV3D::Arena::createMaterials() {
	// create all material IDs
	mBallMaterialID = NewtonMaterialCreateGroupID(mWorld);
	mBlobbMaterialID = NewtonMaterialCreateGroupID(mWorld);
	mWallMaterialID = NewtonMaterialCreateGroupID(mWorld);
	mFloorMaterialID = NewtonMaterialCreateGroupID(mWorld);
	mNetMaterialID = NewtonMaterialCreateGroupID(mWorld);
	mInvisibleBarrierID = NewtonMaterialCreateGroupID(mWorld); // invisible Wall beneath the net - lets ball through but not blobb
	mAiTriggerID = NewtonMaterialCreateGroupID(mWorld);
}

void BV3D::Arena::setupMaterials() {
	// get the default material ID
	int defaultID = NewtonMaterialGetDefaultGroupID (mWorld);

	// set material interaction properties and callbacks if they should differ from the default value/behaviour
	CollisionData* collData = new CollisionData();
	collData->game = mGame;
	collData->arena = this;
	collData->ball = mGame->getBall();
	collData->referee = mGame->getReferee();
	collData->world = mWorld;

	// ball on blobb - high elasticity
	NewtonMaterialSetDefaultElasticity (mWorld, mBallMaterialID, mBlobbMaterialID, 0.8f);
	NewtonMaterialSetContinuousCollisionMode(mWorld, mBallMaterialID, mBlobbMaterialID, 1);
	CollisionData* collDataBallBlobb = new CollisionData(*collData);
	collDataBallBlobb->material1 = mBallMaterialID;
	collDataBallBlobb->material2 = mBlobbMaterialID;
	NewtonMaterialSetCollisionCallback (mWorld, mBallMaterialID, mBlobbMaterialID, collDataBallBlobb, blobbContactBeginCallback, contactProcessCallback, NULL);

	// ball on wall - high elasticity
	NewtonMaterialSetDefaultElasticity (mWorld, mBallMaterialID, mWallMaterialID, 0.9f);

	// ball on floor - low elasticity
	NewtonMaterialSetDefaultElasticity (mWorld, mBallMaterialID, mFloorMaterialID, 0.2f);
	CollisionData* collDataBallFloor = new CollisionData(*collData);
	collDataBallFloor->material1 = mBallMaterialID;
	collDataBallFloor->material2 = mFloorMaterialID;
	NewtonMaterialSetCollisionCallback (mWorld, mBallMaterialID, mFloorMaterialID, collDataBallFloor, NULL, contactProcessCallback, NULL);

	// ball on net - middle elasticity
	NewtonMaterialSetDefaultElasticity (mWorld, mBallMaterialID, mNetMaterialID, 0.75f);

	// ball on invisibleBarrier - no collision
	CollisionData* collDataBallBarrier = new CollisionData(*collData);
	collDataBallBarrier->material1 = mBallMaterialID;
	collDataBallBarrier->material2 = mInvisibleBarrierID;
	NewtonMaterialSetCollisionCallback (mWorld, mBallMaterialID, mInvisibleBarrierID, collDataBallBarrier, NULL, contactProcessCallback, NULL);

	// blobb on invisibleBarrier - middle elasticity
	NewtonMaterialSetDefaultElasticity (mWorld, mBlobbMaterialID, mInvisibleBarrierID, 0.5f);
	CollisionData* collDataBlobbBarrier = new CollisionData(*collData);
	collDataBlobbBarrier->material1 = mBlobbMaterialID;
	collDataBlobbBarrier->material2 = mInvisibleBarrierID;
	NewtonMaterialSetCollisionCallback (mWorld, mBlobbMaterialID, mInvisibleBarrierID, collDataBlobbBarrier, NULL, contactProcessCallback, NULL);

	// set friction for  blobb on surrounding (no friction)
	NewtonMaterialSetDefaultFriction(mWorld, mBlobbMaterialID, mWallMaterialID, 0, 0);
	NewtonMaterialSetDefaultFriction(mWorld, mBlobbMaterialID, mFloorMaterialID, 0, 0);
	NewtonMaterialSetDefaultFriction(mWorld, mBlobbMaterialID, mNetMaterialID, 0, 0);
	NewtonMaterialSetDefaultFriction(mWorld, mBlobbMaterialID, mInvisibleBarrierID, 0, 0);

	// Ball on AI Trigger:
	NewtonMaterialSetCollisionCallback (mWorld, mBallMaterialID, mAiTriggerID, collData, NULL, BV3D::AI::AiCallback, NULL);

	// AI trigger shouldn't collide with blobbs
	NewtonMaterialSetDefaultCollidable(mWorld, mBlobbMaterialID, mAiTriggerID, 0);
}

/*
 * collision callback - called when bounding boxes of a blobb and the ball intersect
 */
int BV3D::Arena::blobbContactBeginCallback(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1) {
	CollisionData* collData = (CollisionData*)NewtonMaterialGetMaterialPairUserData(material);

	VRS::SharedObj* so = (VRS::SharedObj*)NewtonBodyGetUserData(body0);
	if (so->classNameVRS() == BV3D::Blobb::ClassNameVRS())
		collData->currentBlobb = (BV3D::Blobb*)so;
	else
		collData->currentBlobb = (BV3D::Blobb*)NewtonBodyGetUserData(body1);

	return 1;
}

/*
 * collision callback - called for every collision which is interesting for game logic
 */
int BV3D::Arena::contactProcessCallback(const NewtonMaterial* material, const NewtonContact* contact) {
	CollisionData* collData = (CollisionData*)NewtonMaterialGetMaterialPairUserData(material);
	BV3D::TEAM team;

	if (collData->material1 == collData->arena->getBallMaterialID() && collData->material2 == collData->arena->getBlobbMaterialID()) {
		NewtonWorldUnfreezeBody(collData->arena->getWorld(), collData->ball->getBody());
		BV3D::TEAM team = collData->currentBlobb->getTeam();

		// wait some frames after blobb touches ball before new touch gets counted
		if (collData->referee->getCurrentContacts(team) > 0 && (collData->game->getFrameCount() - collData->delayStartFrame < 20)) {
			collData->delayStartFrame = 0;
		}
		else {
			if (team == BV3D::TEAM1)
				collData->referee->ballOnBlobb(BV3D::TEAM1);
			else {
				collData->referee->ballOnBlobb(BV3D::TEAM2);
			}
			collData->delayStartFrame=collData->game->getFrameCount();
			collData->game->playSoundTouch();
		}
	}
	else if (collData->material1 == collData->arena->getBallMaterialID() && collData->material2 == collData->arena->getFloorMaterialID()) {
		if (collData->ball->getPosition()[0] < 0) {
			team = BV3D::TEAM1;
		}
		else {
			team = BV3D::TEAM2;
		}
		collData->referee->ballOnField(team);

		// for AI
		NewtonMaterialSetDefaultCollidable(collData->world, collData->arena->getBallMaterialID(), collData->arena->getAiTriggerID(), 1);
	}
	else if (collData->material1 == collData->arena->getBallMaterialID() && collData->material2 == collData->arena->getInvisibleBarrierID()) {
		// tell referee if ball passes under the net
		Vector pos = collData->ball->getPosition();
		if (pos[1] < BV3D::NET_HEIGHT/2) {
			if (pos[0] < 0)
				team = BV3D::TEAM1;
			else
				team = BV3D::TEAM2;
			collData->referee->ballOnField(team);
		}
		return 0;
	}
	return 1;
}
