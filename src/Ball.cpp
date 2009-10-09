#include "Ball.h"

#include <OgreVector3.h>

//#include <NxOgre.h>
#include <OGRE3DRenderSystem.h>
#include <OGRE3DBody.h>

#include "Application.h"
#include "GameLogic.h"


using namespace Ogre;

Ball::Ball(Application *app, Ogre::Vector3 position) : mApp(app) {
	mBallBody = 0; // make sure it's 0, because otherwise reset() tries to destroy it
	reset(position);
}

void Ball::loadSettings() {
	mBallBody->setMass(mApp->getConfig().getSettingReal("ballMass")); 
}

void Ball::reset(Vector3 position) {
	// TODO!!!: Ball: arenaTransform scale not cosidererd
	if (mBallBody)
		mApp->getPhysicsRenderSystem()->destroyBody(mBallBody);

	Real ballRadius = mApp->getConfig().getSettingReal("BALL_RADIUS");

	NxOgre::Sphere* ballSphere = new NxOgre::Sphere(ballRadius);
	ballSphere->setMaterial(mApp->mBallPhysicsMaterial->getIdentifier());
	mBallBody = mApp->getPhysicsRenderSystem()->createBody(ballSphere, NxOgre::Vec3(position), "Ball.mesh");
	loadSettings();
	mBallBody->getSceneNode()->scale(Vector3(ballRadius / 1.7) * mApp->getConfig().getSettingReal("arenaScale"));

	// register collision callback
	mBallBody->setContactCallback(mApp->getGameLogic());

	mBallBody->putToSleep();
}


