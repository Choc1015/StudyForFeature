#include "MyVector.h"



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
