#include "Menu.h"
#include "Game.h"
#include "Constants.h"
#include "ModelOptimizer.h"

#include <vrs/sg/scenething.h>
#include <vrs/camera.h>
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
#include <vrs/sg/pickingcallback.h>
#include <vrs/methodcallback.h>
#include <vrs/intersectioninfo.h>
#include <stdio.h>


/**
 * \ctor
 * \param game is the Game that is associated with the Menu
 */
BV3D::Menu::Menu(VRS::SO<Game> game) {
	// set default values
	mGame = game;
	mOptimizer = new ModelOptimizer();
	mGameIsPaused = false;
	mFont = 0;
	mMenuTextMaterial = 0;
	mMenuHitBoxMaterial = 0;

	// init settings
	mPlace = CITY;
	mRules = TIE_BREAK;
	mFullscreen = OFF;
	mP1Color = RED;
	mP2Color = GREEN;
	mP1Controls = KB_ARROWS;
	mP2Controls = AI;

	// set up local menu scene
	mScene = new VRS::SceneThing();
	mScene->append(new VRS::Camera(VRS::Vector(0.0,0.0,-5.0), VRS::Vector(), 30, 0.1, 100));
	mScene->append(new VRS::TransparencyTechniqueGL());
	mScene->append(new VRS::ShadowTechniqueGL());
	mScene->append(new VRS::AmbientLight(VRS::Color(0.5),1.0));

	// use a bright point light for nice specular reflection on the ball
	// use a dark point light for casting nice shadows of the menu items on the ball
	mScene->append(new VRS::PointLight(VRS::Vector(-3.0f,5.0f,-3.0f)));
	mScene->append(mLight = new VRS::PointLight(VRS::Vector(-1.0f,1.0f,-10.0f), VRS::Color(0.3)));

	// set up scene holding ball
	mBallScene = new VRS::SceneThing(mScene);
	mBallScene->append(new VRS::Shadowed(mLight));
	mBallScene->append(new VRS::Rotation(VRS::Vector(1.0,0.0,0.0),VRS::Vector(),90));
	mBallScene->append(mOptimizer->getWavefrontModel(BV3D::MODELS_PATH + "volleyball-colored.obj"));

	mCurrentMenuScene = new VRS::SceneThing(mScene);	// just create an empty scene for the menu screens

	// load Blobb model and set up subscene for the Blobb
	mBlobb = mOptimizer->get3dsObject(BV3D::MODELS_PATH + "blobb1.3ds", false, ModelOptimizer::NO_MATERIAL_NO_TEXTURES);
	mBlobbMaterial = new VRS::ShapeMaterialGL(VRS::Color(VRS::Color::blue), VRS::Color(0.5), 
		4.0, VRS::ShapeMaterialGL::AmbientAndDiffuse, VRS::Color(1.0), VRS::Color(0.5), VRS::Color(0.0), true);
	mBlobb->prepend(mBlobbMaterial);
	mBlobb->prepend(new VRS::Scaling(0.1f,0.1f,0.1f));
	mBlobb->prepend(new VRS::Translation(0.5,-0.25,-2.0));
	mBlobb->prepend(new VRS::ShadowCaster(mLight));

	// initialize picking callback for clicking on the menu items
	mPickingCallback = new VRS::PickingCallback(
		new VRS::MethodCallback1<BV3D::Menu, VRS::IntersectionInfo*>(this,&Menu::select));
}


/**
 * evaluates mouse clicks in the menu to perform menu tasks when menu items are clicked
 * \param info is a structure passed by the VRS::PickingCallback possibly holding the clicked-on shape (menu item)
 */
void BV3D::Menu::select(VRS::IntersectionInfo* info) {
	// get command string of clicked object
    const std::string& name = info->getShape()->getObjectName();

	// evaluate command string
	// - showMenu() is often called to just rebuild the current screen to reflect changes
	// - enums are often cycled to change options

	if(name.length()<=0)	// if no command string was specified no action will be performed
		return;

	else if(name.compare("Start")==0) {
		mGame->switchToGame(true);
		mGame->playSoundWhistle();
	}
	else if(name.compare("Resume")==0) {
		mGame->switchToGame(false);
		mGame->playSoundTouch();
	}

	else if(name.compare("Main")==0)
		showMenu(MAIN);
	else if(name.compare("Options")==0)
		showMenu(OPTIONS);
	else if(name.compare("Credits")==0)
		showMenu(CREDITS);
	else if(name.compare("Game")==0)
		showMenu(GAME);
	else if(name.compare("Player 1")==0)
		showMenu(P1);
	else if(name.compare("Player 2")==0)
		showMenu(P2);

	else if(name.compare("Fullscreen")==0) {
		mGame->toggleFullscreen();
		mFullscreen = (mFullscreen==OFF) ? ON : OFF;
		showMenu(GAME);
	}
	else if(name.compare("Place")==0) {
		mPlace = (PLACE)((mPlace+1) % MAX_PLACES);
		showMenu(GAME);
	}
	else if(name.compare("Rules")==0) {
		mRules = (RULES)((mRules+1) % MAX_RULES);
		showMenu(GAME);
	}
	else if(name.compare("P1Color")==0) {
		do {
			mP1Color = (COLOR)((mP1Color+1) % MAX_COLORS);
		} while(mP1Color==mP2Color);	// skip player 2's color
		showMenu(P1);
	}
	else if(name.compare("P2Color")==0) {
		do {
			mP2Color = (COLOR)((mP2Color+1) % MAX_COLORS);
		} while(mP1Color==mP2Color);	// skip player 1's color
		showMenu(P2);
	}
	else if(name.compare("P1Controls")==0) {
		mP1Controls = (CONTROLS)((mP1Controls+1) % MAX_CONTROLS);
		showMenu(P1);
	}
	else if(name.compare("P2Controls")==0) {
		mP2Controls = (CONTROLS)((mP2Controls+1) % MAX_CONTROLS);
		showMenu(P2);
	}

	else if(name.compare("Quit")==0)
		exit(0);
	else
		return;
}


/**
 * creates the specified menu screen
 */
void BV3D::Menu::showMenu(MENUSCREEN screen) {
	static char buffer[50];		// buffer for menu item text construction
	mGame->playSoundTouch();	// play sound on screen switch (also when refreshing options!)
	mCurrentMenuScene->clear();	// remove current menu items
	mCurrentMenuScene->setLocalMatrix(VRS::Matrix());	// reset screen matrix (center screen for first menu item)

	switch(screen) {
		case MAIN:
			if(mGameIsPaused) {
				addText("Resume");
				addText("Start New Game","Start");
			}
			else	// in case game is over...
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
			color = VRS::Color(color[0],color[1],color[2],BV3D::BLOBB_ALPHA);
			mBlobbMaterial->setPerVertexColor(color);
			mCurrentMenuScene->append(mBlobb);	// append blobb after all text
			break;}
		case P2: {
			addText("Color:", "P2Color");
			sprintf_s(buffer,50,"Controls: %s",controls()[mP2Controls]);
			addText(buffer,"P2Controls");
			addText("Back", "Options");
			VRS::Color color(colors()[mP2Color]);
			color = VRS::Color(color[0],color[1],color[2],BV3D::BLOBB_ALPHA);
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
}


/**
 * initializes the main menu screen
 * this method is called by Game before showing the menu
 * \param paused indicates, whether the game is over or if it is just paused. This information is used to display either "Start" or "Resume" and "Start New Game".
 */
void BV3D::Menu::showMainMenu(bool paused) {
	mGameIsPaused = paused;
	showMenu(MAIN);
}


/**
 * adds a menu item (text) to the menu and vertically centers the whole menu on screen
 * \param label is the displayed text for the menu item
 * \param command is a command string evaluated when the menu item is clicked. if command is 0 label will be used instead of command.
 */
void BV3D::Menu::addText(const char* label, const char* command) {
	// create font and shape materials if necessary
	if(!mFont)
		mFont = VRS::Font::searchFont("Arial (TrueType)", VRS::Font::EXTRUDE, 200);
	if(!mMenuTextMaterial)
		mMenuTextMaterial = new VRS::ShapeMaterialGL(VRS::Color(0.2,0.2,1.0,0.6));
	if(!mMenuHitBoxMaterial)
		mMenuHitBoxMaterial = new VRS::ShapeMaterialGL(VRS::Color(1.0,1.0,1.0,0.0));

	// create menu item text
	VRS::SO<VRS::Text> text = new VRS::Text(mFont, label, VRS::Vector(), VRS::Vector(0.2, 0.2, 0.1),
		VRS::Text::XY_PLANE, VRS::Text::CENTER_H, VRS::Text::TOP);
	text->setObjectName( (command==0) ? label : command);	// set command (use label if command not specified)

	// create hit box (to avoid having to click exactly on the text)
	VRS::SO<VRS::Box> hitbox = new VRS::Box(text->boundingBox());
	hitbox->setObjectName( (command==0) ? label : command);

	// pack text and hitbox into a scene
	VRS::SO<VRS::SceneThing> scene = new VRS::SceneThing();
	scene->append(new VRS::Translation(0.0,0.0,-2.0));		// move in front of ball
	double width = text->boundingBox().diagonal()[0];
	if(width>2)	// scale text horizontally to fit view if necessary
		scene->append(new VRS::Scaling(2.0f/width, 1.0, 1.0));
	scene->append(mMenuHitBoxMaterial);
	scene->append(hitbox);
	scene->append(new VRS::ShadowCaster(mLight));
	scene->append(mMenuTextMaterial);
	scene->append(text);

	// append scene below current menu scene and re-center current menu scene
	// leave 0.1 space between hitboxes
	double hitboxHeight = hitbox->boundingBox().diagonal()[1];
	VRS::Matrix matrix = mCurrentMenuScene->getLocalMatrix();
	double menuHeightOffset = matrix.element(1,3);
	scene->setLocalMatrix(VRS::Matrix::translation(VRS::Vector(0.0,-2 * menuHeightOffset,0.0)));
	mCurrentMenuScene->append(scene);
	menuHeightOffset += hitboxHeight * 1.2 / 2;		//
	matrix.setElement(1,3,menuHeightOffset);
	mCurrentMenuScene->setLocalMatrix(matrix);
}


/**
 * \param team indicates the Blobb
 * \return the VRS::Color for the corresponding Blobb
 */
VRS::Color BV3D::Menu::getColor(BV3D::TEAM team) {
	VRS::Color color = colors()[(team) ? mP2Color : mP1Color];
	return VRS::Color(color[0],color[1],color[2],BV3D::BLOBB_ALPHA);
}
