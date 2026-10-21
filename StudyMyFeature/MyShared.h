#pragma once

struct ControlBlock
{
public:

	int Strong = 0;
	int Weak = 0;

};


template <typename T>
class MyShared
{
public:

	T* Data;
	ControlBlock* ControlNum;


	MyShared(T* Other)
	{

		Data = Other;

		ControlNum = new ControlBlock;
		ControlNum->Strong++;


		Other = nullptr;

	}

	virtual ~MyShared()
	{
	

		ControlNum->Strong--;
		if (ControlNum->Strong == 0)
		{
			delete Data;
		}



	}

	MyShared(const MyShared& Other)
	{
		ControlNum = Other.ControlNum;
		Data = Other.Data;
		ControlNum->Strong = Other.ControlNum->Strong;
		ControlNum->Strong++;



	}

	MyShared& operator=(const MyShared& Other)
	{
		if (this == &Other)
		{
			return *this;
		}

		ControlNum->Strong--;
		if (ControlNum->Strong == 0)
		{
			delete Data;
		}


		
		Data = Other.Data;
		ControlNum->Strong = Other.ControlNum->Strong;
		ControlNum->Strong++;



		return *this;
	}



private:


};