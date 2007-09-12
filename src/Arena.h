#ifndef BV3D_ARENA
#define BV3D_ARENA

#include <vrs/sharedobj.h>
#include <vrs/bounds.h>
#include "Constants.h"

struct NewtonWorld;
struct NewtonBody;
struct NewtonMaterial;
struct NewtonContact;

namespace VRS {
	template<class T> class SO;
	class ShapeMaterialGL;
	template<typename T> class FixedSizeIterator;
	class PolygonSet;
	class SceneThing;
}

namespace BV3D {
	class Game;
	class Ball;
	class Referee;
	class Blobb;

	/**
		\brief The Arena class represents the box in which the Blobbs play Ball.

		It is responsible for:
		- setting up valid bounds for the Arena
		- passing valid bounds for Blobbs and the Ball
		- creating the 3D shape - a topless box with transparent walls

		the Arena floor is centered at the origin
		the Net runs along the z-axis
		the individual fields run along the positive/negative x-axis
		the walls are transparent to see the play and still give some hint about Ball collision
		the box is assumed to be topless, still the walls have a certain
		height that should prevent the ball from bouncing out
	*/
	class Arena : public VRS::SharedObj {
	public:
		/**
		 \ctor
		 */
		Arena(BV3D::Game* game);

		/**
		 \dtor
		 */
		~Arena();

		/**
		 * resizes the Arena to the given extent
		 * the arena bottom is centered at the origin
		 * \param extent is a Vector specifying the distance of each dimension of the Arena box
		 */
		void setExtent(VRS::Vector extent);

		/**
		 * get the bounds of the whole arena
		 */
		VRS::Bounds getBounds() {return mBounds;}

		/**
		 * get the valid bounds for one of the blobbs
		 */
		VRS::Bounds getTeamBounds(BV3D::TEAM team);

		/**
		 * get the local arena scene
		 */
		VRS::SO<VRS::SceneThing> getScene() {return mScene;}

	public:	// Physics
		/**
		 * perform physics simulation
		 * \param timestep specifies the amount of time in millisecs to advance during simulation
		 */
		void updateWorld(float timestep);

		/**
		 * return pointer to the Physics world
		 */
		NewtonWorld* getWorld() {return mWorld;}

		void createMaterials();
		void setupMaterials();

		int getBallMaterialID() {return mBallMaterialID;}
		int getBlobbMaterialID() {return mBlobbMaterialID;}
		int getWallMaterialID() {return mWallMaterialID;}
		int getFloorMaterialID() {return mFloorMaterialID;}
		int getNetMaterialID() {return mNetMaterialID;}
		int getInvisibleBarrierID() {return mInvisibleBarrierID;}
		int getAiTriggerID(){return mAiTriggerID;}

	private:
		VRS::SO<VRS::SceneThing>		mScene;		// local arena scene (walls)
		VRS::SO<VRS::FixedSizeIterator<VRS::Vector>>	mWallsVertices;	// vertices for walls
		VRS::SO<VRS::ShapeMaterialGL>	mMaterial;		// wall material
		VRS::SO<VRS::PolygonSet>		mWalls;		// walls shape
		VRS::Bounds						mBounds;		// bounds of the Arena box
		VRS::SO<VRS::SceneThing>		mNet;
		VRS::SO<BV3D::Game>				mGame;

	private:	// Physics
		NewtonWorld*	mWorld;	// physics world
		NewtonBody*		mBody;		// physical arena walls
		NewtonBody*		mFloor;		// physical arena floor

		int mBallMaterialID;
		int mBlobbMaterialID;
		int mWallMaterialID;
		int mFloorMaterialID;
		int mNetMaterialID;
		int mInvisibleBarrierID;
		int mAiTriggerID;

		static int blobbContactBeginCallback(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1);
		static int contactProcessCallback(const NewtonMaterial* material, const NewtonContact* contact);
	};

	struct CollisionData {
	int material1;
	int material2;
	VRS::SO<BV3D::Game> game;
	VRS::SO<BV3D::Arena> arena;
	VRS::SO<BV3D::Ball> ball;
	VRS::SO<BV3D::Referee> referee;
	VRS::SO<BV3D::Blobb> currentBlobb;
	NewtonWorld* world;
	int delayStartFrame;
	};
}



#endif	//BV3D_ARENA