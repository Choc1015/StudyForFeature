#pragma once

class MyVector
{
private:
    int* data;      // Q1: 판 (메모리 위치)
    int sizeValue;      // Q2: 계란 수
    int capacityValue;  // Q3: 판의 칸 수

public:
    MyVector();       // 처음 판 만들기 (미리 몇 칸 잡기로 했죠?)
    ~MyVector();      // 판 버리기
    MyVector(const MyVector& InVector);


    int size() const { return sizeValue; }
    int capacity() const { return capacityValue; }

    void push_back(int InValue);

    int &operator[](int Invalue);

    MyVector& operator=(const MyVector& InVector);
};