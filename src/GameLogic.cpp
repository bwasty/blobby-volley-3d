#include "GameLogic.h"

//#include <NxOgreContactPair.h>
#include <OGRE3DBody.h>
//#include <OgreStringConverter.h>

#include "Constants.h"
#include "Application.h"
#include "Ball.h"
#include "Blobb.h"
#include "GUI.h"

using namespace Ogre;

// TODO!!: GameLogic - make rules changeable (option? configurable?)
GameLogic::GameLogic(Application* app) : mApp(app), mScoreTeam1(0), mScoreTeam2(0), mCurrentlyServing(TEAM1), mCurrentlyOnBall(TEAM1),
										 mCurrentContacts(0), mRules(NEW_RULES), mBallInGame(true), mBlobbContactTimer(Ogre::Timer()), mBlobbContactTimerActive(false),
								         mDelayedNewServeTimer(Ogre::Timer()), mDelayedNewServeTimerActive(false)
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
	// first check if team can score (if old rules: only when it served)
	if (mRules == NEW_RULES || team == mCurrentlyServing) {
		//mApp->addToConsole("score() called");
		int& scoringTeamScore = (team == TEAM1) ? mScoreTeam1 : mScoreTeam2;
		int& otherTeamScore = (team == TEAM1) ? mScoreTeam2 : mScoreTeam1;

		++scoringTeamScore;
		//mApp->addToConsole(StringConverter::toString(mScoreTeam2).append(":").append(StringConverter::toString(mScoreTeam1)));
		mApp->getGUI()->updateScoreDisplay(mScoreTeam1, mScoreTeam2, team);

		int winningScore =  mApp->getConfig().getSettingInt(mRules==OLD_RULES ? "winningScoreOldRules" : "winningScoreNewRules");
		if (scoringTeamScore >= winningScore && (scoringTeamScore - otherTeamScore >= 2)) {
			mApp->addToConsole("Game Over");
			mApp->getGUI()->mDebugText = "Game Over! Press ENTER for new game";
			mBallInGame = false;
			return;
		}
	}
	
	prepareNewServe(team);
}

void GameLogic::ballContact(TEAM team) {
	// PhysX often generates multiple ContactReports for conceptually one touch; therefore ignore all contacts for a short while after a contact
	if (mBlobbContactTimerActive && team==mCurrentlyOnBall) {
		if (mBlobbContactTimer.getMilliseconds() < 500)
			return;
		else
			mBlobbContactTimerActive = false;
	}
	else {
		mBlobbContactTimerActive = true;
		mBlobbContactTimer.reset();
	}

	mApp->addToConsole(team == TEAM1 ? "Blobb1 hits ball" : "Blobb2 hits ball");

	if (team == mCurrentlyOnBall) {
		if (++mCurrentContacts > mApp->getConfig().getSettingInt("maxBallContacts")) {
			score(team==TEAM1 ? TEAM2 : TEAM1);
		}
	}
	else {
		mCurrentlyOnBall = team;
		mCurrentContacts = 1;
	}
}

void GameLogic::hitFloor() {
	// determine on whose side the ball hit the floor - the net is on the z-axis, so check x-coordinate is enough
	mApp->addToConsole("Ball hits floor");
	if (mApp->getBall()->getBody()->getGlobalPosition()[0] > 0) // TEAM2's side
		score(TEAM1);
	else
		score(TEAM2);
}

void GameLogic::prepareNewServe(TEAM team) {
	mCurrentlyServing = mCurrentlyOnBall = team;
	mCurrentContacts = 0;
	mBallInGame = false;

	mApp->getGUI()->updateScoreDisplay(mScoreTeam1, mScoreTeam2, team); // TODO!: double "!" for matchball?

	// activate Timer
	mDelayedNewServeTimer.reset();
	mDelayedNewServeTimerActive = true;
}

void GameLogic::update() {
	if (mDelayedNewServeTimerActive && mDelayedNewServeTimer.getMilliseconds() >= 3000) {
		mApp->getBall()->reset(mCurrentlyServing==TEAM1 ? mServePointTeam1 : mServePointTeam2);
		mDelayedNewServeTimerActive = false;
		mBallInGame = true;
	}
}

void GameLogic::newGame() {
	mScoreTeam1 = 0;
	mScoreTeam2 = 0;
	mCurrentContacts = 0;
	mCurrentlyOnBall = TEAM1;
	mCurrentlyServing = TEAM1;
	mBallInGame = true;

	// reset ball
	mApp->getBall()->reset(mServePointTeam1);
	mApp->getGUI()->updateScoreDisplay(0, 0, TEAM1);


	
}