#include "Character.h"
#include "noCharacter.h"
#include "Player.h"
#include "noPlayer.h"


int main()
{
	Character* c = new Player();
	noCharacter* nc = new noPlayer();

	delete c;
	delete nc;


	return 0;
}