#pragma once

class MyVector
{
private:
    int* data;      // Q1: 판 (메모리 위치)
    int size;      // Q2: 계란 수
    int capacity;  // Q3: 판의 칸 수

public:
    MyVector();       // 처음 판 만들기 (미리 몇 칸 잡기로 했죠?)
    ~MyVector();      // 판 버리기
};