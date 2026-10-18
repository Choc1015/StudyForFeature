#include "Character.h"
#include "noCharacter.h"
#include "Player.h"
#include "noPlayer.h"
#include "MyVector.h"
#include <iostream>

using namespace std;

int main()
{
	Character* c = new Player();
	noCharacter* nc = new noPlayer();

	c->Attack(); // Player Attack
	nc->Attack(); // noCharacter Attack


	int cbit = sizeof(Character);
	int ncbit = sizeof(noCharacter);

	MyVector v;

	cout << v.size() << ", " << v.capacity() << endl;

	v.push_back(1);
	v.push_back(2);
	v.push_back(4);
	v.push_back(4);

	v[2] = 3;

	cout << v[5] << ", " << v[3] << endl;

	delete c;
	delete nc;


	return 0;
}