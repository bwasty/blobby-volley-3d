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

#include "Arena.h"
#include <vrs/facestyle.h>

using namespace BV3D;

/**
 * sets up an Arena with an extent of (1.0,1.0,1.0)
 */
Arena::Arena() {
	m_WallsVertices = new FixedSizeIterator<Vector>(10);

	// set initial volume
	setExtent(Vector(1.0,1.0,1.0));

	m_Scene = new SceneThing();

	m_Material = new ShapeMaterialGL(Color(0.9,0.9,0.9,0.3));
	m_Scene->append(m_Material);

	m_Scene->append(new FaceStyle(FaceStyle::Filled,FaceStyle::Culled));

	m_Walls = new PolygonSet(PolygonSet::QuadStrip,m_WallsVertices);
	m_Scene->append(m_Walls);
}

Arena::~Arena() {
}

/**
 * resizes the Arena to the given extent
 * \param extent is a Vector specifying the distance of each dimension of the Arena box
 */
void Arena::setExtent(Vector extent) {
	Vector llf = Vector(-extent[0]/2, 0.0, -extent[2]/2);
	Vector urb = Vector( extent[0]/2, extent[1], extent[2]/2);

	m_Bounds = Bounds(llf,urb);

	// set up walls' vertices
	m_WallsVertices->set(0, Vector(urb[0],llf[1],urb[2]));
	m_WallsVertices->set(1, Vector(urb[0],urb[1],urb[2]));
	m_WallsVertices->set(2, Vector(llf[0],llf[1],urb[2]));
	m_WallsVertices->set(3, Vector(llf[0],urb[1],urb[2]));

	m_WallsVertices->set(4, Vector(llf[0],llf[1],llf[2]));
	m_WallsVertices->set(5, Vector(llf[0],urb[1],llf[2]));

	m_WallsVertices->set(6, Vector(urb[0],llf[1],llf[2]));
	m_WallsVertices->set(7, Vector(urb[0],urb[1],llf[2]));

	m_WallsVertices->set(8, Vector(urb[0],llf[1],urb[2]));
	m_WallsVertices->set(9, Vector(urb[0],urb[1],urb[2]));
}

/**
 * returns the respective bounds for Blobbs
 * \param team is 0 for Blobbs in the area along the negative x-axis or anything else for Blobbs on the other side
 * \return a Bounds object with the respective bounds
 */
Bounds Arena::getTeamBounds(int team) {
	Vector llf = m_Bounds.getLLF();
	Vector urb = m_Bounds.getURB();

	if(team)
		return Bounds(Vector(0.0,0.0,llf[2]), urb);
	else
		return Bounds(llf, Vector(0.0,urb[1],urb[2]));
}

/**
 * returns the scene node of the Arena
 */
SO<SceneThing> Arena::getScene() {
	return m_Scene;
}