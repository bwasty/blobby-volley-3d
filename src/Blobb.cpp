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
#include <vrs/sg/canvas.h>
#include <Newton.h>

/*3ds-blobb measures: 
		blobb1.3ds:
			height: 2.6		lower radius: 1.0	lower center: 1.0
							upper radius: 0.9	upper center: 1.5)
		blobb2.3ds:
			height: 2.45	lower radius: 1.01	lower center: 0.9
							upper radius: 0.9	upper center: 1.35
		blobb2.3ds:
			height: 2.3		lower radius: 1.05	lower center: 0.8
							upper radius: 0.9	upper center: 1.2
		blobb2.3ds:
			height: 2.1		lower radius: 1.1	lower center: 0.65
							upper radius: 0.9	upper center: 1.1
		blobb2.3ds:
			height: 1.9		lower radius: 1.2	lower center: 0.5
							upper radius: 0.9	upper center: 1.0
	*/

/**
 * ctor
 * \param arena specifies the arena in which the blobb is simulated
 */
BV3D::Blobb::Blobb(VRS::SO<BV3D::Arena> arena, BV3D::BV3D_TEAM team) {
	mArena = arena;
	mTeam = team;
	mControls = new BV3D::KeyboardControls();		// TODO: use as default value in constructor
	mCtrlsOrientation = VRS::Vector(0.0,0.0,5.0);	// TODO: use as default value in constructor
	mJumpAllowed = false;
	mBody = 0;
	mCurrentShape = 0;
	mDecreasing = false;
	mIsMoving = false;
	mInit = true;
	
	// set up the blobb animation scenes
	mShapes = new VRS::Array<VRS::SO<VRS::SceneThing>>;
	mShapes->clear();
	VRS::ThreeDSReader::setMaterialMode(VRS::ThreeDSReader::NO_MATERIAL);
	
	mShapes->append(VRS::ThreeDSReader::readObject(BV3D::threeDSPath + "blobb1.3ds"));	// TODO: exception handling
	mShapes->append(VRS::ThreeDSReader::readObject(BV3D::threeDSPath + "blobb2.3ds"));
	mShapes->append(VRS::ThreeDSReader::readObject(BV3D::threeDSPath + "blobb3.3ds"));
	mShapes->append(VRS::ThreeDSReader::readObject(BV3D::threeDSPath + "blobb4.3ds"));
	mShapes->append(VRS::ThreeDSReader::readObject(BV3D::threeDSPath + "blobb5.3ds"));

	// set blobb local scene
	mScene = new VRS::SceneThing();
	mMaterial = new VRS::ShapeMaterialGL(VRS::Color(1.0,0.0,0.0,BV3D::blobbAlpha), VRS::Color(0.5), 
		4.0, VRS::ShapeMaterialGL::AmbientAndDiffuse, VRS::Color(1.0), VRS::Color(0.5), VRS::Color(0.0), true);
	mScene->append(mMaterial);
	/*mBlobbShape = mShapes->getElement(mCurrentShape);
	mScene->append(mBlobbShape);*/
	for(int i = 0; i < mNumShapes; i++)
		mScene->append(mShapes->getElement(i));
	
	// physics setup

	dFloat radius = 1.05;
	NewtonWorld* world = mArena->getWorld();

	// create NewtonBody with a collision sphere
	NewtonCollision* collision[2];
	dFloat matrix[16] = {1.0,0.0,0.0,0.0, 0.0,1.0,0.0,0.0, 0.0,0.0,1.0,0.0, 0.0,1.0,0.0,1.0};

	// use two sphere collision objects as physics body
	collision[0] = NewtonCreateSphere(world, radius, radius, radius, matrix);
	matrix[13] = 1.9;
	collision[1] = NewtonCreateSphere(world, 0.7, 0.7, 0.7, matrix);
	NewtonCollision* compoundCollision = NewtonCreateCompoundCollision(world, 2, collision);
	mBody = NewtonCreateBody(world, compoundCollision);
	NewtonReleaseCollision(world, collision[0]);
	NewtonReleaseCollision(world, collision[1]);
	NewtonReleaseCollision(world, compoundCollision);

	// set up mass matrix
	dFloat inertia = 2*1*(radius * radius) / 5; 
	NewtonBodySetMassMatrix(mBody, 50 ,inertia,inertia,inertia);

	// attach an up-vector joint prevent blobb from leaning and tipping over
	dFloat upVector[3] = {0.0,1.0,0.0};
	dFloat xVector[3] = {1.0,0.0,0.0};
	NewtonConstraintCreateUpVector(world,upVector,mBody);
	NewtonConstraintCreateUpVector(world,xVector,mBody);

	NewtonBodySetUserData(mBody, this);
	NewtonBodySetForceAndTorqueCallback (mBody, applyForceAndTorqueCallback);
	NewtonBodySetMaterialGroupID(mBody, mArena->getBlobbMaterialID());
	NewtonBodySetAutoFreeze (mBody, 0);
	//NewtonBodySetContinuousCollisionMode(mBody, 0); // needed?
	NewtonWorldUnfreezeBody(world, mBody);

	// move body to blobb position
	setPosition(VRS::Vector(0.0,0.0,0.0));

	setStepDistance(9);
}

/**
 * dtor
 */
BV3D::Blobb::~Blobb() {
	if(mBody)
		NewtonDestroyBody(mArena->getWorld(), mBody);
}

/**
 * move blobb to specified position
 */
void BV3D::Blobb::setPosition(VRS::Vector position) {
	// relocate visual blobb
	VRS::Matrix vrsMatrix = VRS::Matrix::translation(position);
	mScene->setLocalMatrix(vrsMatrix);

	// translate physical body
	dFloat newtonMatrix[16] = {1.0,0.0,0.0,0.0, 0.0,1.0,0.0,0.0, 0.0,0.0,1.0,0.0,
						(dFloat)position[0], (dFloat)position[1], (dFloat)position[2], 1.0};
	NewtonBodySetMatrix(mBody, newtonMatrix);
}

/**
 * sets the orientation of the controls
 * \param ctrlsOri is the direction the blobb moves to when controlling forward
 */
void BV3D::Blobb::setCtrlsOrientation(VRS::Vector ctrlsOri) {
	// normalize new orientation vector and use previous step distance to scale it
	double dStepDistance = sqrt(mCtrlsOrientation.dotProduct(mCtrlsOrientation));
	mCtrlsOrientation = ctrlsOri.normalized() * dStepDistance;
}

/**
 * sets the distance the blobb moves with one step
 */
void BV3D::Blobb::setStepDistance(double distance) {
	// scale normalized orientation vector by new step distance
	mCtrlsOrientation = mCtrlsOrientation.normalized() * distance;
}

/**
 * assigns the controls for controlling the blobb
 */
void BV3D::Blobb::setControls(VRS::SO<BV3D::Controls> controls) {
	mControls = controls;
}

/**
 * set the blobb's color
 */
void BV3D::Blobb::setColor(VRS::Color color) {
	mMaterial->setPerVertexColor(color);
}

/**
 * returns the current controls for this blobb
 */
inline VRS::SO<BV3D::Controls> BV3D::Blobb::getControls() {
	return mControls;
}

/**
 * returns the blobb local scene
 */
VRS::SO<VRS::SceneThing> BV3D::Blobb::getScene() {
	return mScene;
}

/**
 * returns the blobb's current color
 */
inline VRS::Color BV3D::Blobb::getColor() {
	return mMaterial->getPerVertexColor();
}

BV3D::BV3D_TEAM	BV3D::Blobb::getTeam() {
	return mTeam;
}

inline bool BV3D::Blobb::isMoving() {//not needed anymore?
	return mIsMoving;
}

VRS::SO<VRS::SceneThing> BV3D::Blobb::updateShape(VRS::SO<VRS::Canvas> canvas)
{
	//printf("%i\n", mCurrentShape);
	mStep = ++mStep % mMaxStep;
	//animate blobb initially once and then if he is moving or to return to initial shape
	if ((mStep == 0) && ((mIsMoving) || (mInit) || (mCurrentShape != 0)))
	{
		//if blobb is in the air, don't animate
		if ((mCurrentShape == 0) && (mScene->getLocalMatrix().element(1, 3) >= 0.2))
			return mScene;
		mInit = false;
		for(int i = 0; i < mNumShapes; i++)
			canvas->switchOff(mShapes->getElement(i));

		if (mDecreasing)
		{
			if (mCurrentShape == 1)
				mDecreasing = false;
			else if (mCurrentShape < 1)
			{
				mDecreasing = false;
				++mCurrentShape;
				return mScene;
			}
			canvas->switchOn(mShapes->getElement(--mCurrentShape));
			/*mScene->remove(mBlobbShape);
			mBlobbShape = mShapes->getElement(--mCurrentShape);
			mScene->append(mBlobbShape);*/
		}
		else
		{
			if (mCurrentShape == (mNumShapes - 2))
				mDecreasing = true;
			else if (mCurrentShape > (mNumShapes - 2))
			{
				mDecreasing = true;
				--mCurrentShape;
				return mScene;
			}
			canvas->switchOn(mShapes->getElement(++mCurrentShape));
			/*mScene->remove(mBlobbShape);
			mBlobbShape = mShapes->getElement(++mCurrentShape);
			mScene->append(mBlobbShape);*/
		}
		return mScene;
	}
}

/**
 * processes user input for this blobb
 * \param ie is an input event that was explicitely forwarded from the input callback in BV3D::Game
 */
void BV3D::Blobb::processInput(VRS::SO<VRS::InputEvent> ie) {
	mControls->processInput(ie);	// pass input events to blobb's controls
}

/**
* evaluates the controls state and returns the movement vector for the current frame.
* this resets the controls state for the next frame.
*/
VRS::Vector BV3D::Blobb::getMovement() {
	VRS::Vector movement;
	char requests = mControls->getRequests();	// get controls' request bitfield

	// evaluate requests and setup movement vector accordingly
	if(Controls::isRequested(requests, Controls::FORWARD)) movement += mCtrlsOrientation;
	if(Controls::isRequested(requests, Controls::BACKWARD)) movement -= mCtrlsOrientation;
	if(Controls::isRequested(requests, Controls::RIGHT)) {
		movement += VRS::Vector(mCtrlsOrientation[2], 0, -mCtrlsOrientation[0]);
	}
	if(Controls::isRequested(requests, Controls::LEFT)) {
		movement += VRS::Vector(-mCtrlsOrientation[2], 0, mCtrlsOrientation[0]);
	}
	if(Controls::isRequested(requests, Controls::JUMP)) {
		if(mJumpAllowed)
			movement += VRS::Vector(0.0,14.0,0.0);	// blobb may jump only if it is allowed
	}
	else
		mJumpAllowed = false;	// if blobb does not jump in this frame it is not allowed to jump until landed again

	return movement;
}

/**
* updates physical and visual blobb
*/
void BV3D::Blobb::update() {
	dFloat Ixx, Iyy, Izz, mass;

	// get current and requested movement
	VRS::Vector movement = getMovement();
	dFloat currentVelocity[3];
	NewtonBodyGetVelocity(mBody, currentVelocity);

	// jump if requested (linear up-movement, ignoring current movement) or continue current movement
	dFloat move[3] = {(dFloat)movement[0], (movement[1]>0) ? (dFloat)movement[1] : currentVelocity[1], (dFloat)movement[2]};
	NewtonBodySetVelocity(mBody, move);

	// clear any forces
	dFloat nullForce[3] = {0.0f, 0.0f, 0.0f};
	NewtonBodySetForce(mBody, nullForce);

	// apply gravitational force (except when jumping -> linear up-movement)
	if(movement[1]<=0) {
		NewtonBodyGetMassMatrix(mBody, &mass, &Ixx, &Iyy, &Izz);
		dFloat gravitation[3] = {0.0f, -mArena->getGravity()* 3.5 * mass, 0.0f};
		NewtonBodyAddForce(mBody, gravitation);
	}

	//set moving flag when moving
	if ((movement[0] != 0) || (movement[2] != 0))
		mIsMoving = true;
	else
		mIsMoving = false;

	// set up blobb matrix
	dFloat newtonMatrix[16];
	NewtonBodyGetMatrix(mBody,newtonMatrix);
	VRS::Matrix vrsMatrix (
		newtonMatrix[0], newtonMatrix[4], newtonMatrix[8], newtonMatrix[12], 
		newtonMatrix[1], newtonMatrix[5], newtonMatrix[9], newtonMatrix[13], 
		newtonMatrix[2], newtonMatrix[6], newtonMatrix[10], newtonMatrix[14], 
		newtonMatrix[3], newtonMatrix[7], newtonMatrix[11], newtonMatrix[15]);

	if(newtonMatrix[13]>2.0)	// prevent blobb from jumping too high
		mJumpAllowed = false;
	else if(newtonMatrix[13]<0.2)	// re-enable jumping when landed
		mJumpAllowed = true;
	mScene->setLocalMatrix(vrsMatrix);
}

/**
* callback which notifies that a NewtonBody in the NewtonWorld has changed
* \param body is the changed NewtonBody
*/
void BV3D::Blobb::applyForceAndTorqueCallback(const NewtonBody* body) {
	// TODO: use c++/vrs cast
	BV3D::Blobb* blobb = (BV3D::Blobb*)NewtonBodyGetUserData(body);
	if (blobb)
		blobb->update();
}
