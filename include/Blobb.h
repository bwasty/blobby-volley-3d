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

class Blobb {
private:
	Ogre::SceneManager* mSceneMgr;
	NxOgre::Scene* mNxScene;
	BV3D::TEAM mTeam;
	NxOgre::Actor* mActor; /** the physical actor for the blobb */
	NxD6Joint* mD6Joint; /** prevents the blobb from toppling (locks rotational axes */

public:
	Blobb(Ogre::SceneManager* sceneMgr, NxOgre::Scene* nxScene, Ogre::Vector3 position, BV3D::TEAM team, Ogre::ColourValue colour=Ogre::ColourValue::Blue);
	~Blobb() {
		// mNxScene handles this usually for PhysX objects, but the joint is created manually (see constructor)
		delete mD6Joint; 
	}

	void move(Ogre::Vector2 direction);
	void jump(float height);
};