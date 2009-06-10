#pragma once

#include "BaseApplication.h"
//#include "OgreImprovedConfigFile.h"

class Blobb;
class Ball;

class OGRE3DBody;



class Application : public BaseApplication {
public:
	//Application();
	Blobb* getBlobb1() { return mBlobb1; }
	Blobb* getBlobb2() { return mBlobb2; }

	
	Ball* getBall() const { return mBall; }
	

private:
	void createFrameListener(); //TODO: make virtual createFrameListener()?
	void setupScene();
	void fillScene();

	Blobb* mBlobb1;
	Blobb* mBlobb2;

	Ball* mBall;
};

#pragma once