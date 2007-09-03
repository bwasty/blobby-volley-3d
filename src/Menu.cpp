/**
 * Menu.cpp
 *
 * class BV3D::Menu implements the menu for bv3d
 *
 */

#include "Menu.h"
#include "Game.h"

#include <vrs/sg/scenething.h>
#include <vrs/camera.h>
#include <vrs/sg/selector.h>
#include <vrs/rayrequest.h>
#include <vrs/rayrequesttechnique.h>
#include <vrs/methodcallback.h>
#include <vrs/perspective.h>
#include <vrs/ambientlight.h>
#include <vrs/pointlight.h>
#include <vrs/opengl/transparencytechniquegl.h>
#include <vrs/color.h>
#include <vrs/vector.h>
#include <vrs/opengl/shapematerialgl.h>
#include <vrs/font.h>
#include <vrs/text.h>
#include <vrs/box.h>
#include <vrs/scaling.h>
#include <vrs/translation.h>
#include <gl/glut.h>
#include <stdio.h>


BV3D::Menu::Menu(VRS::SO<Game> game) {
	mGame = game;
	mFont = 0;
	mMenuTextMaterial = 0;
	mMenuHitBoxMaterial = 0;

	mScene = new VRS::SceneThing();
	mScene->append(new VRS::Camera(VRS::Vector(0.0,0.0,-10.0), VRS::Vector(), 30, 0.1, 100));
	mScene->append(new VRS::TransparencyTechniqueGL());
	mScene->append(new VRS::AmbientLight(VRS::Color(0.5),1.0));
	mScene->append(new VRS::PointLight(VRS::Vector(-2.0,3.0,-5.0)));

	mCurrentMenuScene = new VRS::SceneThing(mScene);

	mSelector = new VRS::Selector(VRS::InputEvent::MouseButton1,
		VRS::InputEvent::NoModifier,VRS::InputEvent::NoModifier,
		mRayRequest = new VRS::RayRequest(VRS::RayRequestTechnique::Nearest),0,0,
		new VRS::MethodCallback<Menu>(this,&Menu::select),0,0);
}

void BV3D::Menu::select() {
	// TODO: deactivate menu when leaving

    if(mRayRequest->results()>0) {
		VRS::SO<VRS::IntersectionInfo> info = mRayRequest->result(0);

        const std::string& name = info->getShape()->getObjectName();

		if(name.compare("Play")==0) mGame->switchToGame(false);
		if(name.compare("Main")==0) showMainMenu();
		if(name.compare("Options")==0) showOptionsMenu();
		if(name.compare("Toggle Fullscreen")==0) glutFullScreen();
		if(name.compare("Quit")==0) exit(0);
    }
}

void BV3D::Menu::showOptionsMenu() {
	mCurrentMenuScene->clear();
	mCurrentMenuScene->setLocalMatrix(VRS::Matrix());

	addText("Toggle Fullscreen");
	addText("Back","Main");
}

void BV3D::Menu::showMainMenu() {
	mCurrentMenuScene->clear();
	mCurrentMenuScene->setLocalMatrix(VRS::Matrix());

	addText("Play");
	addText("Options");
	addText("Quit");
}


void BV3D::Menu::addText(const char* label, const char* command) {
	// create fonts and shape materials if necessary
	if(!mFont)
		mFont = VRS::Font::searchFont("Arial (TrueType)", VRS::Font::EXTRUDE, 200);
	if(!mMenuTextMaterial)
		mMenuTextMaterial = new VRS::ShapeMaterialGL(VRS::Color(0.0,0.0,1.0,0.8));
	if(!mMenuHitBoxMaterial)
		mMenuHitBoxMaterial = new VRS::ShapeMaterialGL(VRS::Color(1.0,1.0,1.0,0.0));

	// create menu item text
	VRS::SO<VRS::Text> text = new VRS::Text(mFont, label, VRS::Vector(), VRS::Vector(1.0, 1.0, 1.0),
		VRS::Text::XY_PLANE, VRS::Text::CENTER_H, VRS::Text::TOP);
	text->setObjectName( (command==0) ? label : command);

	// create hit box
	VRS::SO<VRS::Box> hitbox = new VRS::Box(text->boundingBox());
	hitbox->setObjectName( (command==0) ? label : command);

	// pack text and hitbox into scene
	VRS::SO<VRS::SceneThing> scene = new VRS::SceneThing();
	scene->append(mMenuTextMaterial);
	scene->append(text);
	scene->append(mMenuHitBoxMaterial);
	scene->append(hitbox);

	// physically append scene below current menu scene and re-center current menu scene
	double hitboxHeight = hitbox->boundingBox().diagonal()[1];
	VRS::Matrix matrix = mCurrentMenuScene->getLocalMatrix();
	double menuHeightOffset = matrix.element(1,3);
	scene->setLocalMatrix(VRS::Matrix::translation(VRS::Vector(0.0,-2 * menuHeightOffset,0.0)));
	mCurrentMenuScene->append(scene);
	menuHeightOffset += hitboxHeight * 1.2 / 2;
	matrix.setElement(1,3,menuHeightOffset);
	mCurrentMenuScene->setLocalMatrix(matrix);
}
