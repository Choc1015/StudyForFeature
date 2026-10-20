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
	
	MyUniquePtr(const MyUniquePtr& Other) = delete;

	void operator=(const MyUniquePtr& Other) = delete;
	
	MyUniquePtr(MyUniquePtr&& Other)
	{
		SavePointer = Other.SavePointer;
		Other.SavePointer = nullptr;
	}


	
	virtual ~MyUniquePtr()
	{
		delete SavePointer;
	}


private:

};

