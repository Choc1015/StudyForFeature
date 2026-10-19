#include "Character.h"
#include "noCharacter.h"
#include "Player.h"
#include "noPlayer.h"
#include "MyVector.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main()
{
	MyVector<string> m2;
	m2.push_back("melon1");
	m2.push_back("melon2");
	m2.push_back("melon3");
	MyVector<string> m3;
	m3.push_back("grape");
	m3 = std::move(m2);
	cout << m3[0] << ", " << m3[1] << ", " << m3[2] << ", " << m2.size() << endl;   // melon, 0 이 나와야 해요


	return 0;
}