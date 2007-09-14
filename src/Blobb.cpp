#include "Blobb.h"
#include "Arena.h"
#include "KeyboardControls.h"
#include "ModelOptimizer.h"
#include "Constants.h"

#include <vrs/lookat.h>
#include <vrs/disc.h>
#include <vrs/cache.h>
#include <vrs/sg/scenething.h>
#include <vrs/sg/inputevent.h>
#include <vrs/sg/canvas.h>
#include <vrs/opengl/shapematerialgl.h>
#include <Newton.h>

/*!
 \ctor
 \param arena specifies the arena in which the blobb is simulated
 */
	BV3D::Blobb::Blobb(VRS::SO<BV3D::Arena> arena, BV3D::TEAM team, VRS::SO<VRS::LookAt> lookAt) {
	mArena = arena;
	mTeam = team;
	mControls = 0;
	mLookAt = lookAt;
	mJumpAllowed = false;
	mBody = 0;
	mCollision = 0;
	mCurrentShape = 0;
	mDecreasing = false;
	mIsMoving = false;
	mForceAnimation = true;
	mStep = 1;
	mMaxJump = false;
	VRS::SO<ModelOptimizer> optimizer = new ModelOptimizer();

	// set up the blobb animation scenes
	mShapes = new VRS::Array<VRS::SO<VRS::SceneThing> >;
	mShapes->clear();

	mShapes->append(optimizer->get3dsObject(BV3D::MODELS_PATH + "blobb1.3ds", false, ModelOptimizer::NO_MATERIAL_NO_TEXTURES));
	mShapes->append(optimizer->get3dsObject(BV3D::MODELS_PATH + "blobb2.3ds", false, ModelOptimizer::NO_MATERIAL_NO_TEXTURES));
	mShapes->append(optimizer->get3dsObject(BV3D::MODELS_PATH + "blobb3.3ds", false, ModelOptimizer::NO_MATERIAL_NO_TEXTURES));
	mShapes->append(optimizer->get3dsObject(BV3D::MODELS_PATH + "blobb4.3ds", false, ModelOptimizer::NO_MATERIAL_NO_TEXTURES));
	mShapes->append(optimizer->get3dsObject(BV3D::MODELS_PATH + "blobb5.3ds", false, ModelOptimizer::NO_MATERIAL_NO_TEXTURES));

	mScene = new VRS::SceneThing();
	mBlobbScene = new VRS::SceneThing();
	mShadowScene = new VRS::SceneThing();

	mMaterial = new VRS::ShapeMaterialGL(VRS::Color(0.0,0.0,0.0,BV3D::BLOBB_ALPHA), VRS::Color(1.0),
		40.0, VRS::ShapeMaterialGL::AmbientAndDiffuse, VRS::Color(1.0), VRS::Color(0.5), VRS::Color(0.0), true);
	mBlobbScene->append(mMaterial);

	for(int i = 0; i < mNumShapes; i++)
		mBlobbScene->append(mShapes->getElement(i));

	mScene->append(mShadowScene = new VRS::SceneThing());
	mShadowScene->append(new VRS::ShapeMaterialGL(VRS::Color(0.0f,0.0f,0.0f,0.3f)));
	mShadowScene->append(new VRS::Cache(new VRS::Disc(SHADOW_HEIGHT,1.0f)));

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
		matrix[13] = BLOBB_SHAPE_DATA[shape][0]-0.1f;		// set vertical offset of lower sphere
		colSphere[0] = NewtonCreateSphere(world,
			BLOBB_SHAPE_DATA[shape][1],	// use lower horizontal radius
			BLOBB_SHAPE_DATA[shape][0],	// use lower vertical radius
			BLOBB_SHAPE_DATA[shape][1],	// use lower horizontal radius
			matrix);

		// create upper collision sphere
		matrix[13] = BLOBB_SHAPE_DATA[shape][2];	// set vertical offset of upper sphere
		colSphere[1] = NewtonCreateSphere(world,
			BLOBB_SHAPE_DATA[shape][3],	// use upper horizontal radius
			BLOBB_SHAPE_DATA[shape][4],	// use upper vertical radius
			BLOBB_SHAPE_DATA[shape][3],	// use upper horizontal radius
			matrix);

		mCollision[shape] = NewtonCreateCompoundCollision(world, 2, colSphere);
		NewtonReleaseCollision(world, colSphere[0]);
		NewtonReleaseCollision(world, colSphere[1]);
	}

	// create newton body with first compound collision object
	mBody = NewtonCreateBody(world, mCollision[0]);

	// set up mass matrix - inertia taken from Marble Frenzy because we didn't know a realistic value
	dFloat radius = 1.0;
	dFloat inertia = 2*1*(radius * radius) / 5;
	NewtonBodySetMassMatrix(mBody, 50,inertia,inertia,inertia);

	// attach two up-vector joints to prevent blobb from leaning, tipping over and spinning around the y-axis
	dFloat upVector[3] = {0.0,1.0,0.0};
	dFloat xVector[3] = {1.0,0.0,0.0};
	NewtonConstraintCreateUpVector(world,upVector,mBody);
	NewtonConstraintCreateUpVector(world,xVector,mBody);

	NewtonBodySetUserData(mBody, this);
	NewtonBodySetForceAndTorqueCallback (mBody, applyForceAndTorqueCallback);
	NewtonBodySetMaterialGroupID(mBody, mArena->getBlobbMaterialID());
	NewtonBodySetAutoFreeze (mBody, 0);
	NewtonWorldUnfreezeBody(world, mBody);

	// move body to blobb position
	setPosition(VRS::Vector(0.0,0.0,0.0));
}

/*!
 \dtor
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

/*!
 move blobb to specified position
 */
void BV3D::Blobb::setPosition(VRS::Vector position) {
	// relocate visual blobb
	VRS::Matrix vrsMatrix = VRS::Matrix::translation(position);
	mBlobbScene->setLocalMatrix(vrsMatrix);
	double scaling = (SHADOW_MAX_HEIGHT - position[1]) * BLOBB_SHAPE_DATA[mCurrentShape][1] / SHADOW_MAX_HEIGHT;
	mShadowScene->setLocalMatrix(VRS::Matrix(
		scaling, 0.0f, 0.0f, position[0],
		0.0f, scaling, 0.0f, SHADOW_HEIGHT,
		0.0f, 0.0f, scaling, position[2],
		0.0f, 0.0f, 0.0f, 1.0f)
	);

	// translate physical body
	dFloat newtonMatrix[16] = {1.0,0.0,0.0,0.0, 0.0,1.0,0.0,0.0, 0.0,0.0,1.0,0.0,
						(dFloat)position[0], (dFloat)position[1], (dFloat)position[2], 1.0};
	NewtonBodySetMatrix(mBody, newtonMatrix);
}

/**
 * set the blobb's color
 */
void BV3D::Blobb::setColor(VRS::Color color) {
	mMaterial->setPerVertexColor(color);
}

/**
 * returns the blobb's current color
 */
VRS::Color BV3D::Blobb::getColor() {
	return mMaterial->getPerVertexColor();
}

/**
 * Animates the blobb by switching the different blobb shapes on and off, depending on various circumstances,
 * e.g. if the blobb is moving or jumping.
 * At most one shape is active at any given moment.
 */
void BV3D::Blobb::updateShape(VRS::SO<VRS::Canvas> canvas) {
	mStep = ++mStep % mMaxStep;
	//animate blobb initially once and then if he is moving or to return to initial shape
	if ((mStep == 0) && ((mIsMoving) || (mForceAnimation) || (mCurrentShape != 0)))	{
		//if blobb is in the air, don't animate
		if ((mCurrentShape == 0) && (mBlobbScene->getLocalMatrix().element(1, 3) >= 0.2))
			return;
		mForceAnimation = false;
		for(int i = 0; i < mNumShapes; i++)	//switch off all shapes
			canvas->switchOff(mShapes->getElement(i));

		if (mDecreasing) {
			if (mCurrentShape == 1)
				mDecreasing = false;
			else if (mCurrentShape < 1)	{
				mDecreasing = false;
				++mCurrentShape;
				return;
			}
			canvas->switchOn(mShapes->getElement(--mCurrentShape));		// switch on new shape
			NewtonBodySetCollision(mBody, mCollision[mCurrentShape]);	// replace corresponding physical body
		}
		else {
			if (mCurrentShape == (mNumShapes - 2))
				mDecreasing = true;
			else if (mCurrentShape > (mNumShapes - 2)) {
				mDecreasing = true;
				--mCurrentShape;
				return;
			}
			canvas->switchOn(mShapes->getElement(++mCurrentShape));
			NewtonBodySetCollision(mBody, mCollision[mCurrentShape]);
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
	if(!mControls) {
		if(mMaxJump)
			return VRS::Vector(0.0,BV3D::BLOBB_JUMP_SPEED,0.0);
		else
			return VRS::Vector();
	}

	// create orientation vector for movement to the specified mLookAt object
	VRS::Vector orientation = mLookAt->getTo() - mLookAt->getFrom();	// get from-to vector
	orientation[1] = 0;												// map vector onto xz-plane
	orientation = orientation.normalized() * BV3D::BLOBB_MOVEMENT_STEP_DISTANCE;	// normalize vector and resize to step distance

	VRS::Vector requestedMovement = mControls->getRequestedMovement();

	// evaluate requests and setup movement vector according to mLookAt
	VRS::Vector movement = requestedMovement[0] * VRS::Vector(orientation[2], 0, -orientation[0])
			+ requestedMovement[2] * orientation;

	if(requestedMovement[1]) {
		if(mJumpAllowed)		// blobb may jump only if it is allowed
			movement += VRS::Vector(0.0,BV3D::BLOBB_JUMP_SPEED,0.0);
	}
	return movement;
}

/**
* updates physical and visual blobb (called every frame)
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
		dFloat gravitation[3] = {0.0f, (dFloat) (-BV3D::GRAVITY * BV3D::BLOBB_GRAVITY_MULTIPLIER * mass), 0.0f};
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

	if(newtonMatrix[13]>BV3D::BLOBB_MAX_JUMP_HEIGHT) {	// prevent blobb from jumping too high
		mJumpAllowed = false;
		mMaxJump = false;
	}
	else if(newtonMatrix[13]<0.2) {	// re-enable jumping when landed
		if (!mJumpAllowed)		//if Blobb was jumping high, then animate when landing again
			forceSingleAnimation();
		mJumpAllowed = true;
	}

	mBlobbScene->setLocalMatrix(vrsMatrix);
	double scaling = (SHADOW_MAX_HEIGHT - newtonMatrix[13]) * BLOBB_SHAPE_DATA[mCurrentShape][1] / SHADOW_MAX_HEIGHT;
	mShadowScene->setLocalMatrix(VRS::Matrix(
		scaling, 0.0f, 0.0f, newtonMatrix[12],
		0.0f, scaling, 0.0f, SHADOW_HEIGHT,
		0.0f, 0.0f, scaling, newtonMatrix[14],
		0.0f, 0.0f, 0.0f, 1.0f)
	);

	// Reset ball if it leaves its field half
	if (!mArena->getTeamBounds(mTeam).containsPoint(VRS::Vector(vrsMatrix.element(0,3), vrsMatrix.element(1,3)+BV3D::BLOBB_HEIGHT/2, vrsMatrix.element(2,3)))) {
		VRS::Vector newPos(mArena->getTeamBounds(mTeam).center());
		newPos[1] = 0.0;
		setPosition(newPos);
	}	
}

/**
* callback which notifies that a NewtonBody in the NewtonWorld has changed
* \param body is the changed NewtonBody
*/
void BV3D::Blobb::applyForceAndTorqueCallback(const NewtonBody* body) {
	VRS::SO<BV3D::Blobb> blobb = static_cast<BV3D::Blobb*>(NewtonBodyGetUserData(body));
	if (blobb)
		blobb->update();
}

/**
 * lets the blobb do a jump with maximum height
 */
void BV3D::Blobb::maxJump() {
	mMaxJump = true;
}

/**
 * force the blobb to do one animation
 */
void BV3D::Blobb::forceSingleAnimation()
{
	mForceAnimation = true;
}
