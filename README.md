# ITER #
An c++ library consist of some exquisite components to make things easy.
## Requirements ##
1. G++: --std=c++11 -pthread
2. Linux kernel: > 2.6

## Log setting ##
Some components might print logs, if you set:
```
ITER_LOG_INIT("./iter.log");
```

All of the logs will print to the file ```"./iter.log"```. If not, the logs will print to ```stderr```. 

## Components ##
* [FileKeeper](https://github.com/qianyl/iter#iterfilekeeper)
* [Link](https://github.com/qianyl/iter#iterlink)

### iter::FileKeeper ###
---
```FileKeeper``` using double buffer for hot loading, and it will parse the file to your custom structures automatically.

  During construction, ```FileKeeper```will register to a singleton```FileLoaderManager```, and during deconstruction, it will deregister from ```FileLoaderManager```.

```FileLoaderManager``` will raise a watcher thread and using ```inotify``` to listen the file modification events.

When modification events occur, the registered ```FileKeeper``` will reload immediatelly.

#### Definition ####
```cpp
template <class LoadFunc, class Buffer> class FileKeeper;
```
The second template argument ```Buffer``` is your target type. 

For example, if you want to load some dictionary, ```Buffer``` might be ```std::map <std::string, int>``` or your own type.


The first template argument ```LoadFunc``` is the type of your load functor, which have defined:
```cpp
bool operator () (const std::string& filename, Buffer& buffer);
```
FileKeeper will call this functor to load the data from files and parsed to the type```Buffer```.

For example:
```cpp
class FileReader {
public:
  typedef const std::string& first_argument_type;
  typedef std::string& second_arguemt_type;
  bool operator (first_argument_type filename, second_argument_type text);
};
```
```cpp 
typedef std::function <bool(const std::string&, Buffer&)> SampleLoadFunc;
```
Both ```FileReader``` and ```SampleLoadFunc``` are acceptable.

#### Member functions ####
| Member function | Description |
| ------ | ------ |
| [(constructor)](https://github.com/qianyl/iter#iterfilekeeperfilekeeper) | Construct function. |
| [GetBuffer](https://github.com/qianyl/iter#iterfilekeepergetbuffer) | Get the shared pointer of buffer. |
| [Load](https://github.com/qianyl/iter#iterfilekeeperload) | Load data. |

##### iter::FileKeeper::FileKeeper #####
```cpp
FileKeeper(const std::string& filename);
```
```cpp
template <class LoadFuncInit, class ...Types>
FileKeeper(const std::string& filename,
    LoadFuncInit&& load_func_init, Types&& ...args);
```
If ```LoadFunc``` and ```Buffer``` have non-parameter constructor, your can use the first constructor.

The second constructor use the following operation to construct ```LoadFunc``` and ```Buffer```:
```cpp
new LoadFunc(load_func_init)
new Buffer(args...)
```
For example:
```cpp
FileKeeper <FileReader, std::string> file_keeper("test.txt");
```
```cpp
bool read_file(const std::string&, std::string&);
FileKeeper <SampleLoadFunc, std::string> file_keeper("test.txt", read_file, "");
```

In addition, if ```LoadFunc``` defined ```second_argument_type```, the following operation is equivalent:
```cpp
FileKeeper <FileReader> file_keeper("test.txt");
```
```cpp
FileKeeper <SampleLoadFunc> file_keeper("test.txt", read_file, "");
```

##### iter::FileKeeper::GetBuffer #####
```cpp
bool GetBuffer(std::shared_ptr <Buffer>* ptr);
```
By calling ```GetBuffer```, you can get the shared pointer of your structure ```Buffer```.

```FileKeeper``` using double buffer for hot loading, and ```std::shared_ptr``` have its own reference counter.

So, by calling ```std::shared_ptr::unique()```, we can know whether the buffer is released by all users. 

If the buffer is not the latest and released by all users, ```FileKeeper``` will release this buffer for the coming new data.

##### iter::FileKeeper::Load #####
```cpp
virtual bool Load();
```
```Load()``` will be called by ```FileLoaderManager``` for auto loading.

#### Example ####
```cpp
#include <iter/file_io.hpp>
#include <iter/file_keeper.hpp>
#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>

int main() {
    std::string filename = "file_keeper.test";
    std::string text = "File keeper test.";
    bool write_ret = iter::FileWriter()(text, filename);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    iter::FileKeeper <iter::FileReader> file_keeper(filename);
    std::shared_ptr <std::string> ptr;
    bool get_ret = file_keeper.GetBuffer(&ptr);
    std::cout << "Get result = " << *ptr << std::endl;
    ptr.reset();

    std::string new_text = "File keeper modified.";
    bool new_write_ret = iter::FileWriter()(new_text, filename);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    bool new_get_ret = file_keeper.GetBuffer(&ptr);
    std::cout << "Get result = " << *ptr << std::endl;

    return 0;
}

```
stdout:
```
Get result = File keeper test.
Get result = File keeper modified.
```

stderr:
```
[INFO][2016-06-21T10:22:46.495+0800][140406254540672][../iter/iter/datamanager/detail/file_loader_manager_impl.hpp:23][FileLoaderManager] msg=Thread start.||inotify_fd=3
[INFO][2016-06-21T10:22:46.495+0800][140406254540672][../iter/iter/datamanager/detail/file_loader_manager_impl.hpp:64][InsertFileLoader] msg=Initial load success.||filename=file_keeper.test
[INFO][2016-06-21T10:22:46.495+0800][140406254540672][../iter/iter/datamanager/detail/file_loader_manager_impl.hpp:79][InsertFileLoader] msg=Insert success.||watcher_fd=1||filename=file_keeper.test
[INFO][2016-06-21T10:22:46.496+0800][140406237718272][../iter/iter/datamanager/detail/file_loader_manager_impl.hpp:156][Callback] msg=Auto load success.||filename=file_keeper.test||event=2
[INFO][2016-06-21T10:22:46.595+0800][140406254540672][../iter/iter/datamanager/detail/file_loader_manager_impl.hpp:101][DeleteFileLoader] msg=Delete success.||watcher_fd=1||filename=file_keeper.test
[INFO][2016-06-21T10:22:46.596+0800][140406254540672][../iter/iter/datamanager/detail/file_loader_manager_impl.hpp:31][~FileLoaderManager] msg=Thread stop.||inotify_fd=3
```

### iter::Link ###
---
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
| [(constructor)](https://github.com/qianyl/iter#iterlinklink) | Construct function. |
| [operator ()](https://github.com/qianyl/iter#iterlinkoperator-) | Call target. |
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




