#include <string>
#include <iostream>
#include <memory>
#include <array>
#include <vector>

static void AutoAndLiterals(void)
{
    // auto places more emphasis on literal specifiers

    // this is a const char*
    auto charPointer = "I am not a string";

    // nope: not a string
    //charPointer.size()


    // this is a string
    std::string str = "I am a string";

    using namespace std::literals;
    auto alsoStr = "I too am also a string"s;

    // will be interpreted as 8.0
    float someFloat = 8;

    // will be interpreted as an integer, 8
    auto notAFloat = 8;

    // will be interpreted as a double 8.0
    auto alsoNotAFloat = 8.0;

    // will be interpreted as a float 8.0f
    auto nowIsAFloat = 8.0f;

}

static auto ReturnsNothing_AutoFunction(void)
{

}

static auto ReturnsSometimes_AutoFunction(int value)
{
    if(value < 10)
    {
        return 8;
    }

    // ....

    // whoops! I forgot to return something in the other condition
    // warning: control reaches end of non-void function
    //  Undefined behavior as usual. But you might not notice this isn't a void function.

    // funny enough, if you put "return" here, it would have errored out and caught the mistake
    // error: inconsistent deduction for auto return type: ‘int’ and then ‘void
    //return;
}   

static auto ReturnsDifferentThings(int value)
{
    if(value < 10)
    {
        return 8.7;
    }
    else
    {
        // first deduced type wins, this returns a double, and int is not coerced into it.
        // error: deduced return type "int" conflicts with previously deduced type "double"
        //return 8;
        // error: deduced return type "float" conflicts with previously deduced type "double"
        //return 8.0f;
        return 8.3;
    }
}

int SomeGlobal = 23;
static auto WillNotReturnRef()
{
    // no matter what gymnastics you do, this function will never return an int&
    auto& globalRef = SomeGlobal;

    // returns int!! A copy of SomeGlobal
    return globalRef;
}

static auto& ReturnRef()
{
    // will always return a ref
    return SomeGlobal;
}

static void References()
{

    int someInteger = 85;

    // Explicit type references
    int& someIntegerRef = someInteger;
    const int& someIntegerConstRef = someInteger;

    // is a copy
    auto someIntegerCopy = someInteger;

    // Is a ref
    auto& autoSomeIntegerRef = someInteger;

    // Not a ref. This makes a copy of the data at someIntegerRef
    auto alsoSomeIntegerCopy = someIntegerRef;

    // Not a "ref to a ref". This is the same as someIntegerRef
    auto& alsoAutoSomeIntegerRef = someIntegerRef;

    // this is a const int even though we didn't specify const
    auto integerConstCopy = someIntegerConstRef;

    // This is a const int& even though we didn't specify const. It does not implicitly cast away const.
    // You might not know this is a const ref though if you didn't check.
    auto& integerConstRef = someIntegerConstRef;

    // you can also specify you want a const ref
    const auto& alsoConstRef = someIntegerConstRef;

    // and it will make it now const even if it isn't
    const auto& yetAnotherConstRef = someIntegerRef;

    someInteger = 95;

    std::cout << "SomeGlobal before modifications is " << SomeGlobal << std::endl;

    // is an int, ReturnRef is declared as auto& but if you try and store it in an auto, it will resolve it to int
    auto notARef = ReturnRef();
    // SomeGlobal is *not* modified
    notARef = 33;

    std::cout << "SomeGlobal after notARef is " << SomeGlobal << std::endl;

    auto& isARef = ReturnRef();
    // SomeGlobal *is* modified
    isARef = 34;

    std::cout << "SomeGlobal after isARef is " << SomeGlobal << std::endl;


}

static int AllocateCount = 0;
struct ArrayDeallocateDetector
{
    const int MyNumber;
    ArrayDeallocateDetector(void) : MyNumber(AllocateCount)
    {
        std::cout << "Array Detector " << MyNumber << " got allocated!" << std::endl;
        AllocateCount++;
    }
    ~ArrayDeallocateDetector()
    {
        std::cout << "Array Detector " << MyNumber << " got deallocated!" << std::endl;
    }
};

static auto AllocateStuff()
{
    // returns an ArrayDeallocateDetector* to memory the client must free with delete[]
    return new ArrayDeallocateDetector[5];
}

// this isn't really an "auto" problem per se, it's more of a shared_ptr mistake.
static auto Bad_WrapAllocation()
{
    // I'm trying to modernize my code and I don't want to deal with int*s, let's wrap it in a smart pointer
    // I'm pretty sure AllocateStuff returns a pointer right?

    auto allocated = AllocateStuff();
    
    // neato, it compiles!
    return std::shared_ptr<ArrayDeallocateDetector>{allocated};

    // but... it doesn't. It's UB because shared_ptr can't deallocate int[], only int
    // also you're now implying the type of what's returned from AllocateStuff
    //
    // This segfaults for me in GCC

}

static auto Dereference(int* somePtr)
{
    // don't write code like this, I just used this function as an example of 
    // how you need to keep track of when you are dealing with an instance vs a pointer
    return *(somePtr);
}

static void Pointers()
{
    int someInteger = 33;

    int* someIntegerPtr = &someInteger;
    int* const someIntegerConstPtr = &someInteger;
    const int* someConstIntegerPtr = &someInteger;
    const int* const someConstIntegerConstPointer = &someInteger;

    // you can do the same thing with auto. It just allows you to vary the pointed to type...
    auto* p0 = &someInteger;
    auto* const p1 = &someInteger;
    const auto* p2 = &someInteger;
    const auto* const p3 = &someInteger;

    auto* const alsoSomeIntegerConstPtr = someIntegerConstPtr;

    // this is a int* const *
    // (pointer to const pointer to int)
    auto* ptrToSomeIntegerConstPtr = &someIntegerConstPtr;

    // so is this
    auto alsoPtrToSomeIntegerConstPtr = &someIntegerConstPtr;

    // I think there's a clearer way to express stuff like this. Not recommended.
    // C pointer declarations are hard enough to read already, let's not add auto into it.
    auto** test = &someIntegerPtr;

    // error: unable to deduce ‘auto**’ from ‘someIntegerPtr’
    //auto** test2 = someIntegerPtr;
    int** test3 = &someIntegerPtr;

    //auto* p0 = someIntegerConstPtr;
    // error: conflicting declaration ‘auto* p0’

    // you don't actually have to specify the *, but I recommend that you do
    auto alsoIntPtr = someIntegerPtr;

    // this is an integer, be careful with dereferencing, especially APIs that imply a dereference
    // would you have known what this was doing if my variable names were worse?
    // I don't think I would.
    auto someIntegerCopy = Dereference(alsoIntPtr);


    // this is int* const, not const int* const
    const auto alsoConstIntegerPointer = someIntegerPtr;

    // is not actually const int* const
    //const auto const yetAnotherConstIntegerPointer = someIntegerPtr;
    // autotest.cc:135:16: error: duplicate ‘const’
    // https://stackoverflow.com/q/47291244
    // if you want const * const you have to put an * in.


    // how do I know to deallocate this? How do I know I have ownership?
    auto allocated = AllocateStuff();

    // delete allocated; // allowed, would be UB!
    // free(allocated) // allowed, would be UB!
    delete[] allocated; // if I didn't call this, it would be a memory leak

}


struct DeallocateDetector
{
    const int MyNumber;
    DeallocateDetector(const int& number) : MyNumber(number)
    {
        std::cout << "Detector " << MyNumber << " got allocated!" << std::endl;
    }
    ~DeallocateDetector()
    {
        std::cout << "Detector " << MyNumber << " got deallocated!" << std::endl;
    }
};

// returns smart pointers, but you won't know that by looking at the declaration or the code
static auto CreateDetector(int value)
{
    return std::shared_ptr<DeallocateDetector> {new DeallocateDetector(value)};
}

// returns *dead on arrival* raw pointers!!
static auto AllocateAndReturnRaw_Bad(int value)
{
    auto detectorSmartPointer = CreateDetector(value);

    auto rawPointer = detectorSmartPointer.get();
    return rawPointer;

    // detectorSmartPointer gets deallocated after the function ends!
}

static void SmartPointers()
{
    std::cout << "Start of SmartPointers()" << std::endl;

    std::shared_ptr<DeallocateDetector> d0{new DeallocateDetector(0)};

    // would you know that d1 was a smart pointer just by looking at this line?
    auto d1 = CreateDetector(1);
    // error: cannot deduce auto type
    //auto* d2 = CreateDetector(2);

    // this pointer is dead on arrival every time!
    std::cout << "Before allocating and returning raw" << std::endl;
    auto d2_deadOnArrival = AllocateAndReturnRaw_Bad(2);
    std::cout << "After allocating and returning raw" << std::endl;

    std::cout << "End of SmartPointers()" << std::endl;
}

constexpr auto ReturnsInt()
{
    return 8;
}

constexpr auto ReturnsDouble()
{
    return 1E99;
}


static void StillPayAttentionToTypes(void)
{
    // I found a bug like this once.

    // so the whole point of auto is you don't have to care as much about the underlying type
    // ...but you still have to be consistent...

    auto someInt = ReturnsInt();
    auto someDouble = ReturnsDouble();

    // Undefined behavior! Signed overflow.
    // g++ doesn't even give you a warning!
    // someInt gets some nonsense value like -2147483648
    someInt = someDouble;

    int breakpoint = 0;
}

// let's say that at one point this function returned a double, but we changed it later to return an int.
// that would be very bad for WatchImpliedTypeRequirements()
// you can't ignore the return type of a function, it just changes how you check for it.
auto ReturnsSomething()
{
    return ReturnsInt();
}


static void WatchImpliedTypeRequirements(void)
{
    // auto allegedly gives flexibility but you can very easily assume the returned type
    auto ub_Int = ReturnsSomething();

    auto someCalculation = 1E99;

    // With this line we just bound ourselves to assuming ReturnsSomething returns a double.
    // We will have UB if it returns an int.
    // only works if someValue is double, don't use auto if you need a double
    // has a value like -2147483648
    ub_Int += someCalculation;

    // we could have avoided UB if we did
    double notUB = ReturnsSomething();

    // though this is still not super great because ReturnsSomething is returning an int that we are converting to double
    // If ReturnsSomething were defined as double instead of auto, we'd have a firm guarantee about what it returned

    notUB += someCalculation;

    int breakpoint = 0;
}

// requires -fconcepts
// note that this will return either an int or a double depending on what's passed in.
static auto FullAuto(auto value)
{
    return value + 2;
}


constexpr static auto GetSize(auto value)
{
    return value + 2;
}

static int UseStaticInt(int value)
{
    return value * 2;
}

static double UseStaticDouble(double value)
{
    return value * 2;
}


static auto ImpliedDouble(auto value)
{
    auto result = value;
    // do some calculations....

    //...

    // surely this is a double right?
    result += 1E99;
    return result;
}


static void WithConcepts(void)
{
    // returns 10
    auto intResult = FullAuto(8);

    // returns 10.4
    auto doubleResult = FullAuto(8.4);

    auto bigDoubleResult = FullAuto(1E99);

    // compile error, it won't try and convert, which is good.
    // std::array<int, ReturnsDouble()> someArray;
    std::array<int, ReturnsInt()> array0;
    std::array<int, GetSize(ReturnsInt())> array1;
    // also does not compile
    // std::array<int, GetSize(ReturnsDouble())> array2;

    // Be careful with functions that take in / return a static value, these all run without any errors or warnings
    int intTimesTwo = UseStaticInt(intResult);
    double intConvertedToDouble = UseStaticDouble(intResult);
    int undefinedBehavior1 = UseStaticInt(bigDoubleResult); // int overflow: don't do this
    double doubleTimesTwo = UseStaticDouble(bigDoubleResult);

    // auto functions can accidentally only work with one type
    auto doublePlus1E99 = ImpliedDouble(1E99);
    auto undefinedBehavior2 = ImpliedDouble(8); // int overflow: don't do this
    auto undefinedBehavior3 = ImpliedDouble(2.0f); // float overflow: don't do this

    int breakpoint = 0;
}

static void ReverseForLoop(void)
{
    // reverse for loop
    std::vector<int> vect = {10,11,12,13};

    // ok as long as the array size isn't too big to fit in an int
    for(int index = (vect.size() - 1); index >= 0; index--)
    {
        std::cout << vect[index] << ' ';
    } 

    std::cout << std::endl;

    // ok
    for(auto it = vect.crbegin(); it != vect.crend(); it++)
    {
        std::cout << *(it) << ' ';
    }

    std::cout << std::endl;

    return;

    // infinite loop/segfault/UB! index is an unsigned integer because size() is unsigned. index >= 0 is always true
    for(auto index = (vect.size() - 1); index >= 0; index--)
    {
        std::cout << vect[index] << ' ';
    } 
}

int main()
{
    AutoAndLiterals();
    Pointers();
    References();
    SmartPointers();
    StillPayAttentionToTypes();
    WatchImpliedTypeRequirements();
    WithConcepts();
    // error: a value of type "void" cannot be used to initialize an entity of type "int"
    //int x = ReturnsNothing_AutoFunction();
    // error: cannot deduce auto type
    //auto x = ReturnsNothing_AutoFunction();
    //void x = ReturnsNothing_AutoFunction();
    // I don't know why you would do this.
    // error: incomplete type is not allowed.

    auto sometimes = ReturnsSometimes_AutoFunction(20);
    ReverseForLoop();





    return 0;
}