#include <NxOgreContactPair.h>
#include <OGRE3DBody.h>
#include <OgreStringConverter.h>

#include "Constants.h"
#include "Application.h"
#include "Ball.h"
#include "Blobb.h"
#include "GameLogic.h"

using namespace Ogre;

// TODO!!!: GameLogic - make rules changeable (configurable?)
GameLogic::GameLogic(Application* app) : mApp(app), mScoreTeam1(0), mScoreTeam2(0), mCurrentlyServing(TEAM1), mCurrentlyOnBall(TEAM1),
										 mCurrentContacts(0), mRules(NEW_RULES), mBallInGame(true), mBlobbContactTimer(Ogre::Timer()), mBlobbContactTimerActive(false),
								         mDelayedNewServeTimer(Ogre::Timer()), mDelayedNewServeTimerActive(false), mBallInGameNextFrame(false)
{
	mServePointTeam1 = Vector3(-mApp->getConfig().getSettingVector3("ARENA_EXTENT").x/4,6.0,0.0);
	mServePointTeam2 = mServePointTeam1 * Vector3(-1,1,1);
};


void GameLogic::onContact(const NxOgre::ContactPair& contactPair) {
	if (!mBallInGame)
		return;

	NxOgre::RigidBody* otherObject;
	if (contactPair.mFirst == static_cast<const NxOgre::RigidBody*>(mApp->getBall()->getBody()) ) //TODO!!: upcast necessary? -> effective c++
		otherObject = contactPair.mSecond;
	else if (contactPair.mSecond == static_cast<const NxOgre::RigidBody*>((mApp->getBall()->getBody())) )
		otherObject = contactPair.mFirst;
	else
		mApp->addToConsole("Error: GameLogic::onContact() - neither object in contactPair is the ball");

	if (otherObject ==  static_cast<const NxOgre::RigidBody*>(mApp->getBlobb1()->getBody()))
		ballContact(TEAM1);
	else if (otherObject ==  static_cast<const NxOgre::RigidBody*>(mApp->getBlobb2()->getBody()))
		ballContact(TEAM2);
	else if (otherObject ==  static_cast<const NxOgre::SceneGeometry*>(mApp->getFloorSceneGeometry())) 
		hitFloor();
}


void GameLogic::score(TEAM team) {
	//mApp->addToConsole("score() called");
	int& scoringTeamScore = (team == TEAM1) ? mScoreTeam1 : mScoreTeam2;
	int& otherTeamScore = (team == TEAM1) ? mScoreTeam2 : mScoreTeam1;

	++scoringTeamScore;
	mApp->addToConsole(StringConverter::toString(mScoreTeam1).append(":").append(StringConverter::toString(mScoreTeam2)));

	int winningScore =  mApp->getConfig().getSettingInt(mRules==OLD_RULES ? "winningScoreOldRules" : "winningScoreNewRules");
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

void GameLogic::ballContact(TEAM team) {
	// PhysX often generates multiple ContactReports for conceptually one touch; therefore ignore all contacts for a short while after a contact
	if (mBlobbContactTimerActive) {
		if (mBlobbContactTimer.getMilliseconds() < 100)
			return;
		else
			mBlobbContactTimerActive = false;
	}
	else {
		mBlobbContactTimerActive = true;
		mBlobbContactTimer.reset();
	}

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
	mCurrentlyServing = mCurrentlyOnBall = team;
	mCurrentContacts = 0;
	mBallInGame = false;

	// activate Timer
	mDelayedNewServeTimer.reset();
	mDelayedNewServeTimerActive = true;
}

void GameLogic::update() {
	if (mDelayedNewServeTimerActive && mDelayedNewServeTimer.getMilliseconds() >= 3000) {
		mApp->getBall()->reset(mCurrentlyServing==TEAM1 ? mServePointTeam1 : mServePointTeam2);
		mDelayedNewServeTimerActive = false;
		mBallInGameNextFrame = true;
	}
	else if (mBallInGameNextFrame) {
		mBallInGame = true;
		mBallInGameNextFrame = false;
	}
}