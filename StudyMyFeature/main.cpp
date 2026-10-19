#include "Character.h"
#include "noCharacter.h"
#include "Player.h"
#include "noPlayer.h"
#include "MyVector.h"
#include <iostream>
#include <string>

using namespace std;

int main()
{
	Character* c = new Player();
	noCharacter* nc = new noPlayer();

	c->Attack(); // Player Attack
	nc->Attack(); // noCharacter Attack


	int cbit = sizeof(Character);
	int ncbit = sizeof(noCharacter);


	MyVector<int> a;
	a.push_back(1);
	
	MyVector<int> d;   // 복사 생성자: b가 지금 막 태어남 (빈 상태에서 시작)
	d.push_back(9);
	d = a;            // 복사 대입: b는 이미 살아 있고, 자기 판도 가지고 있음

	MyVector<string> ca;
	ca.push_back("lemon");
	MyVector<string> cb;
	cb = ca;
	cout << cb[0] << endl;
	cout << d[0] << ca[0] << endl;

	delete c;
	delete nc;


	return 0;
}