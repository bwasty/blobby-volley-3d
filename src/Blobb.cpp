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
#include "ModelOptimizer.h"
#include "Constants.h"
#include <vrs/so.h>
#include <vrs/sg/scenething.h>
#include <vrs/opengl/shapematerialgl.h>
#include <vrs/sg/inputevent.h>
#include <vrs/io/threedsreader.h>
#include <vrs/io/wavefrontreader.h>
#include <vrs/io/vrml1reader.h>
#include <vrs/io/filedataresource.h>
#include <vrs/sg/canvas.h>
#include <vrs/lookat.h>
#include <vrs/disc.h>
#include <vrs/cache.h>
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
// data per blobb:
//	lower vertical offset/radius, lower horizontal radius
//	upper vertical offset, upper horizontal radius, upper vertical radius
dFloat colData[5][5] = {
	{1.0f+0.1f, 1.0f, 1.5f, 0.9f, 1.1f},
	{0.9f+0.1f, 1.01f, 1.35f, 0.9f, 1.1f},
	{0.8f+0.1f, 1.05f, 1.2f, 0.9f, 1.1f},
	{0.65f+0.1f, 1.1f, 1.1f, 0.9f, 1.0f},
	{0.5f+0.1f, 1.2f, 1.0f, 0.9f, 0.9f} };
/**
 * ctor
 * \param arena specifies the arena in which the blobb is simulated
 */
	BV3D::Blobb::Blobb(VRS::SO<BV3D::Arena> arena, BV3D::TEAM team, VRS::SO<VRS::LookAt> lookAt, bool isAIcontrolled) {
	mArena = arena;
	mTeam = team;
	mControls = new BV3D::KeyboardControls();		// TODO: use as default value in constructor
	mLookAt = lookAt;
	mStepDistance = 9.0;
	mJumpAllowed = false;
	mBody = 0;
	mCollision = 0;
	mCurrentShape = 0;
	mDecreasing = false;
	mIsMoving = false;
	mForceAnimation = true;
	mIsAIcontrolled = isAIcontrolled;
	mMaxJump = false;
	VRS::SO<ModelOptimizer> optimizer = new ModelOptimizer();

	// set up the blobb animation scenes
	mShapes = new VRS::Array<VRS::SO<VRS::SceneThing> >;
	mShapes->clear();

	mShapes->append(optimizer->get3dsModel(BV3D::threeDSPath + "blobb1.3ds", false, ModelOptimizer::NO_MATERIAL_NO_TEXTURES));
	mShapes->append(optimizer->get3dsModel(BV3D::threeDSPath + "blobb2.3ds", false, ModelOptimizer::NO_MATERIAL_NO_TEXTURES));
	mShapes->append(optimizer->get3dsModel(BV3D::threeDSPath + "blobb3.3ds", false, ModelOptimizer::NO_MATERIAL_NO_TEXTURES));
	mShapes->append(optimizer->get3dsModel(BV3D::threeDSPath + "blobb4.3ds", false, ModelOptimizer::NO_MATERIAL_NO_TEXTURES));
	mShapes->append(optimizer->get3dsModel(BV3D::threeDSPath + "blobb5.3ds", false, ModelOptimizer::NO_MATERIAL_NO_TEXTURES));

	mScene = new VRS::SceneThing();
	mBlobbScene = new VRS::SceneThing();
	mShadowScene = new VRS::SceneThing();

	mMaterial = new VRS::ShapeMaterialGL(VRS::Color(0.0,0.0,0.0,BV3D::blobbAlpha), VRS::Color(1.0),
		90.0, VRS::ShapeMaterialGL::AmbientAndDiffuse, VRS::Color(1.0), VRS::Color(0.5), VRS::Color(0.0), true);
	mBlobbScene->append(mMaterial);

	for(int i = 0; i < mNumShapes; i++)
		mBlobbScene->append(mShapes->getElement(i));

	mScene->append(mShadowScene = new VRS::SceneThing());
	mShadowScene->append(new VRS::ShapeMaterialGL(VRS::Color(0.0f,0.0f,0.0f,0.3f)));
	mShadowScene->append(new VRS::Cache(new VRS::Disc(shadowHeight,1.0f)));

	mScene->append(mBlobbScene);
	mScene->append(mShadowScene);

	// physics setup
	NewtonWorld* world = mArena->getWorld();
	dFloat matrix[16] = {1.0,0.0,0.0,0.0, 0.0,1.0,0.0,0.0, 0.0,0.0,1.0,0.0, 0.0,0.0,0.0,1.0};
	mCollision = new NewtonCollision*[5];	// compound collisions for blobb
	NewtonCollision* colSphere[2];			// two sphere collisions per compound collision

	// create one compound collision for each blobb?.3ds
	for(int shape=0; shape<5; shape++) {
		// create lower collision sphere
		matrix[13] = colData[shape][0]-0.1f;		// set vertical offset of lower sphere
		colSphere[0] = NewtonCreateSphere(world,
			colData[shape][1],	// use lower horizontal radius
			colData[shape][0],	// use lower vertical radius
			colData[shape][1],	// use lower horizontal radius
			matrix);

		// create upper collision sphere
		matrix[13] = colData[shape][2];	// set vertical offset of upper sphere
		colSphere[1] = NewtonCreateSphere(world,
			colData[shape][3],	// use upper horizontal radius
			colData[shape][4],	// use upper vertical radius
			colData[shape][3],	// use upper horizontal radius
			matrix);

		mCollision[shape] = NewtonCreateCompoundCollision(world, 2, colSphere);
		NewtonReleaseCollision(world, colSphere[0]);
		NewtonReleaseCollision(world, colSphere[1]);
	}

	// create newton body with first compound collision object
	mBody = NewtonCreateBody(world, mCollision[0]);

	// set up mass matrix
	dFloat radius = 1.0;
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
	//NewtonBodySetContinuousCollisionMode(mBody, 1); // needed?
	NewtonWorldUnfreezeBody(world, mBody);

	// move body to blobb position
	setPosition(VRS::Vector(0.0,0.0,0.0));
}

/**
 * dtor
 */
BV3D::Blobb::~Blobb() {
	if(mCollision) {
		for(int i=0;i<5;i++)
			NewtonReleaseCollision(mArena->getWorld(), mCollision[i]);
		delete[] mCollision;
	}
	if(mBody)
		NewtonDestroyBody(mArena->getWorld(), mBody);
}

/**
 * move blobb to specified position
 */
void BV3D::Blobb::setPosition(VRS::Vector position) {
	// relocate visual blobb
	VRS::Matrix vrsMatrix = VRS::Matrix::translation(position);
	mBlobbScene->setLocalMatrix(vrsMatrix);
	double scaling = (shadowMaxHeight - position[1]) * colData[mCurrentShape][1] / shadowMaxHeight;
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
	//printf("%f, %f, %f\n", position[0], position[1], position[2]);
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

BV3D::TEAM	BV3D::Blobb::getTeam() {
	return mTeam;
}

inline bool BV3D::Blobb::isMoving() {//not needed anymore?
	return mIsMoving;
}

void BV3D::Blobb::updateShape(VRS::SO<VRS::Canvas> canvas)
{
	//printf("%i\n", mCurrentShape);
	mStep = ++mStep % mMaxStep;
	//animate blobb initially once and then if he is moving or to return to initial shape
	if ((mStep == 0) && ((mIsMoving) || (mForceAnimation) || (mCurrentShape != 0)))
	{
		//if blobb is in the air, don't animate
		if ((mCurrentShape == 0) && (mBlobbScene->getLocalMatrix().element(1, 3) >= 0.2))
			return;
		mForceAnimation = false;
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
				return;
			}
			canvas->switchOn(mShapes->getElement(--mCurrentShape));
			NewtonBodySetCollision(mBody, mCollision[mCurrentShape]);	// replaces physical body
		}
		else
		{
			if (mCurrentShape == (mNumShapes - 2))
				mDecreasing = true;
			else if (mCurrentShape > (mNumShapes - 2))
			{
				mDecreasing = true;
				--mCurrentShape;
				return;
			}
			canvas->switchOn(mShapes->getElement(++mCurrentShape));
		}
	}
}

/**
 * processes user input for this blobb
 * \param ie is an input event that was explicitely forwarded from the input callback in BV3D::Game
 */
void BV3D::Blobb::processInput(VRS::SO<VRS::InputEvent> ie) {
	if(mControls)
		mControls->processInput(ie);	// pass input events to blobb's controls
}

/**
* evaluates the controls state and returns the movement vector for the current frame.
* this resets the controls state for the next frame.
*/
VRS::Vector BV3D::Blobb::getMovement() {
	if(!mControls) return VRS::Vector();

	// create orientation vector for movement to the specified mLookAt object
	VRS::Vector orientation = mLookAt->getTo() - mLookAt->getFrom();	// get from-to vector
	orientation[1] = 0;												// map vector onto xz-plane
	orientation = orientation.normalized() * mStepDistance;			// normalize vector and resize to step distance

	VRS::Vector requestedMovement = mControls->getRequestedMovement();

	// evaluate requests and setup movement vector according to mLookAt
	VRS::Vector movement = requestedMovement[0] * VRS::Vector(orientation[2], 0, -orientation[0])
			+ requestedMovement[2] * orientation;

	if(requestedMovement[1] || mMaxJump) {
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
		dFloat gravitation[3] = {0.0f, -mArena->getGravity()* 3.5f * mass, 0.0f};
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

	if(newtonMatrix[13]>2.0) {	// prevent blobb from jumping too high
		mJumpAllowed = false;
		mMaxJump = false;
	}
	else if(newtonMatrix[13]<0.2)	// re-enable jumping when landed
	{
		//if (!mJumpAllowed)		//if Blobb was jumping high, then animate when landing again
		//	forceSingleAnimation();
		mJumpAllowed = true;
	}
	mBlobbScene->setLocalMatrix(vrsMatrix);
	double scaling = (shadowMaxHeight - newtonMatrix[13]) * colData[mCurrentShape][1] / shadowMaxHeight;
	mShadowScene->setLocalMatrix(VRS::Matrix(
		scaling, 0.0f, 0.0f, newtonMatrix[12],
		0.0f, scaling, 0.0f, shadowHeight,
		0.0f, 0.0f, scaling, newtonMatrix[14],
		0.0f, 0.0f, 0.0f, 1.0f)
	);
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

void BV3D::Blobb::maxJump() {
	mMaxJump = true;
}

void BV3D::Blobb::forceSingleAnimation()
{
	mForceAnimation = true;
}