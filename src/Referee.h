#ifndef BV3D_REFEREE
#define BV3D_REFEREE

#include <vrs/sharedobj.h>
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

	class Referee
	{
	public:
		Referee(SO<Game> game);
		virtual ~Referee(void) = 0;
		
		virtual void ballOnBlobb(bool isInLeftField) = 0;
		virtual void ballOnField(bool isInLeftField) = 0;	

		SO<Game> getGame();
		int getCurrentScore(bool ofLeftField);
		int getCurrentContacts(bool ofLeftField);
		int getWinningScore();
		int getMaximumContacts();
		int getMinimumDifference();
		int setWinningScore(int winningScore);
		int setMaximumContacts(int maxContacts);
		int setMinimumDifference(int minDifference);
		void startNewGame();

	private:
		int m_score[2], m_contacts[2];
		int m_winningScore, m_maxContacts, m_minDifference;
		SO<Game> m_game;

	protected:
		int increaseScore(bool ofLeftField);
		int increaseContacts(bool ofLeftField);
		int resetContacts(bool ofLeftField);
		bool isGameOver();
	};
}

#endif // #ifndef BV3D_REFEREE
