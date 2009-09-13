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

//TODO!!!: GameLogic:Score() - rename to error() and check for old/new rules (who's serving if old...)??
//TODO!!!!: check Beachvolleyball rules - 21 points, 16x8m...
void GameLogic::score(TEAM team) {
	int& scoringTeamScore = (team == TEAM1) ? mScoreTeam1 : mScoreTeam2;
	int& otherTeamScore = (team == TEAM1) ? mScoreTeam2 : mScoreTeam1;

	++scoringTeamScore;

	int winningScore = mApp->getConfig().getSettingInt("winningScore");
	if (scoringTeamScore >= winningScore && (scoringTeamScore - otherTeamScore >= 2)) {
		//TODO!!: end game
		mApp->addToConsole("Game Over");
	}

}