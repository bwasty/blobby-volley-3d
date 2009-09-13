#include <NxOgreCallback.h>
#include "Constants.h" // for TEAM enum

class GameLogic : public NxOgre::Callback {
public:
	enum RULES {OLD_RULES, NEW_RULES};

	GameLogic(Application* app) : mApp(app), rules (NEW_RULES) {};
	void onContact(const NxOgre::ContactPair& );

	/** increased team's score and checks team has won */
	void score(TEAM team);
	
private:
	Application* mApp;
	int mScoreTeam1;
	int mScoreTeam2;

	TEAM mCurrentlyServing;

	/** number of times the team which currently has the ball has touched it (without the ball crossing the net). 
	Maximum is set in config:maxBallContacts (default: 3)*/
	int mCurrentContacts;

	RULES rules;

	/** the game "runs" from the serve until the ball hits the ground. 
	Until the ball is reset for the next serve the game is not "running" and ground touches are ignored */
	bool gameRunning;
};