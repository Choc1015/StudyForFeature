#include "MyVector.h"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

using namespace std;

void TestMoveAssign()
{
    MyVector<int> a;
    a.push_back(1); a.push_back(2); a.push_back(3);
    MyVector<int> b;
    b = std::move(a);

    assert(b.size() == 3);
    assert(b[0] == 1);
    assert(b[1] == 2);
    assert(b[2] == 3);
    assert(a.size() == 0);

    return;
}

int main()
{

    TestMoveAssign();



	return 0;
}

