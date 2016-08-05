### iter::Link ###

If we have some functors F1, F2, ..., Fn:
```cpp
F1: bool operator () (A0, A1&);
F2: bool operator () (A1, A2&);
...
Fn(An-1, An&): bool operator () (An-1, An);
```
We can use ```Link``` to bind these functors into one functor:
```cpp
Link <F1, F2, ..., Fn> link;
```
Call the following function:
```cpp
bool do(A0 a0, An an) {
  A1 a1; A2 a2; ...; An-1 an-1;
  if (F1()(a0, a1))
    if (F2()(a1, a2))
      ...
        if (Fn()(an-1, an))
          return true;
  return false;
}
```
Is equal to:
```cpp
link(a0, an); // <=> do(a0, an)
```
#### Definition ####
```cpp
template <class First, class ...Functor> class Link;
```
#### Member types ####
| member type | definition |
| ------ | ------ |
| result_type | bool |
| first_argument_type | First::first_argument_type |
| second_argument_type | Link <...Functors>::second_argument_type |

#### Member functions ####
| Member function | Description |
| ------ | ------ |
| (constructor) | Construct function. |
| operator () | Call target. |
##### iter::Link::Link #####
```cpp
template <class FirstInit, class ...Types>
Link(FirstInit&& first_init, Types&& ...args);
```
Possible behavior:
```cpp
new First(first_init);
new Link <...Functor> (args...);
```
##### iter::Link::operator () #####
```cpp
template <class Source, class Target>
bool operator () (Source&& source, Target&& target);
```

#### Example ####
```cpp
#include <iter/link.hpp>
#include <iostream>
#include <string>
#include <functional>

class AddStar {
public:
    typedef const std::string& first_argument_type;
    typedef std::string& second_argument_type;

    AddStar(const std::string& star = "*"):star_(star){}

    bool operator () (const std::string& a, std::string& b) {
        b = a + star_;
        return true;
    }

private:
    std::string star_;
};

bool add_one(std::string& a, std::string& b) {
    b = a + "1";
    return true;
}

typedef std::function <bool(std::string&, std::string&)> FuncBSS;

int main() {
    iter::Link <AddStar> l1("@");
    iter::Link <AddStar, AddStar> l2;
    iter::Link <FuncBSS, FuncBSS> l3(add_one, add_one);
    iter::Link <FuncBSS, FuncBSS, AddStar> l4(add_one, add_one, "@");

    std::string star = "CountingStar:", result;
    bool ret1 = l1(star, result);
    std::cout << "Result = " << result << std::endl;
    bool ret2 = l2(star, result);
    std::cout << "Result = " << result << std::endl;
    bool ret3 = l3(star, result);
    std::cout << "Result = " << result << std::endl;
    bool ret4 = l4(star, result);
    std::cout << "Result = " << result << std::endl;
    return 0;
}
```
stdout:
```
Result = CountingStar:@
Result = CountingStar:**
Result = CountingStar:11
Result = CountingStar:11@
```
