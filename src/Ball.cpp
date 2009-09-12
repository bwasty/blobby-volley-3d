#include <OgreVector3.h>

#include <NxOgreSphere.h>
#include <OGRE3DRenderSystem.h>
#include <OGRE3DBody.h>

#include "Application.h"
#include "Ball.h"


using namespace Ogre;

Ball::Ball(Application *app, Ogre::Vector3 position) : mApp(app), mStartPosition(position) {
	Real ballRadius = mApp->getConfig().getSettingReal("BALL_RADIUS");

	NxOgre::Sphere* ballSphere = new NxOgre::Sphere(ballRadius);
	ballSphere->setMaterial(mApp->mBallPhysicsMaterial->getIdentifier());
	mBallBody = mApp->getPhysicsRenderSystem()->createBody(ballSphere, NxOgre::Real3(position.x, position.y, position.z), "Ball.mesh");
	mBallBody->setMass(0.1);
	mBallBody->getSceneNode()->scale(Vector3(ballRadius / 1.7));
	mBallBody->putToSleep();
}

void Ball::reset() {
	Vector3& p = mStartPosition;
	mBallBody->setGlobalPosition(NxOgre::Real3(p.x, p.y, p.z));
	mBallBody->putToSleep();
}