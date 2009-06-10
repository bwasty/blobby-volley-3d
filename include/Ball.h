#pragma once

#include <OgreVector3.h>

class Application;

class Ball {
public:
	Ball(Application* app, Ogre::Vector3 startPosition=Ogre::Vector3::ZERO);

	/** resets the ball to the initial position (startPosition) (for serve) */
	void reset();

private:
	Application* mApp;
	OGRE3DBody* mBallBody;
	Ogre::Vector3 mStartPosition;
};