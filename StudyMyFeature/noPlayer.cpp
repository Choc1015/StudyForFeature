#include "noPlayer.h"
#include <iostream>


noPlayer::noPlayer()
{
}

noPlayer::~noPlayer()
{
	std::cout << "noPlayer" << std::endl;
}

void noPlayer::Attack()
{
	std::cout << "noPlayer Attack" << std::endl;
}
