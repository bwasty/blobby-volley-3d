#include <NxOgreCallback.h>
#include "Constants.h" // for TEAM enum

class GameLogic : public NxOgre::Callback {
public:
	enum RULES {OLD_RULES, NEW_RULES};

	GameLogic(Application* app);
	
	/** inherited from NxOgre::Callback */
	void onContact(const NxOgre::ContactPair& );

	/** increased team's score and checks team has won */
	void score(TEAM team);

	/** determines if team can score - when the OLD_RULES are used a team can only score when it serves */
	bool canScore(TEAM team);

	/** counts how often a team touches the ball while it's on their side - if the count is above the limit, the other team gets a point */
	void ballContact(TEAM team);

	/** ball hits floor - score for the "other" team if they canScore() */ 
	void hitFloor();

	void prepareNewServe(TEAM team);

	/** to be called once per frame from "main loop" */
	void update();

	
private:
	Application* mApp;
	int mScoreTeam1;
	int mScoreTeam2;

	TEAM mCurrentlyServing;
	TEAM mCurrentlyOnBall;

	/** number of times the team which currently has the ball has touched it (without the ball crossing the net). 
	Maximum is set in config:maxBallContacts (default: 3)*/
	int mCurrentContacts;

	RULES mRules;

	/** the ball is in the game from the serve until it hits the ground. 
	Blobb/floor contacts are ignored when false */
	bool mBallInGame;

	Ogre::Timer mBlobbContactTimer;
	bool mBlobbContactTimerActive;

	Ogre::Timer mDelayedNewServeTimer;
	bool mDelayedNewServeTimerActive;

	Ogre::Vector3 mServePointTeam1;
	Ogre::Vector3 mServePointTeam2;

};