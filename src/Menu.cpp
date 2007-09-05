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
#include <vrs/io/threedsreader.h>
#include <stdio.h>


BV3D::Menu::Menu(VRS::SO<Game> game) {
	mGame = game;
	mGameIsPaused = false;
	mFont = 0;
	mMenuTextMaterial = 0;
	mMenuHitBoxMaterial = 0;

	// init settings
	mCurrentScreen = MAIN;
	mPlace = BEACH;
	mRules = CLASSIC;
	mFullscreen = OFF;
	mP1Color = RED;
	mP2Color = GREEN;
	mP1Controls = KB_ARROWS;
	mP2Controls = MOUSE;

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

	VRS::ThreeDSReader::setMaterialMode(VRS::ThreeDSReader::NO_MATERIAL);
	mBlobb = VRS::ThreeDSReader::readObject(BV3D::threeDSPath + "blobb1.3ds");	// TODO: exception handling
	mBlobbMaterial = new VRS::ShapeMaterialGL(VRS::Color(VRS::Color::blue), VRS::Color(0.5), 
		4.0, VRS::ShapeMaterialGL::AmbientAndDiffuse, VRS::Color(1.0), VRS::Color(0.5), VRS::Color(0.0), true);
	mBlobb->prepend(mBlobbMaterial);
	mBlobb->prepend(new VRS::Scaling(0.1f,0.1f,0.1f));
	mBlobb->prepend(new VRS::Translation(0.5,-0.25,-2.0));
	mBlobb->prepend(new VRS::ShadowCaster(mLight));

	mSelector = new VRS::Selector(VRS::InputEvent::MouseButton1,
		VRS::InputEvent::NoModifier,VRS::InputEvent::NoModifier,
		mRayRequest = new VRS::RayRequest(VRS::RayRequestTechnique::Nearest),0,0,
		new VRS::MethodCallback<Menu>(this,&Menu::select),0,0);
}

void BV3D::Menu::select() {
    if(mRayRequest->results()>0) {
		VRS::SO<VRS::IntersectionInfo> info = mRayRequest->result(0);

        const std::string& name = info->getShape()->getObjectName();

		if(name.compare("Start")==0) {
			mGame->switchToGame(true); mGame->playSoundWhistle();}
		else if(name.compare("Resume")==0) {
			mGame->switchToGame(false); mGame->playSoundTouch();}
		else if(name.compare("Main")==0)	showMenu(MAIN);
		else if(name.compare("Options")==0)	showMenu(OPTIONS);
		else if(name.compare("Credits")==0)	showMenu(CREDITS);
		else if(name.compare("Game")==0)	showMenu(GAME);
		else if(name.compare("Player 1")==0)	showMenu(P1);
		else if(name.compare("Player 2")==0)	showMenu(P2);
		else if(name.compare("Fullscreen")==0) {
			mGame->toggleFullscreen();
			mFullscreen = (mFullscreen==OFF) ? ON : OFF;
			mGame->playSoundWhistle();
			showMenu(GAME);}
		else if(name.compare("Place")==0) {
			mPlace = (PLACE)((mPlace+1) % MAX_PLACES);
			mGame->playSoundWhistle();
			showMenu(GAME);}
		else if(name.compare("Rules")==0) {
			mRules = (RULES)((mRules+1) % MAX_RULES);
			mGame->playSoundWhistle();
			showMenu(GAME);}
		else if(name.compare("P1Color")==0) {
			mP1Color = (COLOR)((mP1Color+1) % MAX_COLORS);
			mGame->playSoundWhistle();
			showMenu(P1);}
		else if(name.compare("P2Color")==0) {
			mP2Color = (COLOR)((mP2Color+1) % MAX_COLORS);
			mGame->playSoundWhistle();
			showMenu(P2);}
		else if(name.compare("P1Controls")==0) {
			mP1Controls = (CONTROLS)((mP1Controls+1) % MAX_CONTROLS);
			mGame->playSoundWhistle();
			showMenu(P1);}
		else if(name.compare("P2Controls")==0) {
			mP2Controls = (CONTROLS)((mP2Controls+1) % MAX_CONTROLS);
			mGame->playSoundWhistle();
			showMenu(P2);}
		else if(name.compare("Quit")==0) exit(0);
		else return;
    }
}

void BV3D::Menu::showMenu(MENUSCREEN screen) {
	static char buffer[50];
	if(screen!=mCurrentScreen)
		mGame->playSoundTouch();
	mCurrentMenuScene->clear();
	mCurrentMenuScene->setLocalMatrix(VRS::Matrix());

	switch(screen) {
		case MAIN:
			if(mGameIsPaused) {
				addText("Resume");
				addText("Restart","Start");}
			else
				addText("Start");
			addText("Options");
			addText("Credits");
			addText("Quit");
			break;
		case OPTIONS:
			addText("Game");
			addText("Player 1");
			addText("Player 2");
			addText("Back","Main");
			break;
		case GAME:
			sprintf_s(buffer,50,"Place: %s",places()[mPlace]);
			addText(buffer, "Place");
			sprintf_s(buffer,50,"Rules: %s",rules()[mRules]);
			addText(buffer, "Rules");
			sprintf_s(buffer,50,"Fullscreen: %s",fullscreen()[mFullscreen]);
			addText(buffer,"Fullscreen");
			addText("Back","Options");
			break;
		case P1: {
			addText("Color:", "P1Color");
			sprintf_s(buffer,50,"Controls: %s",controls()[mP1Controls]);
			addText(buffer,"P1Controls");
			addText("Back", "Options");
			VRS::Color color(colors()[mP1Color]);
			color = VRS::Color(color[0],color[1],color[2],BV3D::blobbAlpha);
			mBlobbMaterial->setPerVertexColor(color);
			mCurrentMenuScene->append(mBlobb);	// append blobb after all text
			break;}
		case P2: {
			addText("Color:", "P2Color");
			sprintf_s(buffer,50,"Controls: %s",controls()[mP2Controls]);
			addText(buffer,"P2Controls");
			addText("Back", "Options");
			VRS::Color color(colors()[mP2Color]);
			color = VRS::Color(color[0],color[1],color[2],BV3D::blobbAlpha);
			mBlobbMaterial->setPerVertexColor(color);
			mCurrentMenuScene->append(mBlobb);	// append blobb after all text
			break;}
		case CREDITS:
			addText("Benjamin HosainWasty");
			addText("Philipp Maschke");
			addText("Erik Schleiff");
			addText("Back", "Main");
			break;
		default:
			addText("Menu Error!");
			addText("Quit");
			break;
	}
	mCurrentScreen = screen;
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
	VRS::SO<VRS::Text> text = new VRS::Text(mFont, label, VRS::Vector(), VRS::Vector(0.2, 0.2, 0.1),
		VRS::Text::XY_PLANE, VRS::Text::CENTER_H, VRS::Text::TOP);
	if(text->boundingBox().diagonal()[0]>2)
		text = new VRS::Text(mFont, label, VRS::Vector(), VRS::Vector(0.2, 0.2, 0.1),
			VRS::Text::XY_PLANE, VRS::Text::CENTER_H, VRS::Text::TOP);
	text->setObjectName( (command==0) ? label : command);

	// create hit box
	VRS::SO<VRS::Box> hitbox = new VRS::Box(text->boundingBox());
	hitbox->setObjectName( (command==0) ? label : command);

	// pack text and hitbox into scene
	VRS::SO<VRS::SceneThing> scene = new VRS::SceneThing();
	scene->append(new VRS::Translation(0.0,0.0,-2.0));
	double width = text->boundingBox().diagonal()[0];
	if(width>2) scene->append(new VRS::Scaling(2.0f/width, 1.0, 1.0));
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
