/// Ball.cpp

#include "Ball.h"
#include "Arena.h"
#include "ModelOptimizer.h"
//#include "Constants.h"
#include <vrs/sg/scenething.h>
#include <vrs/matrix.h>
#include <vrs/io/threedsreader.h>
#include <vrs/io/wavefrontreader.h>
#include <vrs/io/filedataresource.h>
#include <vrs/opengl/shapematerialgl.h>
#include <vrs/color.h>
#include <vrs/vector.h>
#include <vrs/scaling.h>
#include <Newton.h>

BV3D::Ball::Ball(VRS::SO<BV3D::Arena> arena) {
	m_Arena = arena;
	m_Radius = BV3D::ballRadius;
	m_Body = 0;
	m_IsLocked = true;
	VRS::SO<ModelOptimizer> optimizer = new ModelOptimizer();

	// create ball local scene
	m_Scene = new VRS::SceneThing();

	// load ball model
	//radius of 3ds-ball: 1.7 -> need to scale by 'factor'
	double factor = BV3D::ballRadius / 1.7;
	/*VRS::ThreeDSReader::setMaterialMode(VRS::ThreeDSReader::COMPLETE_MATERIAL);
	m_Scene->append(new VRS::Scaling(factor, factor, factor));
	m_Scene->append(VRS::ThreeDSReader::readObject(BV3D::threeDSPath + "volleyball-colored.3ds"));*/	// TODO: exception handling

	/*VRS::ID id = VRS::ID("ball");
	VRS::WavefrontReader reader = VRS::WavefrontReader();*/
	/*VRS::SO<VRS::ShapeMaterialGL>	mMaterial = new VRS::ShapeMaterialGL(VRS::Color(1.0, 1.0, 1.0, 1.0), VRS::Color(0.5), 
		115.0, VRS::ShapeMaterialGL::AmbientAndDiffuse, VRS::Color(1.0));*/
	/*VRS::SO<VRS::FileDataResource> file = new VRS::FileDataResource(BV3D::threeDSPath + "volleyball-colored.obj");*/

	m_Scene->append(new VRS::Scaling(factor, factor, factor));
	m_Scene->append(optimizer->getWavefrontModel(BV3D::threeDSPath + "volleyball-colored.obj"));

	// physics setup
	NewtonWorld* world = m_Arena->getWorld();

	// create NewtonBody with a collision sphere
	NewtonCollision* collision = NewtonCreateSphere(world, (dFloat)m_Radius, (dFloat)m_Radius, (dFloat)m_Radius, NULL);
	m_Body = NewtonCreateBody(world, collision);
	NewtonBodySetMaterialGroupID(m_Body, m_Arena->getBallMaterialID());
	NewtonReleaseCollision(world, collision);

	// set up mass matrix
	dFloat inertia = 2*0.5f/*mass*/*(dFloat)(m_Radius * m_Radius) / 5; 
	NewtonBodySetMassMatrix(m_Body, 4 /*mass*/,inertia,inertia,inertia);

	NewtonBodySetUserData(m_Body, this);
	NewtonBodySetForceAndTorqueCallback (m_Body, applyForceAndTorqueCallback);
	NewtonBodySetMaterialGroupID(m_Body, m_Arena->getBallMaterialID());
	NewtonBodySetAutoFreeze (m_Body, 0);
	//NewtonBodySetContinuousCollisionMode(m_Body, 1); // needed?
	NewtonWorldUnfreezeBody(world, m_Body);

	// move ball (and ball body!) to origin
	resetPosition(VRS::Vector(0.0,0.0,0.0));
}

BV3D::Ball::~Ball() {
	if(m_Body)	// destroy ball body if it exists
		NewtonDestroyBody(m_Arena->getWorld(), m_Body);
}

void BV3D::Ball::resetPosition(VRS::Vector& position) {
	// translate visual ball to specified position
	VRS::Matrix vrsMatrix = VRS::Matrix::translation(position);
	m_Scene->setLocalMatrix(vrsMatrix);

	// translate physical body
	dFloat newtonMatrix[16] = {1.0,0.0,0.0,0.0, 0.0,1.0,0.0,0.0, 0.0,0.0,1.0,0.0,
						(dFloat)position[0], (dFloat)position[1], (dFloat)position[2], 1.0};
	NewtonBodySetMatrix(m_Body, newtonMatrix);
	dFloat nullVelocity[3] = {0.0,0.0,0.0};
	NewtonBodySetVelocity(m_Body, nullVelocity);
	m_IsLocked = true;	// lock the ball at the newly set position
}

VRS::Vector BV3D::Ball::getPosition() {
	return m_Scene->getLocalMatrix() * VRS::Vector(0.0,0.0,0.0);
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
	NewtonBodyGetForce(m_Body, force);
	if(!m_IsLocked || (force[0] != 0) || (force[1] != 0) || (force[2] != 0)) {
		m_IsLocked = false;

		// apply gravitational force
		NewtonBodyGetMassMatrix(m_Body, &mass, &Ixx, &Iyy, &Izz);
		dFloat gravitation[3] = {0.0f, -m_Arena->getGravity() * mass, 0.0f};
		dFloat nullForce[3] = {0.0f, 0.0f, 0.0f};
		NewtonBodySetForce(m_Body, nullForce);
		NewtonBodyAddForce(m_Body, gravitation);

		// check for max speed
		dFloat vel[3];
		NewtonBodyGetVelocity(m_Body, vel);
		VRS::Vector v(vel[0], vel[1], vel[2]);
		//printf("%f\n", v.abs());
		double length = v.abs();
		if (length > BV3D::maxBallVelocity) {
			v = v.normalized() * BV3D::maxBallVelocity;
			vel[0] = (dFloat)v[0];
			vel[1] = (dFloat)v[1];
			vel[2] = (dFloat)v[2];
			NewtonBodySetVelocity(m_Body, vel);
			//printf("%f\n", v.abs());
		}

		

		// set up matrix for visual ball
		dFloat newtonMatrix[16];
		NewtonBodyGetMatrix(m_Body, newtonMatrix);
		VRS::Matrix vrsMatrix (
			newtonMatrix[0], newtonMatrix[4], newtonMatrix[8], newtonMatrix[12], 
			newtonMatrix[1], newtonMatrix[5], newtonMatrix[9], newtonMatrix[13], 
			newtonMatrix[2], newtonMatrix[6], newtonMatrix[10], newtonMatrix[14], 
			newtonMatrix[3], newtonMatrix[7], newtonMatrix[11], newtonMatrix[15]);

		// apply matrix to visual ball
		m_Scene->setLocalMatrix(vrsMatrix);
	}
}
