/**
 * The Blobb class represents the players in BlobbyVolley3D
 * it is responsible for:
 * - creating(loading) and managing: the 3D shape of the Blobb
 * - forwarding InputEvents to its Controls
 * - managing the Controls orientation and movement distance
 * - moving the Blobb inside its assigned bounds
 * - updating the position based on input and physics
 */

#include "Blobb.h"
#include "Arena.h"
#include "KeyboardControls.h"
#include <vrs/so.h>
#include <vrs/sg/scenething.h>
#include <vrs/opengl/shapematerialgl.h>
#include <vrs/sg/inputevent.h>
#include <vrs/io/threedsreader.h>
#include <Newton.h>

BV3D::Blobb::Blobb(VRS::SO<BV3D::Arena> arena, BV3D::BV3D_TEAM team) {
	m_Arena = arena;
	m_Team = team;
	m_Controls = new BV3D::KeyboardControls();		// TODO: use as default value in constructor
	m_CtrlsOrientation = VRS::Vector(0.0,0.0,5.0);	// TODO: use as default value in constructor
	m_JumpAllowed = false;
	m_Body = 0;

	// set up the blobb local scene
	m_Scene = new VRS::SceneThing();
	m_Material = new VRS::ShapeMaterialGL(VRS::Color(1.0,0.0,0.0,0.4));
	m_Scene->append(m_Material);
	VRS::ThreeDSReader::setMaterialMode(VRS::ThreeDSReader::NO_MATERIAL);
	m_Scene->append(VRS::ThreeDSReader::readObject("blobb1.3ds"));	// TODO: exception handling

	// physics setup
	dFloat radius = 1.0;
	NewtonWorld* world = m_Arena->getWorld();

	// create NewtonBody with a collision sphere
	NewtonCollision* collision[2];
	dFloat matrix[16] = {1.0,0.0,0.0,0.0, 0.0,1.0,0.0,0.0, 0.0,0.0,1.0,0.0, 0.0,1.0,0.0,1.0};

	// use two sphere collision objects as physics body
	collision[0] = NewtonCreateSphere(world, radius, radius, radius, NULL);
	collision[1] = NewtonCreateSphere(world, radius, radius, radius, matrix);
	NewtonCollision* compoundCollision = NewtonCreateCompoundCollision(world, 2, collision);
	m_Body = NewtonCreateBody(world, compoundCollision);
	NewtonReleaseCollision(world, collision[0]);
	NewtonReleaseCollision(world, collision[1]);
	NewtonReleaseCollision(world, compoundCollision);

	// set up mass matrix
	dFloat inertia = 2*1*(radius * radius) / 5; 
	NewtonBodySetMassMatrix(m_Body, 10 ,inertia,inertia,inertia);

	// attach an up-vector joint prevent blobb from leaning and tipping over
	dFloat upVector[3] = {0.0,1.0,0.0};
	NewtonConstraintCreateUpVector(world,upVector,m_Body);

	NewtonBodySetUserData(m_Body, this);
	NewtonBodySetForceAndTorqueCallback (m_Body, applyForceAndTorqueCallback);
	NewtonBodySetMaterialGroupID(m_Body, m_Arena->getBlobbMaterialID());
	NewtonBodySetAutoFreeze (m_Body, 0);
	NewtonBodySetContinuousCollisionMode(m_Body, 0); // needed?
	NewtonWorldUnfreezeBody(world, m_Body);

	// move body to blobb position
	setPosition(VRS::Vector(0.0,0.0,0.0));
}

BV3D::Blobb::~Blobb() {
	if(m_Body)
		NewtonDestroyBody(m_Arena->getWorld(), m_Body);
}

void BV3D::Blobb::setCtrlsOrientation(VRS::Vector ctrlsOri) {
	// normalize new orientation vector and use previous step distance to scale it
	double dStepDistance = sqrt(m_CtrlsOrientation.dotProduct(m_CtrlsOrientation));
	m_CtrlsOrientation = ctrlsOri.normalized() * dStepDistance;
}

void BV3D::Blobb::setStepDistance(double distance) {
	// scale normalized orientation vector by new step distance
	m_CtrlsOrientation = m_CtrlsOrientation.normalized() * distance;
}

VRS::Vector BV3D::Blobb::getMovement() {
	VRS::Vector movement;
	char requests = m_Controls->getRequests();	// get controls' request bitfield

	// evaluate requests and setup movement vector accordingly
	if(Controls::isRequested(requests, Controls::FORWARD)) movement += m_CtrlsOrientation;
	if(Controls::isRequested(requests, Controls::BACKWARD)) movement -= m_CtrlsOrientation;
	if(Controls::isRequested(requests, Controls::RIGHT)) {
		movement += VRS::Vector(m_CtrlsOrientation[2], 0, -m_CtrlsOrientation[0]);
	}
	if(Controls::isRequested(requests, Controls::LEFT)) {
		movement += VRS::Vector(-m_CtrlsOrientation[2], 0, m_CtrlsOrientation[0]);
	}
	if(Controls::isRequested(requests, Controls::JUMP)) {
		if(m_JumpAllowed)
			movement += VRS::Vector(0.0,3.0,0.0);	// blobb may jump only if it is allowed
	}
	else
		m_JumpAllowed = false;	// if blobb does not jump in this frame it is not allowed to jump until landed again

	return movement;
}

void BV3D::Blobb::processInput(VRS::SO<VRS::InputEvent> ie) {
	m_Controls->processInput(ie);	// pass input events to blobb's controls
}

void BV3D::Blobb::setPosition(VRS::Vector position) {
	// relocate visual blobb
	VRS::Matrix vrsMatrix = VRS::Matrix::translation(position);
	m_Scene->setLocalMatrix(vrsMatrix);

	// translate physical body
	dFloat newtonMatrix[16] = {1.0,0.0,0.0,0.0, 0.0,1.0,0.0,0.0, 0.0,0.0,1.0,0.0,
						(dFloat)position[0], (dFloat)position[1], (dFloat)position[2], 1.0};
	NewtonBodySetMatrix(m_Body, newtonMatrix);
}

VRS::Color BV3D::Blobb::getColor() {
	return m_Material->getPerVertexColor();
}

void BV3D::Blobb::setColor(VRS::Color color) {
	m_Material->setPerVertexColor(color);
}

void BV3D::Blobb::applyForceAndTorqueCallback(const NewtonBody* body) {
	// TODO: use c++/vrs cast
	BV3D::Blobb* blobb = (BV3D::Blobb*)NewtonBodyGetUserData(body);
	if (blobb)
		blobb->update();
}

void BV3D::Blobb::update() {
	dFloat Ixx, Iyy, Izz, mass;

	// get current and requested movement
	VRS::Vector movement = getMovement();
	dFloat currentVelocity[3];
	NewtonBodyGetVelocity(m_Body, currentVelocity);

	// jump if requested (linear up-movement, ignoring current movement) or continue current movement
	dFloat move[3] = {(dFloat)movement[0], (movement[1]>0) ? (dFloat)movement[1] : currentVelocity[1], (dFloat)movement[2]};
	NewtonBodySetVelocity(m_Body, move);

	// clear any forces
	dFloat nullForce[3] = {0.0f, 0.0f, 0.0f};
	NewtonBodySetForce(m_Body, nullForce);

	// apply gravitational force (except when jumping -> linear up-movement)
	if(movement[1]<=0) {
		NewtonBodyGetMassMatrix(m_Body, &mass, &Ixx, &Iyy, &Izz);
		dFloat gravitation[3] = {0.0f, -m_Arena->getGravity() * mass, 0.0f};
		NewtonBodyAddForce(m_Body, gravitation);
	}

	// set up blobb matrix
	dFloat newtonMatrix[16];
	NewtonBodyGetMatrix(m_Body,newtonMatrix);
	VRS::Matrix vrsMatrix (
		newtonMatrix[0], newtonMatrix[4], newtonMatrix[8], newtonMatrix[12], 
		newtonMatrix[1], newtonMatrix[5], newtonMatrix[9], newtonMatrix[13], 
		newtonMatrix[2], newtonMatrix[6], newtonMatrix[10], newtonMatrix[14], 
		newtonMatrix[3], newtonMatrix[7], newtonMatrix[11], newtonMatrix[15]);

	if(newtonMatrix[13]>2.0)	// prevent blobb from jumping too high
		m_JumpAllowed = false;
	else if(newtonMatrix[13]<1.1)	// re-enable jumping when landed
		m_JumpAllowed = true;

	m_Scene->setLocalMatrix(vrsMatrix);
}
