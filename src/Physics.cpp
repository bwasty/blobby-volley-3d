/// Physics.cpp

#include <Newton.h>
#include "Physics.h"

BV3D::Physics::Physics() {
	m_World = NewtonCreate(0,0);
}

BV3D::Physics::~Physics() {
	NewtonDestroy(m_World);
}

void BV3D::Physics::updateWorld(float timestep) {
	NewtonUpdate(m_World, (dFloat)timestep);
}

void BV3D::Physics::registerBall(VRS::SO<BV3D::Ball> ball) {

	dFloat radius = (dFloat)ball->getRadius();
	NewtonCollision* collision = NewtonCreateSphere(m_World, radius, radius, radius, NULL);
	NewtonBody* newtonBody = NewtonCreateBody(m_World, collision);
	NewtonReleaseCollision(m_World, collision);

	//NewtonBodySetMatrix(newtonBody, location);

	dFloat inertia = 2*1/*mass*/*(radius * radius) / 5; 
	NewtonBodySetMassMatrix(newtonBody, 10 /*mass*/,inertia,inertia,inertia);

	NewtonBodySetForceAndTorqueCallback (newtonBody, applyForceAndTorqueToBall);
	NewtonBodySetMaterialGroupID(newtonBody, NewtonMaterialGetDefaultGroupID(m_World));
	NewtonBodySetUserData(newtonBody, ball);
	NewtonBodySetAutoFreeze (newtonBody, 0);
	NewtonBodySetContinuousCollisionMode(newtonBody, 0); // needed?
	NewtonWorldUnfreezeBody(m_World, newtonBody);
}

void BV3D::Physics::applyForceAndTorqueToBall(const NewtonBody* body) {
	const float Gravity = 9.81f; //gravitative Beschleunigung
	const float torque_influence = 0.9f;	//Verhältnis zwischen Kraft und Torque; je höher, desto mehr Torque [0..1]
	dFloat Ixx, Iyy, Izz, mass;

	// TODO: use c++ cast
	BV3D::Ball* ball = (BV3D::Ball*)NewtonBodyGetUserData(body);

	if (ball) {
		//Gravitationskraft-Vektor berechnen und anwenden
		NewtonBodyGetMassMatrix(body, &mass, &Ixx, &Iyy, &Izz);
		dFloat gravitation[3] = {0.0f, -Gravity * mass, 0.0f};
		dFloat nullForce[3] = {0.0f, 0.0f, 0.0f};
		NewtonBodySetForce(body, nullForce);
		NewtonBodyAddForce(body, gravitation);

		dFloat location[16];
		NewtonBodyGetMatrix(body,location);
		VRS::Matrix matrix (
			location[0], location[1], location[2], location[3],
			location[4], location[5], location[6], location[7],
			location[8], location[9], location[10], location[11],
			location[12], location[13], location[14], location[15]);
		matrix.transpose();
		//Matrix auf den Ball anwenden
		ball->setTransformation(matrix);
	}
}
