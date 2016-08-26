### iter::FileKeeper ###

```FileKeeper``` is using ```iter::DoubleBuffer``` for hot loading.

During initialization, ```FileKeeper``` will register to an instance of  ```iter::FileMonitor```. 

And when file modification events occured, ```FileKeeper``` will reload immediatelly.

#### Definition ####
```cpp
template <class LoadFunc, class Buffer =
    typename std::remove_pointer <typename LoadFunc::second_argument_type>::type>
class FileKeeper;
```
The second template argument ```Buffer``` is your target type to store your data. 

For example, if you want to load some dictionary, ```Buffer``` might be ```std::map <std::string, int>```.

The first template argument ```LoadFunc``` is the type of your load-functor.

The ```LoadFunc``` must defined:
```cpp
bool operator () (const std::string& filename, Buffer* buffer);
```

When file modification events occured, ```FileKeeper``` will call this functor and update its buffer.

For example:
```cpp
class FileReader : public std::binary_function <const std::string&, std::string*, bool> {
public:
  bool operator (first_argument_type filename, second_argument_type text);
};
```
```cpp 
typedef std::function <bool(const std::string&, std::string*)> Loader;
```
Both ```FileReader``` and ```Loader``` are valid template argument.

#### Member functions ####
| Member function | Description |
| ------ | ------ |
| (constructor) | Construct function. |
| Get | Get the const shared pointer of buffer. |
| operator bool | Validation check. |

##### iter::FileKeeper::FileKeeper #####
```cpp
template <class LoadFunc>
FileKeeper(
    const std::string& filename,
    const LoadFunc& load_func = LoadFunc(),
    const std::shared_ptr <FileMonitor>& file_monitor_ptr = std::shared_ptr <FileMonitor> ());
```

For example:
```cpp
FileKeeper <FileReader> file_keeper("test.txt");
```
```cpp
FileKeeper <std::function <const std::string&, std::string*> file_keeper("test.txt", 
    [](const std::string& filename, std::string* text) { return FileReader()(filename, text); });
```

##### iter::FileKeeper::Get #####
```cpp
std::shared_ptr <typename std::add_const <Buffer>::type> Get();
```
By calling ```Get```, you can get the const shared pointer of your structure ```Buffer```.

NOTICE, it might cause ```FileKeeper``` reload failed when you hold the shared_ptr for a long time.

You can view the source code of ```iter::DoubleBuffer```(iter/util/double_buffer.hpp) for more details.

##### iter::FileKeeper::operator bool #####
```cpp
operator bool();
```
Check whether it is registered on ```FileMonitor``` successfully.

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
