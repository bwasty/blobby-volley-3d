#pragma once

#include "Constants.h"
#include <OgreColourValue.h>

namespace Ogre {
	class SceneManager;
}

namespace NxOgre {
	class Scene;
	class Actor;
}
class NxD6Joint;
class OGRE3DBody;

class Application;

class Blobb {
private:
	Application* mApp;
	Ogre::SceneManager* mSceneMgr;
	TEAM mTeam;
	OGRE3DBody* mBody;
	NxD6Joint* mD6Joint; /** prevents the blobb from toppling (locks rotational axes) */

public:
	Blobb(Application* app, Ogre::Vector3 position, TEAM team, Ogre::ColourValue colour=Ogre::ColourValue::Blue);
	
	/** loads settings from global config. for initial setup and dynamic changing of settings */
	void loadSettings();

	~Blobb() {
		// mNxScene handles this usually for PhysX objects, but the joint is created manually (see constructor) //TODO: deletion of D6Joint?
		//delete mD6Joint; 
	}

	void move(Ogre::Vector2 direction);
	void jump();
	const OGRE3DBody* getBody() { return mBody; }
};