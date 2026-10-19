#include "MyVector.h"
#include <cassert>


MyVector::MyVector()
{
	data = nullptr;
	sizeValue = 0;
	capacityValue = 0;


}

MyVector::~MyVector()
{
	delete[] data;
}

MyVector::MyVector(const MyVector& InVector)
{
	if (InVector.data == nullptr)
	{
		data = nullptr;
		sizeValue = 0;
		capacityValue = 0;
		return;
	}


	data = new int[InVector.capacityValue];
	sizeValue = InVector.sizeValue;
	capacityValue = InVector.capacityValue;


	for (int i = 0; i < InVector.size(); i++)
	{
		data[i] = InVector.data[i];
	}
}

void MyVector::push_back(int InValue)
{
	if (capacityValue == 0)
	{
		capacityValue = 1;
		sizeValue = 1;
		data = new int[capacityValue];
		data[sizeValue - 1] = InValue;

		return;
	}

	if (sizeValue == capacityValue)
	{
		int* temp = data;

		capacityValue *= 2;
		data = new int[capacityValue];
		for (int i = 0; i < sizeValue; i++)
		{
			data[i] = temp[i];
		}

		delete[] temp;

	}

	++sizeValue;
	data[sizeValue - 1] = InValue;


}

int& MyVector::operator[](int Invalue)
{
	assert(Invalue < sizeValue && Invalue >= 0);


	return data[Invalue];
}

MyVector& MyVector::operator=(const MyVector &InVector)
{
	if (this == &InVector)
	{
		return *this;
	}

	if (data != nullptr)
	{
		delete[] data;
	}

	data = new int[InVector.capacityValue];
	sizeValue = InVector.sizeValue;
	capacityValue = InVector.capacityValue;


	for (int i = 0; i < InVector.size(); i++)
	{
		data[i] = InVector.data[i];
	}

	return *this;
}
