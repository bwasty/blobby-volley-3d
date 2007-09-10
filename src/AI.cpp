#include "AI.h"
#include "Game.h"
#include "Blobb.h"
#include <Newton.h>
#include "Referee.h"
#include "Ball.h"
#include "Arena.h"


BV3D::AI::AI(BV3D::Game* game) {
	mGame = game;
}

void BV3D::AI::aiServe(BV3D::BV3D_TEAM team) {
	int teamModifier = team==BV3D::BV3D_TEAM1 ? -1 : 1;
	float random = ((rand() % 13) - 6.0) / 10; // from -0.6 to 0.6
	mGame->getBlobb(team)->setPosition(VRS::Vector(teamModifier*(BV3D::arenaExtent[0]/4+1.3+random/6), 0.0,random));
	mGame->getBlobb(team)->maxJump();
}


int BV3D::AI::AiCallback(const NewtonMaterial* material, const NewtonContact* contact) {
	AI* this_ = (AI*)NewtonMaterialGetMaterialPairUserData(material);

	if (!this_->getGame()->getReferee()->getActive())
		return 0;

	NewtonBody* body = this_->getGame()->getBall()->getBody();

	// determine on which side the ball is (which team's AI is currently active)
	int teamModifier;
	BV3D::BV3D_TEAM team;
	if (this_->getGame()->getBall()->getPosition()[0] < 0) {
		teamModifier = -1;
		team = BV3D::BV3D_TEAM1;
	}
	else {
		teamModifier = 1;
		team = BV3D::BV3D_TEAM2;
	}

	dFloat v[3];
	NewtonBodyGetVelocity(body, v);
	VRS::Vector v_(v[0], v[1], v[2]);

	// accelerate the ball more than usual if it is slow
	int speedUp = 1;
	if (v_.abs() < 10) // TODO: tune
		speedUp = 2.5;
	printf("ball speed: %f\n", v_.abs());

	int random = rand() % 15;
	if (random < 14) {
		// move blobb to contact point
		dFloat pos[3], norm[3];
		NewtonMaterialGetContactPositionAndNormal(material, pos, norm);
		//printf("contact position: %f, %f, %f\n", pos[0], pos[1], pos[2]);
		this_->getGame()->getBlobb(team)->setPosition(Vector(pos[0], 0, pos[2]));

		if (v[0]*teamModifier < 0) { // ball moves toward the net -> use normal collision with random elasticity
			NewtonMaterialSetContactElasticity(material, (0.9+(rand()%9)/10.0)*speedUp);
			this_->getGame()->getReferee()->ballOnBlobb(team);
			this_->getGame()->playSoundTouch();
			return 1;
		}
		else {
			random = rand() % 10;
			if (random<5 || (this_->getGame()->getReferee()->getCurrentContacts(team)>1 && v[0]*teamModifier > 0)) { // reverse balls direction
				random = (rand()%10) / 10.0;
				v[0] = - v[0]*(0.8+random)*speedUp;
				v[1] = - v[1]*(0.8+random)*speedUp;
				v[2] = - v[2]*(0.8+random)*speedUp;
				NewtonBodySetVelocity(body, v);
				this_->getGame()->getReferee()->ballOnBlobb(team);
				this_->getGame()->playSoundTouch();
				return 0;
			}
			else if (random <10) { // normal collision with random elasticity
				random = rand()%9;
				NewtonMaterialSetContactElasticity(material, (0.9+random/10)*speedUp);
				this_->getGame()->getReferee()->ballOnBlobb(team);
				this_->getGame()->playSoundTouch();
				return 1;
			}
		}
	}
	else {
		NewtonMaterialSetDefaultCollidable(this_->getGame()->getArena()->getWorld(), 
			this_->getGame()->getArena()->getBallMaterialID(), 
			this_->getGame()->getArena()->getAITriggerID(), 
			0);
		return 0;
	}
}