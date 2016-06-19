# ITER #
An c++ library consist of some exquisite components to make things easy.
## Requirements ##
1. G++: --std=c++11
2. Linux kernel: > 2.6

## Log setting ##
Some components might print logs, if you set:
```
ITER_LOG_INIT("./iter.log");
```

All of the logs will print to the file ```"./iter.log"```. If not, the logs will print to ```stderr```. 

## Components ##
* [FileKeeper](https://github.com/qianyl/iter#filekeeper)
* [Link](https://github.com/qianyl/iter#link)

### FileKeeper ###
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
| [(constructor)](https://github.com/qianyl/iter#constructor) | Construct function. |
| [GetBuffer](https://github.com/qianyl/iter#getbuffer) | Get the shared pointer of buffer. |
| [Load](https://github.com/qianyl/iter#load) | Load data. |

##### (Constructor) #####
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

##### GetBuffer #####
```cpp
bool GetBuffer(std::shared_ptr <Buffer>* ptr);
```
By calling ```GetBuffer```, you can get the shared pointer of your structure ```Buffer```.

```FileKeeper``` using double buffer for hot loading, and ```std::shared_ptr``` have its own reference counter.

So, by calling ```std::shared_ptr::unique()```, we can know whether the buffer is released by all users. 

If the buffer is not the latest and released by all users, ```FileKeeper``` will release this buffer for the coming new data.

##### Load #####
```cpp
virtual bool Load();
```
```Load()``` will be called by ```FileLoaderManager``` for auto loading.

### Link ###
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
| [(constructor)](https://github.com/qianyl/iter#constructor-1) | Construct function. |
| [operator ()](https://github.com/qianyl/iter#operator-) | Call target. |
##### (Constructor) #####
```cpp
template <class FirstInit, class ...Types>
Link(FirstInit&& first_init, Types&& ...args);
```
Possible behavior:
```cpp
new First(first_init);
new Link <...Functor> (args...);
```
##### operator () #####
```cpp
template <class Source, class Target>
bool operator () (Source&& source, Target&& target);
```







