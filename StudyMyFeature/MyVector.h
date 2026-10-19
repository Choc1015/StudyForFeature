#pragma once


template <typename T>
class MyVector
{
private:
    T* data;      // Q1: 판 (메모리 위치)
    T sizeValue;      // Q2: 계란 수
    T capacityValue;  // Q3: 판의 칸 수

public:
    MyVector();       // 처음 판 만들기 (미리 몇 칸 잡기로 했죠?)
    ~MyVector();      // 판 버리기
    MyVector(const MyVector& InVector);


    T size() const { return sizeValue; }
    T capacity() const { return capacityValue; }

    void push_back(T InValue);

    T &operator[](T Invalue);

    MyVector& operator=(const MyVector& InVector);
};