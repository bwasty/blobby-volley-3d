
#include <stdio.h>
#include <math.h>
#include "Blobb.h"
#include "KeyboardControls.h"
#include <vrs/io/threedsreader.h>

Blobb::Blobb() {
	m_Controls = new KeyboardControls();
	m_vtrCtrlsOrientation = Vector(0.0,0.0,0.05);

	m_Scene = new SceneThing();
	m_Translation = new Translation();
	m_Scene->append(m_Translation);
	m_Scene->append(ThreeDSReader::readObject("blobb1.3ds"));	// TODO: exception handling
}

Blobb::~Blobb() {
	delete m_Controls;
}

void Blobb::setCtrlsOrientation(Vector vtrCtrlsOri) {
	double dStepDistance = sqrt(m_vtrCtrlsOrientation.dotProduct(m_vtrCtrlsOrientation));
	m_vtrCtrlsOrientation = vtrCtrlsOri.normalized() * dStepDistance;
}

void Blobb::setStepDistance(double distance) {
	m_vtrCtrlsOrientation = m_vtrCtrlsOrientation.normalized() * distance;
}

void Blobb::setPosition(Vector vtrPos) {
	Vector llf = m_Bounds.getLLF();		// (-x,-y,-z)
	Vector urb = m_Bounds.getURB();		// (x,y,z)
	if(vtrPos[0]<llf[0]) vtrPos[0] = llf[0];
	if(vtrPos[1]<llf[1]) vtrPos[1] = llf[1];
	if(vtrPos[2]<llf[2]) vtrPos[2] = llf[2];
	if(vtrPos[0]>urb[0]) vtrPos[0] = urb[0];
	if(vtrPos[1]>urb[1]) vtrPos[1] = urb[1];
	if(vtrPos[2]>urb[2]) vtrPos[2] = urb[2];
	m_vtrPosition = vtrPos;
	// TODO: apply Newton force/movement
}

void Blobb::moveUp() {
	setPosition(m_vtrPosition + m_vtrCtrlsOrientation);
}

void Blobb::moveDown() {
	setPosition(m_vtrPosition - m_vtrCtrlsOrientation);
}

void Blobb::moveRight() {
	setPosition(Vector(
		m_vtrPosition[0] + m_vtrCtrlsOrientation[2],
		m_vtrPosition[1],
		m_vtrPosition[2] - m_vtrCtrlsOrientation[0]));
}

void Blobb::moveLeft() {
	setPosition(Vector(
		m_vtrPosition[0] - m_vtrCtrlsOrientation[2],
		m_vtrPosition[1],
		m_vtrPosition[2] + m_vtrCtrlsOrientation[0]));
}

void Blobb::jump() {
	// TODO: apply Newton force
	printf("jump!\n");
}

void Blobb::setBounds(Bounds bounds) {
	// TODO: subtract blobb extent
	m_Bounds = bounds;
}

Controls* Blobb::getControls() {
	return m_Controls;
}

void Blobb::update() {
	if(m_Controls->isRequested(Controls::UP)) moveUp();
	if(m_Controls->isRequested(Controls::DOWN)) moveDown();
	if(m_Controls->isRequested(Controls::RIGHT)) moveRight();
	if(m_Controls->isRequested(Controls::LEFT)) moveLeft();
	if(m_Controls->isRequested(Controls::JUMP)) jump();
	// TODO: update transformation matrix
	//printf("(%f,%f,%f)\n",m_vtrPosition[0],m_vtrPosition[1],m_vtrPosition[2]);
	m_Translation->setTranslate(m_vtrPosition);
}

SO<SceneThing> Blobb::getScene() {
	return m_Scene;
}