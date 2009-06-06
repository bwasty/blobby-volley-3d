#pragma once

#include "BaseApplication.h"
#include "OgreImprovedConfigFile.h"

class Blobb;

//namespace NxOgre {
//	class NxActor;
//}



class Application : public BaseApplication {
protected:
	void createFrameListener(); //TODO: make virtual?
	void setupScene();
	
public:
	Application();
	Blobb* getBlobb1() { return mBlobb1; }
	Blobb* getBlobb2() { return mBlobb2; }

	//NxOgre::Actor* mBallActor;

	Ogre::ImprovedConfigFile mConfig;

private:
	Blobb* mBlobb1;
	Blobb* mBlobb2;
};

#pragma once