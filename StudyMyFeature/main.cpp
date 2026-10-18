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

	delete c;
	delete nc;


	return 0;
}