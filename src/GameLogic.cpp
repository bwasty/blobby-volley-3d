#include <NxOgreContactPair.h>
#include <OGRE3DBody.h>
#include <OgreStringConverter.h>

#include "Constants.h"
#include "Application.h"
#include "Ball.h"
#include "Blobb.h"
#include "GameLogic.h"

using namespace Ogre;

void GameLogic::onContact(const NxOgre::ContactPair& contactPair) {
	NxOgre::RigidBody* otherObject;
	if (contactPair.mFirst == static_cast<const NxOgre::RigidBody*>(mApp->getBall()->getBody()) ) //TODO!!: upcast necessary? -> effective c++
		otherObject = contactPair.mSecond;
	else if (contactPair.mSecond == static_cast<const NxOgre::RigidBody*>((mApp->getBall()->getBody())) )
		otherObject = contactPair.mFirst;
	else
		mApp->addToConsole("Error: GameLogic::onContact() - neither object in contactPair is the ball");

	if (otherObject ==  static_cast<const NxOgre::RigidBody*>(mApp->getBlobb1()->getBody()) )
		ballContact(TEAM1);
	else if (otherObject ==  static_cast<const NxOgre::RigidBody*>(mApp->getBlobb2()->getBody()) )
		ballContact(TEAM2);
	else if (otherObject ==  static_cast<const NxOgre::SceneGeometry*>(mApp->getFloorSceneGeometry()) && mGameRunning ) 
		hitFloor();
}


void GameLogic::score(TEAM team) {
	int& scoringTeamScore = (team == TEAM1) ? mScoreTeam1 : mScoreTeam2;
	int& otherTeamScore = (team == TEAM1) ? mScoreTeam2 : mScoreTeam1;

	++scoringTeamScore;
	mApp->addToConsole(StringConverter::toString(mScoreTeam1).append(":").append(StringConverter::toString(mScoreTeam2)));

	int winningScore = mApp->getConfig().getSettingInt("winningScore");
	if (scoringTeamScore >= winningScore && (scoringTeamScore - otherTeamScore >= 2)) {
		//TODO!!!: end game (display winning message, menu? free movement?
		mApp->addToConsole("Game Over");
		return;
	}

	prepareNewServe(team);
	
}

bool GameLogic::canScore(TEAM team) {
	if (mRules == NEW_RULES || team == mCurrentlyServing)
		return true;
	return false;
}

//TODO!!!: ballContact() - add timer to discard quick consecutive contacts
void GameLogic::ballContact(TEAM team) {
	mApp->addToConsole("Blobb hits ball");
	if (team = mCurrentlyOnBall) {
		if (++mCurrentContacts > mApp->getConfig().getSettingInt("maxBallContacts") && canScore(team)) {
			score(team);
		}
	}
	else {
		mCurrentlyOnBall = team;
		mCurrentContacts = 0;
	}
}

void GameLogic::hitFloor() {
	// determine on whose side the ball hit the floor - the net is on the z-axis, so check x-coordinate is enough
	mApp->addToConsole("Ball hits floor");
	TEAM scoringTeam;
	if (mApp->getBall()->getBody()->getGlobalPosition()[0] > 0) // TEAM2's side
		scoringTeam = TEAM1;
	else
		scoringTeam = TEAM2;

	if (canScore(scoringTeam))
		score(scoringTeam);

}

void GameLogic::prepareNewServe(TEAM team) {
	//TODO!!!: implement newServe(TEAM team) - reset ball to right position after time, set mGameRunning to true again somewhere
	mCurrentlyServing = mCurrentlyOnBall = team;
	mCurrentContacts = 0;
	mGameRunning = false;
}