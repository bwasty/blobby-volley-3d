#ifndef BV3D_REFEREE
#define BV3D_REFEREE

#include <vrs/sharedobj.h>
//#include "Constants.h"
/*#include <vrs/so.h>
#include <vrs/id.h>
#include <cassert>
#include <vrs/io/document.h>
//class SharedObj;
*/
using namespace VRS;

namespace BV3D
{
	class Game;
	class HUD;

	class Referee
	{
	public:
		Referee(SO<Game> game);
		virtual ~Referee(void){};
		
		virtual void ballOnBlobb(BV3D_TEAM team) = 0;
		virtual void ballOnField(BV3D_TEAM team) = 0;	

		SO<Game> getGame();
		int getCurrentScore(BV3D_TEAM team);
		int getCurrentContacts(BV3D_TEAM team);
		int getWinningScore();
		int getMaximumContacts();
		int getMinimumDifference();
		BV3D_ERROR setWinningScore(int winningScore);
		BV3D_ERROR setMaximumContacts(int maxContacts);
		BV3D_ERROR setMinimumDifference(int minDifference);
		void startNewGame();
		BV3D_TEAM getServingTeam() {return m_ServingTeam;}
		void setActive(bool b) {m_Active = b;}
		void setHUD(VRS::SO<HUD> hud) {mHud = hud;}
		void gameOver(BV3D_TEAM winner);

	protected:
		int increaseScore(BV3D_TEAM team);
		int increaseContacts(BV3D_TEAM team);
		void resetContacts(BV3D_TEAM team);
		bool isGameOver();
		int getTeamIndex(BV3D_TEAM team);
		BV3D_TEAM getOpponent(BV3D_TEAM team);

	private:
		int m_score[2], m_contacts[2];
		int m_winningScore, m_maxContacts, m_minDifference;
		VRS::SO<HUD>	mHud;
		
	protected:
		SO<Game> m_game;
		BV3D_TEAM m_ServingTeam;
		bool m_Active;

	};
}

#endif // #ifndef BV3D_REFEREE
