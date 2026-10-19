#pragma once
#include <cassert>

template <typename T>
class MyVector
{
private:
	T* data;      // Q1: 판 (메모리 위치)
	int sizeValue;      // Q2: 계란 수
	int capacityValue;  // Q3: 판의 칸 수

public:
	MyVector<T>()
	{
		data = nullptr;
		sizeValue = 0;
		capacityValue = 0;
	};       // 처음 판 만들기 (미리 몇 칸 잡기로 했죠?)
	~MyVector<T>()
	{
		delete[] data;
	};      // 판 버리기
	MyVector<T>(const MyVector& InVector)
	{
		if (InVector.capacityValue == 0)
		{
			data = nullptr;
			sizeValue = 0;
			capacityValue = 0;

			return;
		}


		data = new T[InVector.capacityValue];
		sizeValue = InVector.sizeValue;
		capacityValue = InVector.capacityValue;


		for (int i = 0; i < InVector.size(); i++)
		{
			data[i] = InVector.data[i];
		}

	};


	int size() const { return sizeValue; }
	int capacity() const { return capacityValue; }

	void push_back(T InValue)
	{

		if (capacityValue == 0)
		{
			capacityValue = 1;
			sizeValue = 1;
			data = new T[capacityValue];
			data[sizeValue - 1] = InValue;

			return;
		}

		if (sizeValue == capacityValue)
		{
			T* temp = data;

			capacityValue *= 2;
			data = new T[capacityValue];
			for (int i = 0; i < sizeValue; i++)
			{
				data[i] = temp[i];
			}

			delete[] temp;

		}

		++sizeValue;
		data[sizeValue - 1] = InValue;


	}

	T& operator[](int Invalue)
	{

		assert(0 <= Invalue && sizeValue > Invalue);
	
		return data[Invalue];

	}

	MyVector& operator=(const MyVector& InVector)
	{
		if (this == &InVector)
		{
			return *this;
		}
		delete[] data;
		if (InVector.capacityValue == 0)
		{
			data = nullptr;
			sizeValue = 0;
			capacityValue = 0;
		}
		else
		{
			data = new int[InVector.capacityValue];
			sizeValue = InVector.sizeValue;
			capacityValue = InVector.capacityValue;
		}

		for (int i = 0; i < InVector.size(); i++)
		{
			data[i] = InVector.data[i];
		}

		return *this;
	}
};