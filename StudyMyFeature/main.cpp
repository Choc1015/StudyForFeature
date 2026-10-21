#include "MyVector.h"
#include "Character.h"
#include "Player.h"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include "MyShared.h"

using namespace std;

void TestCopyConstruct()
{
	MyVector<int> d;
	d.push_back(1); d.push_back(2); d.push_back(3);
	MyVector<int> c = d;

	// 복사 생성
	assert(c.size() == 3);
	assert(c[0] == 1);
	assert(c[1] == 2);
	assert(c[2] == 3);

	assert(d.size() == 3);
	assert(d[0] == 1);
	assert(d[1] == 2);
	assert(d[2] == 3);

	c[0] = 100;
	assert(d[0] == 1);
}
void TestCopyAssign()
{
	MyVector<int> d;
	d.push_back(1); d.push_back(2); d.push_back(3);
	MyVector<int> f;
	f = d;
	// 복사 대입
	assert(f.size() == 3);
	assert(f[0] == 1);
	assert(f[1] == 2);
	assert(f[2] == 3);
	f[0] = 100;

	assert(d.size() == 3);
	assert(d[0] == 1);
	assert(d[1] == 2);
	assert(d[2] == 3);

}
void TestMoveConstruct()
{
	MyVector<int> e;
	e.push_back(4); e.push_back(5); e.push_back(6);
	MyVector<int> k = std::move(e);

	// 이동 생성

	assert(k.size() == 3);
	assert(k[0] == 4);
	assert(k[1] == 5);
	assert(k[2] == 6);
	assert(e.size() == 0);

}
void TestMoveAssign()
{
	MyVector<int> a;
	a.push_back(1); a.push_back(2); a.push_back(3);
	MyVector<int> b;
	b = std::move(a);


	// 이동 대입
	assert(b.size() == 3);
	assert(b[0] == 1);
	assert(b[1] == 2);
	assert(b[2] == 3);
	assert(a.size() == 0);
}

//void Spawn(bool bCancel)
//{
//	MyUnique1<Character> c(new Player());
//	if (bCancel)                   // ②
//	{
//		return;                    // ③
//	}
//}


int main()
{
	MyWeak<Character>* w = nullptr;
	{
		MyShared<Character> s(new Player());
		w = new MyWeak<Character>(s);
		{
			MyShared<Character> borrowed = w->Lock();
			cout << "빌리는 중 strong: " << s.ControlNum->Strong << endl;   // 2
		}
		cout << "다 씀 strong: " << s.ControlNum->Strong << endl;          // 1
	}
	MyShared<Character> late = w->Lock();
	cout << "죽은 뒤 Lock: " << (late.ControlNum == nullptr) << endl;     // 1 (빈 것)
	delete w;
	cout << "끝" << endl;

	return 0;
}



