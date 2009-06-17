#pragma once

#include "BaseApplication.h"
#include <OgreFrameListener.h>
//#include "OgreImprovedConfigFile.h"

class Blobb;
class Ball;

class OGRE3DBody;



class Application : public BaseApplication, public Ogre::FrameListener {
public:
	~Application();

	inline Blobb* getBlobb1() { return mBlobb1; }
	inline Blobb* getBlobb2() { return mBlobb2; }
	
	inline Ball* getBall() const { return mBall; }

	// FrameListener
	//bool frameStarted(const Ogre::FrameEvent &evt);
	bool frameRenderingQueued(const Ogre::FrameEvent &evt);
	bool frameEnded(const Ogre::FrameEvent &evt);
	

private:
	void setupInputSystem();
	void createFrameListener(); //TODO: make virtual createFrameListener()?
	//void setupScene();
	void fillScene();

	Blobb* mBlobb1;
	Blobb* mBlobb2;

	Ball* mBall;

	ControlsListener *mControls;
};

#pragma once