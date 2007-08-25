/**
 * The Arena class represents the box in which the Blobbs play Ball.
 * It is responsible for:
 * - setting up valid bounds for the Arena
 * - passing valid bounds for Blobbs and the Ball
 * - creating the 3D shape - a topless box with transparent walls
 *
 * the Arena floor is centered at the origin
 * the Net runs along the z-axis
 * the individual fields run along the positive/negative x-axis
 * the walls are transparent to see the play and still give some hint about Ball collision
 * the box is assumed to be topless, still the walls have a certain
 *  height that should prevent the ball from bouncing out
 */

#ifndef BV3D_ARENA
#define BV3D_ARENA

#include <vrs/sharedobj.h>
#include <vrs/bounds.h>

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
	class Arena : public VRS::SharedObj {
	public:
		/**
		 * ctor
		 */
		Arena(/*VRS::SO<BV3D::Game>*/ BV3D::Game* game);

		/**
		 * dtor
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
		VRS::Bounds getBounds() {return m_Bounds;}

		/**
		 * get the valid bounds for one of the blobbs
		 */
		VRS::Bounds getTeamBounds(int team);

		/**
		 * get the local arena scene
		 */
		VRS::SO<VRS::SceneThing> getScene() {return m_Scene;}

	public:	// Physics
		/**
		 * get the current gravity force value
		 */
		float getGravity() {return m_Gravity;}

		/**
		 * set the current gravity force value
		 */
		void  setGravity(float gravity) {m_Gravity = gravity;}

		/**
		 * perform physics simulation
		 * \param timestep specifies the amount of time in millisecs to advance during simulation
		 */
		void updateWorld(float timestep);

		/**
		 * return pointer to the Physics world
		 */
		NewtonWorld* getWorld() {return m_World;}

		int getBallMaterialID() {return mBallMaterialID;}
		int getBlobbMaterialID() {return mBlobbMaterialID;}
		int getWallMaterialID() {return mWallMaterialID;}
		int getFloorMaterialID() {return mFloorMaterialID;}
		int getNetMaterialID() {return mNetMaterialID;}
		int getInvisibleBarrierID() {return mInvisibleBarrierID;}

	private:
		VRS::SO<VRS::SceneThing>		m_Scene;		// local arena scene (walls)
		VRS::SO<VRS::FixedSizeIterator<VRS::Vector>>	m_WallsVertices;	// vertices for walls
		VRS::SO<VRS::ShapeMaterialGL>	m_Material;		// wall material
		VRS::SO<VRS::PolygonSet>		m_Walls;		// walls shape
		VRS::Bounds				m_Bounds;			// bounds of the Arena box

	private:	// Physics
		float			m_Gravity;	// gravity of world
		NewtonWorld*	m_World;	// physics world
		NewtonBody*		m_Body;		// physical arena walls
		NewtonBody*		m_Floor;		// physical arena floor

		int mBallMaterialID;
		int mBlobbMaterialID;
		int mWallMaterialID;
		int mFloorMaterialID;
		int mNetMaterialID;
		int mInvisibleBarrierID;

		void setupMaterials(BV3D::Game* game);
		static int contactBeginCallback(const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1);
		static int contactProcessCallback(const NewtonMaterial* material, const NewtonContact* contact);
		static void contactEndCallback(const NewtonMaterial* material);
	};
}

#endif	//BV3D_ARENA