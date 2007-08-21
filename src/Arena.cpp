
#include "Arena.h"
//#include <vrs/so.h>
#include <vrs/opengl/shapematerialgl.h>
#include <vrs/container/fixedsizeiterator.h>
#include <vrs/polygonset.h>
#include <vrs/sg/scenething.h>
#include <vrs/facestyle.h>
#include <Newton.h>

/**
 * sets up an Arena with an extent of (1.0,1.0,1.0)
 */
BV3D::Arena::Arena() {
	///m_ArenaPhysics = 0;
	m_WallsVertices = new VRS::FixedSizeIterator<VRS::Vector>(10);

	m_Scene = new VRS::SceneThing();

	m_Material = new VRS::ShapeMaterialGL(VRS::Color(0.9,0.9,0.9,0.3));
	m_Scene->append(m_Material);

	m_Scene->append(new VRS::FaceStyle(VRS::FaceStyle::Filled,VRS::FaceStyle::Culled));

	m_Walls = new VRS::PolygonSet(VRS::PolygonSet::QuadStrip,m_WallsVertices);
	m_Scene->append(m_Walls);

	// Physics setup
	m_World = NewtonCreate(0,0);
	m_Gravity = 9.81f;
	m_Body = 0;

	setExtent(VRS::Vector(1.0,1.0,1.0));
}

BV3D::Arena::~Arena() {
	if(m_Body)
		NewtonDestroyBody(m_World, m_Body);
	NewtonDestroy(m_World);
}

void BV3D::Arena::setExtent(VRS::Vector extent) {
	VRS::Vector llf(-extent[0]/2, 0.0, -extent[2]/2);
	VRS::Vector urb( extent[0]/2, extent[1], extent[2]/2);

	m_Bounds = VRS::Bounds(llf,urb);

	// set up walls' vertices
	m_WallsVertices->set(0, VRS::Vector(urb[0],llf[1],urb[2]));
	m_WallsVertices->set(1, VRS::Vector(urb[0],urb[1],urb[2]));
	m_WallsVertices->set(2, VRS::Vector(llf[0],llf[1],urb[2]));
	m_WallsVertices->set(3, VRS::Vector(llf[0],urb[1],urb[2]));

	m_WallsVertices->set(4, VRS::Vector(llf[0],llf[1],llf[2]));
	m_WallsVertices->set(5, VRS::Vector(llf[0],urb[1],llf[2]));

	m_WallsVertices->set(6, VRS::Vector(urb[0],llf[1],llf[2]));
	m_WallsVertices->set(7, VRS::Vector(urb[0],urb[1],llf[2]));

	m_WallsVertices->set(8, VRS::Vector(urb[0],llf[1],urb[2]));
	m_WallsVertices->set(9, VRS::Vector(urb[0],urb[1],urb[2]));

	// (re)create physical arena body
	const dFloat wallDepth = 1.0f;

	// destroy old physical arena body if it exists
	if(m_Body) NewtonDestroyBody(m_World, m_Body);

	NewtonCollision* collision[5];
	dFloat matrix[16] = {1.0,0.0,0.0,0.0, 0.0,1.0,0.0,0.0, 0.0,0.0,1.0,0.0, 0.0,0.0,0.0,1.0};

	// create floor
	matrix[12] = 0.0;
	matrix[13] = (dFloat) -(wallDepth / 2);
	matrix[14] = 0.0;
	collision[0] = NewtonCreateBox(m_World, (dFloat)extent.get(0), wallDepth, (dFloat)extent.get(2), matrix);

	// create wall bounding the arena on the positive z axis
	matrix[12] = 0.0;
	matrix[13] = (dFloat) (extent[1] / 2);
	matrix[14] = (dFloat) (extent[2] + wallDepth) / 2;
	collision[1] = NewtonCreateBox(m_World, (dFloat)extent[0], (dFloat)extent[1], wallDepth, matrix);

	// create wall bounding the arena on the negative z axis
	matrix[12] = 0.0;
	matrix[13] = (dFloat)  (extent[1] / 2);
	matrix[14] = (dFloat) -(extent[2] + wallDepth) / 2;
	collision[2] = NewtonCreateBox(m_World, (dFloat)extent[0], (dFloat)extent[1], wallDepth, matrix);

	// create wall bounding the arena on the negative x axis
	matrix[12] = (dFloat) -(extent[0] + wallDepth) / 2;
	matrix[13] = (dFloat)  (extent[1] / 2);
	matrix[14] = 0.0;
	collision[3] = NewtonCreateBox(m_World, wallDepth, (dFloat)extent[1], (dFloat)extent[2], matrix);

	// create wall bounding the arena on the positive x axis
	matrix[12] = (dFloat) (extent[0] + wallDepth) / 2;
	matrix[13] = (dFloat) (extent[1] / 2);
	matrix[14] = 0.0;
	collision[4] = NewtonCreateBox(m_World, wallDepth, (dFloat)extent[1], (dFloat)extent[2], matrix);

	// combine walls and floor collisions and create physical arena body from compound collision
	NewtonCollision* compoundCollision = NewtonCreateCompoundCollision(m_World, 5, collision);
	m_Body = NewtonCreateBody(m_World, compoundCollision);

	// release all collision references
	for(int i=0;i<5;i++)
		NewtonReleaseCollision(m_World, collision[i]);
	NewtonReleaseCollision(m_World, compoundCollision);
}

VRS::Bounds BV3D::Arena::getTeamBounds(int team) {
	VRS::Vector llf = m_Bounds.getLLF();
	VRS::Vector urb = m_Bounds.getURB();

	if(team)
		return VRS::Bounds(VRS::Vector(0.0,0.0,llf[2]), urb);
	else
		return VRS::Bounds(llf, VRS::Vector(0.0,urb[1],urb[2]));
}

void BV3D::Arena::updateWorld(float timestep) {
	NewtonUpdate(m_World, (dFloat)timestep);
}
