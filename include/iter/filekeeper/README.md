### iter::FileKeeper ###

```FileKeeper``` is using ```iter::DoubleBuffer``` for hot loading.

During initialization, ```FileKeeper``` will register to the given ```iter::FileMonitor``` instance.

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
| Load | Load immediately, without wait modification events occurred. |
| Get | Get the const shared pointer of buffer. |
| operator bool | Validation check. |

##### iter::FileKeeper::FileKeeper #####
```cpp
template <class Buffer>
FileKeeper(
    const std::string& filename,
    const std::function <const std::string&, Buffer*> loader,
    const std::shared_ptr <FileMonitor>& file_monitor_ptr);
```

For example:
```cpp
FileKeeper <std::string> file_keeper(FileRead, "test.txt", file_monitor_ptr);
```

##### iter::FileKeeper::Load #####
```cpp
bool Load();
```
You can call this member function to makes your  ```FileKeeper``` instance load immediately.

Usually for initial-loading.

##### iter::FileKeeper::Get #####
```cpp
template <class Buffer>
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
#include "basicio/file_io.hpp"
#include <iter/file_keeper.hpp>
#include <iter/file_monitor.hpp>
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

    auto file_monitor_ptr =
            std::make_shared <iter::FileMonitor> ();
    auto file_keeper_ptr =
            std::make_shared <iter::FileKeeper <std::string>> (filename, iter::FileRead, file_monitor_ptr);

    if (file_keeper_ptr->Load()) {
        std::cout << "Initial load success." << std::endl;
    }
    else {
        std::cout << "Initial load failed." << std::endl;
    }

    std::shared_ptr <const std::string> ptr;
    ptr = file_keeper_ptr->Get();
    std::cout << "Get result = " << *ptr << std::endl;
    ptr.reset();

    std::string new_text = "File keeper modified.";
    bool new_write_ret = iter::FileWrite(filename, new_text);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ptr = file_keeper_ptr->Get();
    std::cout << "Get result = " << *ptr << std::endl;

    return 0;
}
```

stdout:
```
Initial load success.
Get result = File keeper test.
Get result = File keeper modified.
```

stderr:
```
[INFO][2016-09-02T23:31:38.136+0800][835] msg=Auto-loading success.||filename=file_keeper.test
```
