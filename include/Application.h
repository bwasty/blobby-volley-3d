#pragma once

#include "BaseApplication.h"
#include <OgreFrameListener.h>
//#include "OgreImprovedConfigFile.h"

class Blobb;
class Ball;
class GameLogic;

class OGRE3DBody;

namespace NxOgre {
	class Material;
	class SceneGeometry;
}


class Application : public BaseApplication, public Ogre::FrameListener {
public:
	~Application();

	inline Blobb* getBlobb1() const { return mBlobb1; }
	inline Blobb* getBlobb2() const { return mBlobb2; }
	
	inline Ball* getBall() const { return mBall; }

	inline GUI* getGUI() const { return mGUI; }

	inline GameLogic* getGameLogic() const { return mGameLogic; }

	// FrameListener
	bool frameStarted(const Ogre::FrameEvent &evt);
	bool frameRenderingQueued(const Ogre::FrameEvent &evt);
	bool frameEnded(const Ogre::FrameEvent &evt);

	// Physics Materials, setup in fillScene()
	NxOgre::Material* mFloorPhysicsMaterial;
	NxOgre::Material* mWallPhysicsMaterial;
	NxOgre::Material* mBlobbPhysicsMaterial;
	NxOgre::Material* mBallPhysicsMaterial;
	NxOgre::Material* mNetPhysicsMaterial;

	/** loads settings from global config. for initial setup and dynamic changing of settings */
	void loadSettings();

	const NxOgre::SceneGeometry* getFloorSceneGeometry() { return mFloorSceneGeometry; }
	const NxOgre::SceneGeometry* getWallsSceneGeometry() { return mWallsSceneGeometry; }

	void addToConsole(Ogre::String);



private:
	void setupInputSystem();
	void setupGUI();
	void createFrameListener();
	void fillScene();

	//TODO!: Application.h - check which pointer members can be converted to local members
	NxOgre::SceneGeometry* mFloorSceneGeometry;
	NxOgre::SceneGeometry* mWallsSceneGeometry;

	Blobb* mBlobb1;
	Blobb* mBlobb2;

	Ball* mBall;

	ControlsListener *mControls;

	GUI* mGUI;

	GameLogic* mGameLogic;

	/** modifier for the physics timestep (1.0 = real time) */
	float mSimulationSpeed;
};