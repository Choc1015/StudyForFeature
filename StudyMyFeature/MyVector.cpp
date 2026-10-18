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
