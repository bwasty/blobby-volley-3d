#pragma once

#include "Constants.h"

namespace Ogre {
	class SceneManager;
}

namespace NxOgre {
	class Scene;
}
class NxD6Joint;

class Blobb {
private:
	Ogre::SceneManager* mSceneMgr;
	NxOgre::Scene* mNxScene;
	BV3D::TEAM mTeam;
	NxD6Joint* mD6Joint; /** prevents the blobb from toppling (locks rotational axes */
public:
	Blobb(Ogre::SceneManager* sceneMgr, NxOgre::Scene* nxScene, Vector3 position, BV3D::TEAM team);
	~Blobb() {
		// mNxScene handles this usually for PhysX objects, but the joint is created manually (see constructor)
		delete mD6Joint; 
	}
};