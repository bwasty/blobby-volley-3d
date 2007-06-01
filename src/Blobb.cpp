/**
 * The Blobb class represents the players in BlobbyVolley3D
 * it is responsible for:
 * - creating(loading) and managing: the 3D shape of the Blobb
 * - forwarding InputEvents to its Controls
 * - managing the Controls orientation and movement distance
 * - moving the Blobb inside its assigned bounds
 * - updating the position based on input and physics
 */

#include <stdio.h>
#include <math.h>
#include "Blobb.h"
#include "KeyboardControls.h"
#include <vrs/io/threedsreader.h>
#include <vrs/sphere.h>

Blobb::Blobb() {
	m_Controls = new KeyboardControls();
	m_CtrlsOrientation = Vector(0.0,0.0,0.05);

	// setting up the Blobb sub-scene
	m_Scene = new SceneThing();
	m_Translation = new Translation();
	m_Scene->append(m_Translation);
	m_Material = new ShapeMaterialGL(Color(1.0,0.0,0.0,0.4));
	m_Scene->append(m_Material);
	//m_Scene->append(ThreeDSReader::readObject("blobb1.3ds"));	// TODO: exception handling
	m_Scene->append(new Sphere());
}

Blobb::~Blobb() {
	delete m_Controls;
}

/*
 * sets the base orientation for all relative movement
 * \param ctrlsOri is a Vector which points forward
 */
void Blobb::setCtrlsOrientation(Vector ctrlsOri) {
	double dStepDistance = sqrt(m_CtrlsOrientation.dotProduct(m_CtrlsOrientation));
	m_CtrlsOrientation = ctrlsOri.normalized() * dStepDistance;
}

/*
 * set the distance of movement
 * \param distance
 */
void Blobb::setStepDistance(double distance) {
	m_CtrlsOrientation = m_CtrlsOrientation.normalized() * distance;
}

/*
 * position the Blobb
 * the new position is checked against the bounds and corrected if necessary
 * \param position
 */
void Blobb::setPosition(Vector position) {
	if(m_Bounds.isDefined()) {
		Vector llf = m_Bounds.getLLF();
		Vector urb = m_Bounds.getURB();
		if(position[0] < llf[0]) position[0] = llf[0];
		if(position[1] < llf[1]) position[1] = llf[1];
		if(position[2] < llf[2]) position[2] = llf[2];
		if(position[0] > urb[0]) position[0] = urb[0];
		if(position[1] > urb[1]) position[1] = urb[1];
		if(position[2] > urb[2]) position[2] = urb[2];
	}
	m_Position = position;
	// TODO: apply Newton force/movement
}

/*
 * convenience method for moving one step forward
 */
void Blobb::moveForward() {
	setPosition(m_Position + m_CtrlsOrientation);
}

/*
 * convenience method for moving one step backward
 */
void Blobb::moveBackward() {
	setPosition(m_Position - m_CtrlsOrientation);
}

/*
 * convenience method for moving one step to the right
 */
void Blobb::moveRight() {
	setPosition(Vector(
		m_Position[0] + m_CtrlsOrientation[2],
		m_Position[1],
		m_Position[2] - m_CtrlsOrientation[0]));
}

/*
 * convenience method for moving one step to the left
 */
void Blobb::moveLeft() {
	setPosition(Vector(
		m_Position[0] - m_CtrlsOrientation[2],
		m_Position[1],
		m_Position[2] + m_CtrlsOrientation[0]));
}

/*
 * method to invoke jumping
 */
void Blobb::jump() {
	// TODO: apply Newton force
	printf("jump!\n");
}

/*
 * sets the inhabitable volume for the Blobb
 * it is assumed that the Blobb's bounding box is centered at the origin!
 */
void Blobb::setBounds(Bounds bounds) {
	Vector llf = bounds.getLLF();
	Vector urb = bounds.getURB();
	Vector extent = Vector(2.0,2.0,2.0);	// simulated bounds for standard Sphere
	llf += extent/2;
	urb -= extent/2;
	m_Bounds = Bounds(llf,urb);	// TODO; what if bounds too small?!
	setPosition(m_Position);	// reset position for bounds check
}

/*
 * \return a pointer to the Blobb's Controls object
 */
Controls* Blobb::getControls() {
	return m_Controls;
}

/*
 * assign new Controls to the Blobb
 * \param controls
 */
void Blobb::setControls(Controls* controls) {
	m_Controls = controls;	// TODO: fix memory leak!
}

/*
 * evaluate Controls and reposition the Blobb
 */
void Blobb::update() {
	if(m_Controls->isRequested(Controls::UP)) moveForward();
	if(m_Controls->isRequested(Controls::DOWN)) moveBackward();
	if(m_Controls->isRequested(Controls::RIGHT)) moveRight();
	if(m_Controls->isRequested(Controls::LEFT)) moveLeft();
	if(m_Controls->isRequested(Controls::JUMP)) jump();
	m_Controls->consumed();		// important for MouseControls

	m_Translation->setTranslate(m_Position);
}

/*
 * \return the Blobb's sub-scene
 */
SO<SceneThing> Blobb::getScene() {
	return m_Scene;
}

/*
 * \return the Blobb's material per-vertex color
 */
Color Blobb::getColor() {
	return m_Material->getPerVertexColor();
}

/*
 * set the Blobb's material per-vertex color
 * \param color is the new color
 */
void Blobb::setColor(Color color) {
	m_Material->setPerVertexColor(color);
}

/*
 * lets the Blobb react on input
 * \param ie
 */
void Blobb::processInput(InputEvent* ie) {
	m_Controls->processInput(ie);
}
