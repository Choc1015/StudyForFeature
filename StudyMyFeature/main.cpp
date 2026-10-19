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

	MyVector<int> v;

	cout << v.size() << ", " << v.capacity() << endl;

	v.push_back(1);
	v.push_back(2);
	v.push_back(4);
	v.push_back(4);

	v[2] = 3;

	cout << v[3] << ", " << v[1] << endl;

	MyVector<int> a;
	a.push_back(1);
	
	MyVector<int> d;   // 복사 생성자: b가 지금 막 태어남 (빈 상태에서 시작)
	d.push_back(9);
	d = a;            // 복사 대입: b는 이미 살아 있고, 자기 판도 가지고 있음

	delete c;
	delete nc;


	return 0;
}