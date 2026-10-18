#include "MyVector.h"



MyVector::MyVector()
{
	data = nullptr;
	size = 0;
	capacity = 0;


}

MyVector::~MyVector()
{
	delete[] data;
}
