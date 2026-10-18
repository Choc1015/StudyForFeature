#include "noCharacter.h"
#include <iostream>

noCharacter::noCharacter()
{
}

noCharacter::~noCharacter()
{
	std::cout << "noCharacter" << std::endl;
}

void noCharacter::Attack()
{
	std::cout << "noCharacter Attack" << std::endl;
}