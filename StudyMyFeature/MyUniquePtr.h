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
	
	MyUniquePtr(const MyUniquePtr& Same) = delete;

	void operator=(const MyUniquePtr& Same) = delete;
	
	
	virtual ~MyUniquePtr()
	{
		delete SavePointer;
	}


private:

};

