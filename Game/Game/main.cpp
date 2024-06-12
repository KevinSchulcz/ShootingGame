#include "game.h"

// TODO:
//	1. Text is not working
//	2. Randomness is not working
//
//	IMPORTANT: runtime error in spawnEnemy, could have modulo by 0 from config
// 	
//	IMPORTANT: capitalize class letters next time
//

int main()
{
	game g("config.txt");
	g.run();
}