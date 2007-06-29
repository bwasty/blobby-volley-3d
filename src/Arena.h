#ifndef BV3D_ARENA
#define BV3D_ARENA

#include <vrs/sharedobj.h>
#include <vrs/so.h>
#include <vrs/bounds.h>
#include <vrs/sg/scenething.h>
#include <vrs/opengl/shapematerialgl.h>
#include <vrs/container/fixedsizeiterator.h>
#include <vrs/polygonset.h>

using namespace VRS;

namespace BV3D {
	class Arena : public SharedObj {
	public:
		Arena();
		~Arena();
		void setExtent(Vector extent);
		Bounds getTeamBounds(int team);
		SO<SceneThing> getScene();
	protected:
		SO<SceneThing>		m_Scene;
		SO<ShapeMaterialGL>	m_Material;
		SO<FixedSizeIterator<Vector>>	m_WallsVertices;
		SO<PolygonSet>		m_Walls;	// walls shape
		Bounds				m_Bounds;	// bounds of the Arena box
	};
}

#endif	//BV3D_ARENA