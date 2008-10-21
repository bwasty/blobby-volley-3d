#pragma once

#include "Constants.h"

namespace Ogre {
	class SceneManager;
}

namespace NxOgre {
	class Scene;
}

class Blobb {
private:
	Ogre::SceneManager* mSceneMgr;
	NxOgre::Scene* mNxScene;
	BV3D::TEAM mTeam;
public:
	Blobb(Ogre::SceneManager* sceneMgr, NxOgre::Scene* nxScene, Vector3 position, BV3D::TEAM team);
	~Blobb(); //TODO: implement destructor!
};