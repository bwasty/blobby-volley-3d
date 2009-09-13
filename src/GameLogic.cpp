#include <NxOgreContactPair.h>
#include <OGRE3DBody.h>

#include "Constants.h"
#include "Application.h"
#include "Ball.h"
#include "Blobb.h"
#include "GameLogic.h"

void GameLogic::onContact(const NxOgre::ContactPair& contactPair) {
	NxOgre::RigidBody* otherObject;
	if (contactPair.mFirst == static_cast<const NxOgre::RigidBody*>(mApp->getBall()->getBody()) ) //TODO!!: upcast necessary? -> effective c++
		otherObject = contactPair.mSecond;
	else if (contactPair.mSecond == static_cast<const NxOgre::RigidBody*>((mApp->getBall()->getBody())) )
		otherObject = contactPair.mFirst;
	else
		mApp->addToConsole("Error: GameLogic::onContact() - neither object in contactPair is the ball");

	if (otherObject ==  static_cast<const NxOgre::RigidBody*>(mApp->getBlobb1()->getBody()) )
		mApp->addToConsole("Ball collided with Blobb1");
	else if (otherObject ==  static_cast<const NxOgre::RigidBody*>(mApp->getBlobb2()->getBody()) )
		mApp->addToConsole("Ball collided with Blobb2");
	else if (otherObject ==  static_cast<const NxOgre::SceneGeometry*>(mApp->getFloorSceneGeometry()) )
		mApp->addToConsole("Ball collided with Floor");
	//else if (otherObject ==  static_cast<const NxOgre::SceneGeometry*>(mApp->getWallsSceneGeometry()) )
	//	mApp->addToConsole("Ball collided with Wall");
}