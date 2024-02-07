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

    int Print2(const char* message, int number, const char* otherMessage)
    {
        std::cout << message << " - " << number << " - " << otherMessage << '\n';
        return number;
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
    Delegate<void(int)> AddDelegate;
    MultiDelegate<void(const char*)> DestructorDelegate;

    const char* MyName;
    float MyNumber;
};



class OtherTestClass
{
public:
    void PrintSomeNumbers()
    {
        std::vector<float> numbers = GetSomeNumbersDelegate.BroadcastRetVal();

        for(float number : numbers)
            std::cout << "Number is: " << number << '\n';
    }

    void PrintInt(int a)
    {
        std::cout << "PrintInt -> " << a << '\n';
    }


public:
    MultiDelegate<float()> GetSomeNumbersDelegate;
};
