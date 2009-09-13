#include <OgreVector3.h>

#include <NxOgreSphere.h>
#include <OGRE3DRenderSystem.h>
#include <OGRE3DBody.h>

#include "Application.h"
#include "Ball.h"
#include "GameLogic.h"


using namespace Ogre;

Ball::Ball(Application *app, Ogre::Vector3 position) : mApp(app), mStartPosition(position) {
	Real ballRadius = mApp->getConfig().getSettingReal("BALL_RADIUS");

	NxOgre::Sphere* ballSphere = new NxOgre::Sphere(ballRadius);
	ballSphere->setMaterial(mApp->mBallPhysicsMaterial->getIdentifier());
	mBallBody = mApp->getPhysicsRenderSystem()->createBody(ballSphere, NxOgre::Real3(), "Ball.mesh");
	loadSettings();
	mBallBody->getSceneNode()->scale(Vector3(ballRadius / 1.7));

	// register collision callback
	mBallBody->setContactCallback(mApp->mGameLogic);

	reset();
}

void Ball::loadSettings() {
	mBallBody->setMass(mApp->getConfig().getSettingReal("ballMass")); 
}

void Ball::reset() {
	Vector3& p = mStartPosition;
	mBallBody->setGlobalPosition(NxOgre::Real3(p.x, p.y, p.z));
	mBallBody->putToSleep();
}

//Ogre::Vector3 Ball::getPosition() {
//	return mBallBody->getGlobalPosition();
//}
