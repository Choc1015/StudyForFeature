#include "Character.h"
#include "noCharacter.h"
#include "Player.h"
#include "noPlayer.h"


int main()
{
	Character* c = new Player();
	noCharacter* nc = new noPlayer();

	c->Attack(); // Player Attack
	nc->Attack(); // noCharacter Attack

	
	int cbit = sizeof(Character);
	int ncbit = sizeof(noCharacter);


	delete c;
	delete nc;


	return 0;
}