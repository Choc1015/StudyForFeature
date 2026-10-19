#include "MyVector.h"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

using namespace std;

int main()
{
	MyVector<int> m2;
	vector<int> m3;

	auto start = std::chrono::steady_clock::now();
	
	for (int i = 0; i < 1000000; i++)
	{
		m2.push_back(i);
	}

	auto end = std::chrono::steady_clock::now();

	
	auto start1 = std::chrono::steady_clock::now();

	for (int i = 0; i < 1000000; i++)
	{
		m3.push_back(i);
	}

	auto end1 = std::chrono::steady_clock::now();

	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	auto ms1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
	cout << ms << " ms vs " << ms1 << " ms1" << endl;



	return 0;
}