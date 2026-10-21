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

	MyShared(const MyShared& Other)
	{
		Data = Other.Data;
		NumPointer = Other.NumPointer;
		*NumPointer += 1;

	}

	MyShared& operator=(const MyShared& Other)
	{
		if (this == &Other)
		{
			return *this;
		}

		Data = nullptr;
		NumPointer = nullptr;
		
		Data = Other.Data;
		NumPointer = Other.NumPointer;
		*NumPointer += 1;



		return *this;
	}



private:


};