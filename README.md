# CPP_Delegate
 An implementation of UE like delegates for vanilla CPP


## Build
Test code uses premake. Just run GENERATE_VS_SOLUTION.bat for windows  
For other platforms you can check out premake  
Or you can just use something else...  


## Known Issues
- You can't Bind const functions/methods
- It's inconvenient to Unbind stuff
- It makes assumption of "Every object will only bind once (including nullptr for lambdas)"


## TODO:
- Add ability to pass in payload data when binding
- Maybe remove "virtual void* GetObjectPtr() = 0;" functions and do casting instead??
- Maybe rewrite some of MultiDelegate/MultiDelegateRetVal logic??
- Maybe get rid of std::function?? (I can't bind lambdas without it right now. Capturing stuff actually changes lambdas type and type mismatch happens)


## Test code output (if you don't wanna run it)
```
PrintInt -> 5

----------------------

Number is: 0
Number is: 133.99

----------------------

Number is: 0
Number is: 133.99
Number is: 15.21
Number is: 3.14

----------------------

Number is: 133.99
Number is: 15.21
Number is: 3.14

----------------------

Test Class 0
Destroyed a TestClass object
Test Class 1
```