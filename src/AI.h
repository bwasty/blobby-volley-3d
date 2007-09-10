#ifndef BV3D_AI
#define BV3D_AI

#include <vrs/sharedobj.h>
#include "Constants.h"

struct NewtonMaterial;
struct NewtonContact;

namespace BV3D
{
	class Arena;
	class Game;

	class AI : public VRS::SharedObj
	{
	public:
		AI(BV3D::Game* game);
		void aiServe(BV3D::BV3D_TEAM team);
		static int AiCallback(const NewtonMaterial* material, const NewtonContact* contact);

		VRS::SO<BV3D::Game> getGame() {return mGame;}

	private:
		VRS::SO<BV3D::Game> mGame;

	};
}

#endif	// BV3D_AI