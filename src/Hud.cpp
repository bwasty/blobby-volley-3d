/**
 * Hud.cpp
 *
 * class BV3D::HUD represents the heads-up display for blobby volley 3d.
 * its main task is to display the score during the game.
 * it also show win-messages
 */

#include "Hud.h"
#include <vrs/opengl/overlayimagegl.h>
#include <vrs/font.h>
#include "Constants.h"
#include <stdio.h>

BV3D::HUD::HUD(VRS::Color colorPlayer1, VRS::Color colorPlayer2) {
	setColors(colorPlayer1, colorPlayer2);

	mScene =  new VRS::OverlayImageGL();
	VRS::SO<VRS::OverlayImageGL> overlay = VRS_Cast(VRS::OverlayImageGL, mScene);

	mScoreP1Index = -1;
	mScoreP2Index = -1;
	mServingIndex = -1;
	mWinnerIndex = -1;
}

void BV3D::HUD::setColors(VRS::Color colorPlayer1, VRS::Color colorPlayer2) {
	// store colors internally without transparency
	mP1Color = VRS::Color(colorPlayer1[0],colorPlayer1[1],colorPlayer1[2]);
	mP2Color = VRS::Color(colorPlayer2[0],colorPlayer2[1],colorPlayer2[2]);
}

void BV3D::HUD::setScore(int scoreP1, int scoreP2, TEAM servingTeam) {
	VRS::SO<VRS::OverlayImageGL> overlay = VRS_Cast(VRS::OverlayImageGL, mScene);
	if(mScoreP1Index>=0) overlay->removeOverlayItem(mScoreP1Index);
	if(mScoreP2Index>=0) overlay->removeOverlayItem(mScoreP2Index);
	if(mServingIndex>=0) overlay->removeOverlayItem(mServingIndex);
	if(mWinnerIndex>=0) overlay->removeOverlayItem(mWinnerIndex);

	// set up player 1 score
	char buffer[5];
	sprintf_s(buffer,4, "%d",scoreP1);
	mScoreP1Index = overlay->addText(VRS::Font::searchFont("Arial Black (TrueType)", VRS::Font::TEXTURE, 150),
		buffer,VRS::Vector(0.2,0.95),VRS::Vector(0.35,1.0),VRS::Text::LEFT,VRS::Text::TOP,mP1Color);

	// set up player 2 score
	sprintf_s(buffer,4, "%d",scoreP2);
	mScoreP2Index = overlay->addText(VRS::Font::searchFont("Arial Black (TrueType)", VRS::Font::TEXTURE, 150),
		buffer,VRS::Vector(0.65,0.95),VRS::Vector(0.8,1.0),VRS::Text::LEFT,VRS::Text::TOP,mP2Color);

	// set up serving indicator
	VRS::Vector servingMin, servingMax;
	if(servingTeam==TEAM1) {
		servingMin = VRS::Vector(0.10,0.95); servingMax = VRS::Vector(0.15,1.0);}
	else if(servingTeam==TEAM2) {
		servingMin = VRS::Vector(0.85,0.95); servingMax = VRS::Vector(0.9,1.0);}
	mServingIndex = overlay->addText(VRS::Font::searchFont("Arial Black (TrueType)", VRS::Font::TEXTURE, 150),
		"!",servingMin,servingMax,VRS::Text::LEFT,VRS::Text::TOP,VRS::Color(0.8,0.0,0.0));
}

void BV3D::HUD::showWinner(TEAM winner) {
	char buffer[15];
	sprintf_s(buffer,15, "Player %d wins!",(int)winner+1);
	VRS::SO<VRS::OverlayImageGL> overlay = VRS_Cast(VRS::OverlayImageGL, mScene);
	if(mWinnerIndex>=0) overlay->removeOverlayItem(mWinnerIndex);
	mWinnerIndex = overlay->addText(VRS::Font::searchFont("Arial Black (TrueType)", VRS::Font::TEXTURE, 150),
		buffer,VRS::Vector(0.1,0.45),VRS::Vector(0.9,0.65),VRS::Text::LEFT,VRS::Text::CENTER_V,VRS::Color(0.0,0.0,1.0));
}
