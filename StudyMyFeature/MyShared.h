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
		Other = nullptr;
		Num++;


	}
	virtual ~MyShared()
	{
		Num--;
		delete Data;

	}



private:


};