
#include "Game.h"
#include "Arena.h"

using namespace BV3D;

Arena::Arena(Game* game) {
	m_Game = game;
}

Arena::Arena() {
}

Arena::~Arena() {
}

void Arena::setExtent(Vector extent) {
	Vector llf = Vector(-extent[0]/2, 0.0, -extent[2]/2);
	Vector urb = Vector( extent[0]/2, extent[1], extent[2]/2);

	m_Bounds = Bounds(llf,urb);

	if(m_Game)
		m_Game->notifyBoundsUpdated();
}

Bounds Arena::getTeamBounds(int nTeam) {
	Vector llf = m_Bounds.getLLF();
	Vector urb = m_Bounds.getURB();

	if(nTeam)
		return Bounds(Vector(0.0,0.0,llf[2]), urb);
	else
		return Bounds(llf, Vector(0.0,urb[1],urb[2]));
}