/// Ball.cpp

#include "Ball.h"
#include "Arena.h"
#include "ModelOptimizer.h"
#include "Constants.h"
#include <vrs/sg/scenething.h>
#include <vrs/matrix.h>
#include <vrs/io/threedsreader.h>
#include <vrs/io/wavefrontreader.h>
#include <vrs/io/filedataresource.h>
#include <vrs/opengl/shapematerialgl.h>
#include <vrs/color.h>
#include <vrs/vector.h>
#include <vrs/scaling.h>
#include <vrs/disc.h>
#include <vrs/cache.h>
#include <Newton.h>


BV3D::Ball::Ball(VRS::SO<BV3D::Arena> arena) {
	mArena = arena;
	mRadius = BV3D::ballRadius;
	mBody = 0;
	mIsLocked = true;
	VRS::SO<ModelOptimizer> optimizer = new ModelOptimizer();

	// create ball local scene
	mScene = new VRS::SceneThing();

		//radius of 3ds-ball: 1.7 -> need to scale by 'factor'
	double factor = BV3D::ballRadius / 1.7;
	mScene->append(mBallScene = optimizer->getWavefrontModel(BV3D::threeDSPath + "volleyball-colored.obj"));
	mBallScene->prepend(new VRS::Scaling(factor, factor, factor));

	mScene->append(mShadowScene = new VRS::SceneThing());
	mShadowScene->append(new VRS::ShapeMaterialGL(VRS::Color(0.0f,0.0f,0.0f,0.3f)));
	mShadowScene->append(new VRS::Cache(new VRS::Disc(shadowHeight,mRadius)));

	// physics setup
	NewtonWorld* world = mArena->getWorld();

	// create NewtonBody with a collision sphere
	NewtonCollision* collision = NewtonCreateSphere(world, (dFloat)mRadius, (dFloat)mRadius, (dFloat)mRadius, NULL);
	mBody = NewtonCreateBody(world, collision);
	NewtonBodySetMaterialGroupID(mBody, mArena->getBallMaterialID());
	NewtonReleaseCollision(world, collision);

	// set up mass matrix - inertia taken from Marble Frenzy because we didn't know a realistic value
	dFloat inertia = 2*4/*mass*/*(dFloat)(mRadius * mRadius) / 5; 
	NewtonBodySetMassMatrix(mBody, 4 /*mass*/,inertia,inertia,inertia);

	NewtonBodySetUserData(mBody, this);
	NewtonBodySetForceAndTorqueCallback (mBody, applyForceAndTorqueCallback);
	NewtonBodySetMaterialGroupID(mBody, mArena->getBallMaterialID());
	NewtonBodySetAutoFreeze (mBody, 0);
	//NewtonBodySetContinuousCollisionMode(mBody, 1); // needed?
	NewtonWorldUnfreezeBody(world, mBody);

	// move ball (and ball body!) to origin
	resetPosition(VRS::Vector(0.0,0.0,0.0));
}

BV3D::Ball::~Ball() {
	if(mBody)	// destroy ball body if it exists
		NewtonDestroyBody(mArena->getWorld(), mBody);
}

void BV3D::Ball::resetPosition(VRS::Vector& position) {
	// translate visual ball to specified position
	VRS::Matrix vrsMatrix = VRS::Matrix::translation(position);
	mBallScene->setLocalMatrix(vrsMatrix);
	double scaling = (shadowMaxHeight - position[1]) / shadowMaxHeight;
	mShadowScene->setLocalMatrix(VRS::Matrix(
		scaling, 0.0f, 0.0f, position[0],
		0.0f, scaling, 0.0f, shadowHeight,
		0.0f, 0.0f, scaling, position[2],
		0.0f, 0.0f, 0.0f, 1.0f)
	);

	// translate physical body
	dFloat newtonMatrix[16] = {1.0,0.0,0.0,0.0, 0.0,1.0,0.0,0.0, 0.0,0.0,1.0,0.0,
						(dFloat)position[0], (dFloat)position[1], (dFloat)position[2], 1.0};
	NewtonBodySetMatrix(mBody, newtonMatrix);
	dFloat nullVelocity[3] = {0.0,0.0,0.0};
	NewtonBodySetVelocity(mBody, nullVelocity);
	mIsLocked = true;	// lock the ball at the newly set position
}

VRS::Vector BV3D::Ball::getPosition() {
	return mBallScene->getLocalMatrix() * VRS::Vector(0.0,0.0,0.0);
}

void BV3D::Ball::applyForceAndTorqueCallback(const NewtonBody* body) {
	// TODO: use c++/vrs cast
	BV3D::Ball* ball = (BV3D::Ball*)NewtonBodyGetUserData(body);

	if (ball)
		ball->update();
}

void BV3D::Ball::update() {
	dFloat Ixx, Iyy, Izz, mass;

	// apply no forces until ball is unlocked by collision (external force)
	dFloat force[3];
	NewtonBodyGetForce(mBody, force);
	if(!mIsLocked || (force[0] != 0) || (force[1] != 0) || (force[2] != 0)) {
		mIsLocked = false;

		// apply gravitational force
		NewtonBodyGetMassMatrix(mBody, &mass, &Ixx, &Iyy, &Izz);
		dFloat gravitation[3] = {0.0f, -BV3D::gravity * mass, 0.0f};
		dFloat nullForce[3] = {0.0f, 0.0f, 0.0f};
		NewtonBodySetForce(mBody, nullForce);
		NewtonBodyAddForce(mBody, gravitation);

		// check for max speed and reduce if necessary
		dFloat vel[3];
		NewtonBodyGetVelocity(mBody, vel);
		VRS::Vector v(vel[0], vel[1], vel[2]);
		double length = v.abs();
		if (length > BV3D::maxBallVelocity) {
			v = v.normalized() * BV3D::maxBallVelocity;
			vel[0] = (dFloat)v[0];
			vel[1] = (dFloat)v[1];
			vel[2] = (dFloat)v[2];
			NewtonBodySetVelocity(mBody, vel);
		}

		// set up matrix for visual ball
		dFloat newtonMatrix[16];
		NewtonBodyGetMatrix(mBody, newtonMatrix);
		VRS::Matrix vrsMatrix (
			newtonMatrix[0], newtonMatrix[4], newtonMatrix[8], newtonMatrix[12], 
			newtonMatrix[1], newtonMatrix[5], newtonMatrix[9], newtonMatrix[13], 
			newtonMatrix[2], newtonMatrix[6], newtonMatrix[10], newtonMatrix[14], 
			newtonMatrix[3], newtonMatrix[7], newtonMatrix[11], newtonMatrix[15]);

		// apply matrix to visual ball
		mBallScene->setLocalMatrix(vrsMatrix);
		double scaling = (shadowMaxHeight - newtonMatrix[13]) / shadowMaxHeight;
		mShadowScene->setLocalMatrix(VRS::Matrix(
			scaling, 0.0f, 0.0f, newtonMatrix[12],
			0.0f, scaling, 0.0f, shadowHeight,
			0.0f, 0.0f, scaling, newtonMatrix[14],
			0.0f, 0.0f, 0.0f, 1.0f)
		);
	}
}
