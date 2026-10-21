#pragma once

template <typename T>
class MyShared
{
public:

	T* Data;
	int Num = 0;;


	MyShared(T* Other)
	{

		Data = Other;
		++Num;
		Other->count = Num;
		
		Other = nullptr;

	}

	virtual ~MyShared()
	{
		Num--;
		delete Data;

	}



private:


};