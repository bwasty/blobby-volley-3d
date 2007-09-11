#ifndef BV3D_AI
#define BV3D_AI

#include <vrs/sharedobj.h>
#include "Constants.h"

struct NewtonMaterial;
struct NewtonContact;
struct NewtonBody;

namespace BV3D
{
	class Arena;
	class Game;

	class AI : public VRS::SharedObj
	{
	public:
		AI(BV3D::Game* game);
		void enableAI(BV3D::TEAM team);
		bool isAiControlled(BV3D::TEAM team) {return mAiTriggerBody[team==BV3D::TEAM1 ? 0 : 1] == NULL ? false : true;}
		void disableAI(BV3D::TEAM team);
		void aiServe(BV3D::TEAM team);
		static int AiCallback(const NewtonMaterial* material, const NewtonContact* contact);
		//static int AICallback(const NewtonMaterial* material, const NewtonContact* contact);

		VRS::SO<BV3D::Game> getGame() {return mGame;}

	private:
		VRS::SO<BV3D::Game> mGame;
		NewtonBody*		mAiTriggerBody[2];

	};
}

#endif	// BV3D_AI