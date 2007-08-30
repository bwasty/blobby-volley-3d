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
#include <vrs/ambientlight.h>
#include <vrs/pointlight.h>
#include <vrs/opengl/transparencytechniquegl.h>
#include <vrs/color.h>
#include <vrs/vector.h>
#include <vrs/opengl/shapematerialgl.h>
#include <vrs/font.h>
#include <vrs/text.h>
#include <vrs/box.h>
#include <stdio.h>

// tmp
#include <vrs/box.h>

BV3D::Menu::Menu(VRS::SO<Game> game) {
	mGame = game;
	mIsActive = false;

	mScene = new VRS::SceneThing();
	mScene->append(new VRS::TransparencyTechniqueGL());
	mScene->append(new VRS::Camera(VRS::Vector(0.0,0.0,-5.0), VRS::Vector(), 30, 0.1, 10));
	mScene->append(new VRS::AmbientLight(VRS::Color(0.5),1.0));
	mScene->append(new VRS::PointLight(VRS::Vector(-2.0,3.0,-5)));

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

		if(name.compare("play")==0) {mIsActive=false; mGame->switchToGame(false);}
		if(name.compare("quit")==0) exit(0);
    }
}

void BV3D::Menu::showMainMenu() {
	// indicate that menu wants to process input events
	mIsActive = true;

	mCurrentMenuScene->clear();

	VRS::SO<VRS::Font> font = VRS::Font::searchFont("Arial (TrueType)", VRS::Font::EXTRUDE, 50);


	VRS::SO<VRS::Text> text = new VRS::Text(font, "Play", VRS::Vector(0.0, 0.3, 0.0), VRS::Vector(0.5, 0.5, 0.5),
		VRS::Text::XY_PLANE, VRS::Text::CENTER_H, VRS::Text::CENTER_V);
	text->setObjectName("play");
	mCurrentMenuScene->append(new VRS::ShapeMaterialGL(VRS::Color(0.0,0.0,1.0,0.8)));
	mCurrentMenuScene->append(text);

	VRS::SO<VRS::Box> box = new VRS::Box(text->boundingBox());
	box->setObjectName("play");
	mCurrentMenuScene->append(new VRS::ShapeMaterialGL(VRS::Color(1.0,1.0,1.0,0.0)));
	mCurrentMenuScene->append(box);


	text = new VRS::Text(font, "Quit", VRS::Vector(0.0, -0.3, 0.0), VRS::Vector(0.5, 0.5, 0.5),
		VRS::Text::XY_PLANE, VRS::Text::CENTER_H, VRS::Text::CENTER_V);
	text->setObjectName("quit");
	mCurrentMenuScene->append(new VRS::ShapeMaterialGL(VRS::Color(0.0,0.0,1.0,0.8)));
	mCurrentMenuScene->append(text);

	box = new VRS::Box(text->boundingBox());
	box->setObjectName("quit");
	mCurrentMenuScene->append(new VRS::ShapeMaterialGL(VRS::Color(1.0,1.0,1.0,0.0)));
	mCurrentMenuScene->append(box);
}
