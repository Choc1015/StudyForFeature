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
			if (ControlNum->Weak == 0)
			{
				delete ControlNum;
			}
		}



	}

	MyShared(const MyShared& Other)
	{
		ControlNum = Other.ControlNum;
		Data = Other.Data;
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
			if (ControlNum->Weak == 0)
			{
				delete ControlNum;
			}
		}



		Data = Other.Data;
		ControlNum = Other.ControlNum;
		ControlNum->Strong++;



		return *this;
	}



private:


};

template <typename T>
class MyWeak
{
public:
	T* Data;
	ControlBlock* ControlNum;

	MyWeak(const MyShared<T>& Shared)
	{
		Data = Shared.Data;
		ControlNum = Shared.ControlNum;
		Shared.ControlNum->Weak++;
	}
	virtual ~MyWeak()
	{
		ControlNum->Weak--;

		if (ControlNum->Strong == 0 && ControlNum->Weak == 0)
		{
			delete ControlNum;
		}
	}

private:

};
