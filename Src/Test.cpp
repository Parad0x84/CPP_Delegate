
#include "Delegate.h"
#include "TestClass.h"




int main(int argc, char** argv)
{
    //MultiDelegate<void(int, int)> myDel;
    //myDel.AddLambda(nullptr, [] (const int asd, int dsa) { return asd; });
    //myDel.Broadcast(0, 5);

    TestClass* a = new TestClass("Test Class 0", 0.0f);
    TestClass* b = new TestClass("Test Class 1", 3.14f);
    TestClass* c = new TestClass("Test Class 2", 15.21f);

    OtherTestClass* o = new OtherTestClass;


    /////////////////////////////////////////////////////////////////////////////////////
    // std::cout << "\n----------------------\n\n";
    /////////////////////////////////////////////////////////////////////////////////////


    auto lambda = [] (const char* message) { std::cout << message << '\n'; };
    auto otherLambda = [] (const char* message) { std::cout << "Destroyed a TestClass object\n"; };



    a->DestructorDelegate.AddLambda(nullptr, lambda);
    b->DestructorDelegate.AddObject(a, &TestClass::Print);
    c->DestructorDelegate.AddLambda(nullptr, otherLambda);

    a->Add(3, 2);
    a->AddDelegate.BindObject(o, &OtherTestClass::PrintInt);
    a->Add(3, 2);


    /////////////////////////////////////////////////////////////////////////////////////
    std::cout << "\n----------------------\n\n";
    /////////////////////////////////////////////////////////////////////////////////////


    o->GetSomeNumbersDelegate.AddObject(a, &TestClass::ReturnMyNumber);
    o->GetSomeNumbersDelegate.AddLambda(nullptr, [] () { return 133.99f; });
    o->PrintSomeNumbers();

    std::cout << "\n----------------------\n\n";

    o->GetSomeNumbersDelegate.AddObject(c, &TestClass::ReturnMyNumber);
    o->GetSomeNumbersDelegate.AddObject(b, &TestClass::ReturnMyNumber);
    o->PrintSomeNumbers();

    std::cout << "\n----------------------\n\n";

    o->GetSomeNumbersDelegate.Remove(a);
    o->PrintSomeNumbers();


    /////////////////////////////////////////////////////////////////////////////////////
    std::cout << "\n----------------------\n\n";
    /////////////////////////////////////////////////////////////////////////////////////
    

    delete a;
    delete c;

    // This causes Undefined Behaviour, because you need to unbind stuff when destroying
    // o->PrintSomeNumbers();

    delete o;
    delete b;


    return 0;
}
