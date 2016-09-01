### iter::FileKeeper ###

```FileKeeper``` is using ```iter::DoubleBuffer``` for hot loading.

During initialization, ```FileKeeper``` will register to an instance of  ```iter::FileMonitor```. 

And when file modification events occured, ```FileKeeper``` will reload immediatelly.

#### Definition ####
```cpp
template <class Buffer>
class FileKeeper;
```
The template argument ```Buffer``` is your target type to store your data. 

For example, if you want to load some dictionary, ```Buffer``` might be ```std::map <std::string, int>```.

#### Member functions ####
| Member function | Description |
| ------ | ------ |
| (constructor) | Construct function. |
| Get | Get the const shared pointer of buffer. |
| Load | Load immediately, without wait modification events occurred. |
| operator bool | Validation check. |

##### iter::FileKeeper::FileKeeper #####
```cpp
template <class Buffer>
FileKeeper(
    const std::function <const std::string&, Buffer*> loader,
    const std::string& filename,
    const std::shared_ptr <FileMonitor>& file_monitor_ptr = std::shared_ptr <FileMonitor> ());
```

For example:
```cpp
FileKeeper <std::string> file_keeper(FileRead, "test.txt");
```

##### iter::FileKeeper::Get #####
```cpp
template <class Buffer>
std::shared_ptr <typename std::add_const <Buffer>::type> Get();
```
By calling ```Get```, you can get the const shared pointer of your structure ```Buffer```.

NOTICE, it might cause ```FileKeeper``` reload failed when you hold the shared_ptr for a long time.

You can view the source code of ```iter::DoubleBuffer```(iter/util/double_buffer.hpp) for more details.

##### iter::FileKeeper::Load #####
```cpp
bool Load();
```
You can call this member function to makes your  ```FileKeeper``` instance load immediately.

##### iter::FileKeeper::operator bool #####
```cpp
operator bool();
```
Check whether it is registered on ```FileMonitor``` successfully.

#### Example ####
```cpp
#include "basicio/file_io.hpp"
#include <iter/file_keeper.hpp>
#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>

int main() {
    std::string filename = "file_keeper.test";
    std::string text = "File keeper test.";
    bool write_ret = iter::FileWrite(filename, text);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    iter::FileKeeper <std::string> file_keeper(FileRead, filename);
    std::shared_ptr <const std::string> ptr;
    ptr = file_keeper.Get();
    std::cout << "Get result = " << *ptr << std::endl;
    ptr.reset();

    std::string new_text = "File keeper modified.";
    bool new_write_ret = iter::FileWrite(filename, new_text);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ptr = file_keeper.Get();
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
[INFO][2016-07-03T20:25:22.887+0800][594] msg=Auto load success.||filename=file_keeper.test
[INFO][2016-07-03T20:25:22.888+0800][607] msg=Auto load success.||filename=file_keeper.test
```
