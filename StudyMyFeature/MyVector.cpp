#include "MyVector.h"



MyVector::MyVector()
{
	data = new int[0];
	size = 0;
	capacity = 0;


}

MyVector::~MyVector()
{
	delete data;
}
