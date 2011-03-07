#pragma once

#include <OgreVector3.h>

class Application;
class OGRE3DBody;

class Ball {
public:
	Ball(Application* app, Ogre::Vector3 startPosition=Ogre::Vector3::ZERO);

	/** resets the ball to the  given position and freezes it in midair (for serve) */
	void reset(Ogre::Vector3);

	/** loads settings from global config. for initial setup and dynamic changing of settings */
	void loadSettings();

	const OGRE3DBody* getBody() { return mBallBody; }
	//Ogre::Vector3 getPosition();

private:
	Application* mApp;
	OGRE3DBody* mBallBody;
};