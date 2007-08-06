/// Ball.cpp

//#include "Physics.h"
#include "Ball.h"
#include <vrs/opengl/shapematerialgl.h>
#include <vrs/sphere.h>

BV3D::Ball::Ball() {
	m_radius = 1.0;
	m_Scene = new VRS::SceneThing();
	m_Scene->append(new VRS::ShapeMaterialGL(VRS::Color(1.0,1.0,0.0)));
	m_Scene->append(new VRS::Sphere(m_radius));
}

BV3D::Ball::~Ball() {
}

void BV3D::Ball::setTransformation(VRS::Matrix matrix){
	m_Scene->setLocalMatrix(matrix);
}
