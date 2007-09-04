/**
 * Menu.cpp
 *
 * class BV3D::Menu implements the menu for bv3d
 *
 */

#include "Menu.h"
#include "Game.h"
#include "Constants.h"

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
#include <vrs/opengl/shadowtechniquegl.h>
#include <vrs/shadowed.h>
#include <vrs/shadowcaster.h>
#include <vrs/color.h>
#include <vrs/vector.h>
#include <vrs/opengl/shapematerialgl.h>
#include <vrs/font.h>
#include <vrs/text.h>
#include <vrs/box.h>
#include <vrs/translation.h>
#include <vrs/scaling.h>
#include <vrs/rotation.h>
#include <vrs/io/filedataresource.h>
#include <vrs/io/wavefrontreader.h>
#include <stdio.h>


BV3D::Menu::Menu(VRS::SO<Game> game) {
	mGame = game;
	mGameIsPaused = false;
	mFont = 0;
	mMenuTextMaterial = 0;
	mMenuHitBoxMaterial = 0;

	mScene = new VRS::SceneThing();
	mScene->append(new VRS::Camera(VRS::Vector(0.0,0.0,-5.0), VRS::Vector(), 30, 0.1, 100));
	mScene->append(new VRS::TransparencyTechniqueGL());
	mScene->append(new VRS::ShadowTechniqueGL());
	mScene->append(new VRS::AmbientLight(VRS::Color(0.5),1.0));
	mScene->append(new VRS::PointLight(VRS::Vector(-3.0f,5.0f,-3.0f)));
	mScene->append(mLight = new VRS::PointLight(VRS::Vector(-1.0f,1.0f,-10.0f), VRS::Color(0.3)));

	mBallScene = new VRS::SceneThing(mScene);
	VRS::ID id = VRS::ID("ball");
	VRS::WavefrontReader reader = VRS::WavefrontReader();
	VRS::SO<VRS::FileDataResource> file = new VRS::FileDataResource(BV3D::threeDSPath + "volleyball-colored.obj");
	mBallScene->append(new VRS::Shadowed(mLight));
	mBallScene->append(new VRS::Rotation(VRS::Vector(1.0,0.0,0.0),VRS::Vector(),90));
	mBallScene->append(reader.read(file, id));

	mCurrentMenuScene = new VRS::SceneThing(mScene);

	mSelector = new VRS::Selector(VRS::InputEvent::MouseButton1,
		VRS::InputEvent::NoModifier,VRS::InputEvent::NoModifier,
		mRayRequest = new VRS::RayRequest(VRS::RayRequestTechnique::Nearest),0,0,
		new VRS::MethodCallback<Menu>(this,&Menu::select),0,0);
}

void BV3D::Menu::select() {
    if(mRayRequest->results()>0) {
		VRS::SO<VRS::IntersectionInfo> info = mRayRequest->result(0);

        const std::string& name = info->getShape()->getObjectName();

		if(name.compare("Start")==0) mGame->switchToGame(true);
		else if(name.compare("Resume")==0) mGame->switchToGame(true);
		else if(name.compare("Main")==0) showMenu(MAIN);
		else if(name.compare("Options")==0) showMenu(OPTIONS);
		else if(name.compare("Toggle Fullscreen")==0) mGame->toggleFullscreen();
		else if(name.compare("Quit")==0) exit(0);
    }
}

void BV3D::Menu::showMenu(MENUSCREEN screen) {
	mCurrentMenuScene->clear();
	mCurrentMenuScene->setLocalMatrix(VRS::Matrix());

	switch(screen) {
		case MAIN:
			if(mGameIsPaused) {
				addText("Resume");
				addText("Restart","Start");
			}
			else
				addText("Start");
			addText("Options");
			addText("Quit");
			break;
		case OPTIONS:
			addText("Toggle Fullscreen");
			addText("Back","Main");
			break;
		default:
			addText("Menu Error!");
			addText("Quit");
			break;
	}
}

void BV3D::Menu::showMainMenu(bool paused) {
	mGameIsPaused = paused;
	showMenu(MAIN);
}


void BV3D::Menu::addText(const char* label, const char* command) {
	// create fonts and shape materials if necessary
	if(!mFont)
		mFont = VRS::Font::searchFont("Arial (TrueType)", VRS::Font::EXTRUDE, 200);
	if(!mMenuTextMaterial)
		mMenuTextMaterial = new VRS::ShapeMaterialGL(VRS::Color(0.2,0.2,1.0,0.6));
	if(!mMenuHitBoxMaterial)
		mMenuHitBoxMaterial = new VRS::ShapeMaterialGL(VRS::Color(1.0,1.0,1.0,0.0));

	// create menu item text
	VRS::SO<VRS::Text> text = new VRS::Text(mFont, label, VRS::Vector(), VRS::Vector(0.2, 0.2, 0.2),
		VRS::Text::XY_PLANE, VRS::Text::CENTER_H, VRS::Text::TOP);
	text->setObjectName( (command==0) ? label : command);

	// create hit box
	VRS::SO<VRS::Box> hitbox = new VRS::Box(text->boundingBox());
	hitbox->setObjectName( (command==0) ? label : command);

	// pack text and hitbox into scene
	VRS::SO<VRS::SceneThing> scene = new VRS::SceneThing();
	scene->append(new VRS::Translation(0.0,0.0,-2.0));
	scene->append(mMenuHitBoxMaterial);
	scene->append(hitbox);
	scene->append(new VRS::ShadowCaster(mLight));
	scene->append(mMenuTextMaterial);
	scene->append(text);

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
