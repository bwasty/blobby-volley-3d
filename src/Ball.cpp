/// Ball.cpp

#include "Ball.h"
#include "Arena.h"
#include <vrs/sg/scenething.h>
#include <vrs/matrix.h>
#include <vrs/opengl/shapematerialgl.h>
#include <vrs/sphere.h>
#include <vrs/vector.h>
#include <Newton.h>

BV3D::Ball::Ball(VRS::SO<BV3D::Arena> arena) {
	m_Arena = arena;
	m_Radius = 1.0;
	m_Body = 0;
	m_IsLocked = true;

	// create ball local scene
	m_Scene = new VRS::SceneThing();
	m_Scene->append(new VRS::ShapeMaterialGL(VRS::Color(1.0,1.0,0.0)));
	m_Scene->append(new VRS::Sphere(m_Radius));

	// physics setup
	NewtonWorld* world = m_Arena->getWorld();

	// create NewtonBody with a collision sphere
	NewtonCollision* collision = NewtonCreateSphere(world, (dFloat)m_Radius, (dFloat)m_Radius, (dFloat)m_Radius, NULL);
	m_Body = NewtonCreateBody(world, collision);
	NewtonReleaseCollision(world, collision);

	// set up mass matrix
	dFloat inertia = 2*1/*mass*/*(dFloat)(m_Radius * m_Radius) / 5; 
	NewtonBodySetMassMatrix(m_Body, 10 /*mass*/,inertia,inertia,inertia);

	NewtonBodySetUserData(m_Body, this);
	NewtonBodySetForceAndTorqueCallback (m_Body, applyForceAndTorqueCallback);
	NewtonBodySetMaterialGroupID(m_Body, NewtonMaterialGetDefaultGroupID(world));
	NewtonBodySetAutoFreeze (m_Body, 0);
	NewtonBodySetContinuousCollisionMode(m_Body, 0); // needed?
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
