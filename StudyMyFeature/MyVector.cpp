#include "MyVector.h"
#include <cassert>


template <typename T>
MyVector<T>::MyVector()
{
	data = nullptr;
	sizeValue = 0;
	capacityValue = 0;


}

template <typename T>
MyVector<T>::~MyVector()
{
	delete[] data;
}

template <typename T>
MyVector<T>::MyVector(const MyVector& InVector)
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
}

template <typename T>
void MyVector<T>::push_back(T InValue)
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
template <typename T>
T& MyVector<T>::operator[](T Invalue)
{
	assert(Invalue < sizeValue && Invalue >= 0);


	return data[Invalue];
}

template <typename T>
MyVector<T>& MyVector<T>::operator=(const MyVector& InVector)
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
