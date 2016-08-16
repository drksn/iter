### iter::FileKeeper ###

```FileKeeper``` using double buffer for hot loading, and it will parse the file to your custom structures automatically.

During initialization, ```FileKeeper```will register to a singleton```FileLoaderManager```, and during deconstruction, it will deregister from ```FileLoaderManager```.

```FileLoaderManager``` will raise a watcher thread and using ```inotify``` to listen the file modification events.

When modification events occur, the registered ```FileKeeper``` will reload immediatelly.

#### Definition ####
```cpp
template <class LoadFunc, class Buffer> class FileKeeper;
```
The second template argument ```Buffer``` is your target type to store your data. 

For example, if you want to load some dictionary, ```Buffer``` might be ```std::map <std::string, int>``` or your own type.


The first template argument ```LoadFunc``` is the type of your load-functor, indicate the way to load data into your ```Buffer```.

The ```LoadFunc``` must defined:
```cpp
bool operator () (const std::string& filename, Buffer& buffer);
```
FileKeeper will call this functor to load the data from files and parsed to the type```Buffer```.

For example:
```cpp
class FileReader {
public:
  typedef const std::string& first_argument_type;
  typedef std::string& second_argument_type;
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
| (constructor) | Construct function. |
| Get | Get the const shared pointer of buffer. |

##### iter::FileKeeper::FileKeeper #####
```cpp
FileKeeper(
    const std::string& filename,
    LoadFunc load_func = LoadFunc(),
    std::shared_ptr <FileMonitor> file_monitor_ptr = std::make_shared <FileMonitor> ());
```

For example:
```cpp
FileKeeper <FileReader, std::string> file_keeper("test.txt");
```
```cpp
bool read_file(const std::string&, std::string&);
FileKeeper <SampleLoadFunc, std::string> file_keeper("test.txt", read_file);
```

In addition, if ```LoadFunc``` defined ```second_argument_type```, the following operation is equivalent:
```cpp
FileKeeper <FileReader> file_keeper("test.txt");
```
```cpp
FileKeeper <SampleLoadFunc> file_keeper("test.txt", read_file, "");
```

##### iter::FileKeeper::Get #####
```cpp
std::shared_ptr <typename std::add_const <Buffer>::type> Get();
```
By calling ```Get```, you can get the const shared pointer of your structure ```Buffer```.

```FileKeeper``` using double buffer for hot loading, and ```std::shared_ptr``` have its own reference counter.

So, by calling ```std::shared_ptr::unique()```, we can know whether the buffer is released by all users.

If the buffer is not the latest and released by all users, ```FileKeeper``` will release this buffer for the coming new data.

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
    std::shared_ptr <const std::string> ptr;
    ptr = file_keeper.Get();
    if (ptr)
        std::cout << "Get result = " << *ptr << std::endl;
    else
        std::cout << "Buffer is empty." << std::endl;
    ptr.reset();

    std::string new_text = "File keeper modified.";
    bool new_write_ret = iter::FileWriter()(new_text, filename);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ptr = file_keeper.Get();
    if (ptr)
        std::cout << "Get result = " << *ptr << std::endl;
    else
        std::cout << "Buffer is empty." << std::endl;

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
[INFO][2016-07-03T20:25:22.887+0800] msg=Auto load success.||filename=file_keeper.test
[INFO][2016-07-03T20:25:22.888+0800] msg=Auto load success.||filename=file_keeper.test
```
