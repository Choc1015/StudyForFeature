#pragma once

template <typename T>
class MyUnique1
{
	public:
		T* SavePointer;

		MyUnique1(T* NewPointer)
		{
			SavePointer = NewPointer;
		}

		virtual ~MyUnique1()
		{
			delete SavePointer;
		}

		MyUnique1(const MyUnique1& Other) = delete;

		MyUnique1& operator=(const MyUnique1& Other) = delete;

		MyUnique1(MyUnique1&& Other)
		{
			SavePointer = Other.SavePointer;
			Other.SavePointer = nullptr;
		}
		MyUnique1& operator=(MyUnique1&& Other)
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




	private:


};