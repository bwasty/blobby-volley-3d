#pragma once

#include "BaseApplication.h"

class Blobb;

namespace NxOgre {
	class NxActor;
}

namespace Ogre {
	class ConfigFile;
}

class Application : public BaseApplication {
protected:
	void createFrameListener(); //TODO: make virtual?
	void setupScene();
	
public:
	Application();
	Blobb* getBlobb1() { return mBlobb1; }
	Blobb* getBlobb2() { return mBlobb2; }

	NxOgre::Actor* mBallActor;

	Ogre::ConfigFile mConfigFile;

private:
	Blobb* mBlobb1;
	Blobb* mBlobb2;
};

#pragma once