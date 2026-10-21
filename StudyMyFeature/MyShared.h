#pragma once

template <typename T>
class MyShared
{
public:

	T* Data;
	int* NumPointer;


	MyShared(T* Other)
	{

		Data = Other;
		NumPointer = new int();
		*NumPointer += 1;

		Other = nullptr;

	}

	virtual ~MyShared()
	{
		*NumPointer -= 1;
		if(*NumPointer == 0)
		{
			delete NumPointer;
			delete Data;
		}

	}



private:


};