#pragma once

#include "BaseApplication.h"
#include "OgreImprovedConfigFile.h"

class Blobb;

class OGRE3DBody;



class Application : public BaseApplication {
public:
	Application();
	Blobb* getBlobb1() { return mBlobb1; }
	Blobb* getBlobb2() { return mBlobb2; }

	
	OGRE3DBody* getBallBody() const { return mBallBody; }
	
	Ogre::ImprovedConfigFile& getConfig() { return mConfig; }

private:
	void createFrameListener(); //TODO: make virtual createFrameListener()?
	void setupScene();

	Blobb* mBlobb1;
	Blobb* mBlobb2;

	OGRE3DBody* mBallBody;

	Ogre::ImprovedConfigFile mConfig;
};

#pragma once