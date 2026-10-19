#pragma once

template <typename T>
class MyUniquePtr
{
public:
	T* SavePointer;

	MyUniquePtr(T* Tname)
	{
		SavePointer = Tname;
	}
	virtual ~MyUniquePtr()
	{
		delete[] SavePointer;
	}


private:

};

