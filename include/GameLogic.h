#include <NxOgreCallback.h>
#include "Constants.h" // for TEAM enum

class GameLogic : public NxOgre::Callback {
public:
	GameLogic(Application* app) : mApp(app) {};
	void onContact(const NxOgre::ContactPair& );
private:
	Application* mApp;
	int mScoreTeam1;
	int mScoreTeam2;
	TEAM mCurrentlyServing;
};