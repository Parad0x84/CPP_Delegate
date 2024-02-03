#pragma once


#include "Delegate.h"

#include <iostream>




class TestClass
{
public:
    TestClass() = delete;
    explicit TestClass(const char* myName, float inNumber)
        : MyName(myName), MyNumber(inNumber)  { }

    ~TestClass()
    {
        DestructorDelegate.Broadcast(MyName);
    }



    void Print(const char* message)
    {
        std::cout << message << '\n';
    }

    void Add(int a, int b)
    {
        int result = a + b;
        AddDelegate.ExecuteIfBound(result);
    }

    float ReturnMyNumber() const
    {
        return MyNumber;
    }


public:
    Delegate<int> AddDelegate;
    MultiDelegate<const char*> DestructorDelegate;

    const char* MyName;
    float MyNumber;
};



class OtherTestClass
{
public:
    void PrintSomeNumbers()
    {
        const std::vector<float>& numbers = GetSomeNumbersDelegate.Broadcast();

        for(float number : numbers)
            std::cout << "Number is: " << number << '\n';
    }

    void PrintInt(int a)
    {
        std::cout << "PrintInt -> " << a << '\n';
    }


public:
    MultiDelegateRetVal<float> GetSomeNumbersDelegate;
};
