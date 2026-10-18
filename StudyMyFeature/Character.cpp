#include "Character.h"
#include <iostream>

Character::Character()
{
}

Character::~Character()
{
	std::cout << "Character" << std::endl;
}


void Character::Attack() const
{
	std::cout << "Character Attack" << std::endl;
}

