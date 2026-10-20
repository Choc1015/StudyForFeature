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

	MyUniquePtr& operator=(MyUniquePtr&& Other)
	{
		if (this == &Other)
		{
			return *this;
		}

		delete SavePointer;
		SavePointer = Other.SavePointer;
		Other.SavePointer = nullptr;

		return *this;
	}

	
	virtual ~MyUniquePtr()
	{
		delete SavePointer;
	}


private:

};

