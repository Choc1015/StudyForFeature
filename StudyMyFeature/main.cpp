#include "Character.h"
#include "noCharacter.h"
#include "Player.h"
#include "noPlayer.h"
#include "MyVector.h"


int main()
{
	Character* c = new Player();
	noCharacter* nc = new noPlayer();

	c->Attack(); // Player Attack
	nc->Attack(); // noCharacter Attack

	
	int cbit = sizeof(Character);
	int ncbit = sizeof(noCharacter);

	MyVector v;



	delete c;
	delete nc;


	return 0;
}