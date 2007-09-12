#include "AI.h"
#include "Game.h"
#include "Blobb.h"
#include <Newton.h>
#include "Referee.h"
#include "Ball.h"
#include "Arena.h"
#include <time.h>

BV3D::AI::AI(BV3D::Game* game) {
	srand((int)time(0)); // because AI uses random numbers to select between a number of possible reactions
	mGame = game;
	mAiTriggerBody[0] = mAiTriggerBody[1] = NULL;
}

/**
 * switch AI on for given team by creating AI trigger plane (a physical plane the ball collides with, but not the blobbs)
 */
void BV3D::AI::enableAI(BV3D::TEAM team) {
	if (mAiTriggerBody[team==BV3D::TEAM1 ? 0 : 1] == NULL) {
		int teamModifier = (team == BV3D::TEAM1) ? -1 : 1;
		dFloat matrix[16] = {1.0,0.0,0.0,0.0, 0.0,1.0,0.0,0.0, 0.0,0.0,1.0,0.0, 0.0,0.0,0.0,1.0};

		matrix[12] = (dFloat)BV3D::ARENA_EXTENT.get(0)/4 * teamModifier;
		matrix[13] = (dFloat)BV3D::BLOBB_HEIGHT+0.3f;
		matrix[14] = 0.0;
		NewtonCollision* AiTriggerCollision = NewtonCreateBox(mGame->getArena()->getWorld(), (dFloat)(BV3D::ARENA_EXTENT.get(0)/2 - 0.2f), 0, (dFloat)BV3D::ARENA_EXTENT.get(2), matrix);
		mAiTriggerBody[team==BV3D::TEAM1 ? 0 : 1] = NewtonCreateBody(mGame->getArena()->getWorld(), AiTriggerCollision);
		NewtonBodySetMaterialGroupID(mAiTriggerBody[team==BV3D::TEAM1 ? 0 : 1], mGame->getArena()->getAiTriggerID());
		NewtonReleaseCollision(mGame->getArena()->getWorld(), AiTriggerCollision);
	}
}

void BV3D::AI::disableAI(BV3D::TEAM team) {
	NewtonDestroyBody(mGame->getArena()->getWorld(), mAiTriggerBody[team==BV3D::TEAM1 ? 0 : 1]);
	mAiTriggerBody[team==BV3D::TEAM1 ? 0 : 1] = NULL;
}

/**
 * lets the blobb specified by team do a serve
 */
void BV3D::AI::aiServe(BV3D::TEAM team) {
	int teamModifier = team==BV3D::TEAM1 ? -1 : 1;
	float random = ((rand() % 13) - 6.0f) / 10.0f; // from -0.6 to 0.6
	mGame->getBlobb(team)->setPosition(VRS::Vector(teamModifier*(BV3D::ARENA_EXTENT[0]/4+1.3+random/6), 0.0,random));
	mGame->getBlobb(team)->maxJump();
}

int BV3D::AI::AiCallback(const NewtonMaterial* material, const NewtonContact* contact) {
	CollisionData* collData = (CollisionData*)NewtonMaterialGetMaterialPairUserData(material);

	if (!collData->referee->getActive())
		return 0;

	NewtonBody* body = collData->ball->getBody();

	// determine on which side the ball is (which team's AI is currently active)
	int teamModifier;
	BV3D::TEAM team;
	if (collData->ball->getPosition()[0] < 0) {
		teamModifier = -1;
		team = BV3D::TEAM1;
	}
	else {
		teamModifier = 1;
		team = BV3D::TEAM2;
	}

	dFloat v[3];
	NewtonBodyGetVelocity(body, v);
	VRS::Vector v_(v[0], v[1], v[2]);

	// accelerate the ball more than usual if it is slow
	float speedUp = 1;
	if (v_.abs() < 10)
		speedUp = 2.5;

	int random = rand() % 15;
	if (random < 14) {
		// move blobb to contact point
		dFloat pos[3], norm[3];
		NewtonMaterialGetContactPositionAndNormal(material, pos, norm);
		collData->game->getBlobb(team)->setPosition(Vector(pos[0], 0, pos[2]));

		if (v[0]*teamModifier < 0) { // ball moves toward the net -> use normal collision with random elasticity
			NewtonMaterialSetContactElasticity(material, (0.9f+(rand()%9)/10.0f)*speedUp);
			collData->referee->ballOnBlobb(team);
			collData->game->playSoundTouch();
			return 1;
		}
		else {
			collData->referee->ballOnBlobb(team);
			collData->game->playSoundTouch();
			random = rand() % 10;
			if (random<5 || (collData->referee->getCurrentContacts(team)>1 && v[0]*teamModifier > 0)) { // reverse balls direction
				dFloat fRand = (rand()%10) / 10.0f;
				v[0] = - v[0]*(0.8f+fRand)*speedUp;
				v[1] = - v[1]*(0.8f+fRand)*speedUp;
				v[2] = - v[2]*(0.8f+fRand)*speedUp;
				NewtonBodySetVelocity(body, v);
				return 0;
			}
			else if (random <10) { // normal collision with random elasticity
				random = rand()%9;
				NewtonMaterialSetContactElasticity(material, (0.9f+random/10.0f)*speedUp);
				return 1;
			}
		}
	}
	else {
		NewtonMaterialSetDefaultCollidable(collData->world, collData->arena->getBallMaterialID(), collData->arena->getAiTriggerID(), 0);
		return 0;
	}
	return 1;
}