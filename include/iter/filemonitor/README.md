### iter::FileMonitor ###
You can register your callback and the corresponding file system events to ```FileMonitor```, and when the registered file system events occured, ```FileMonitor``` will triger your callback immediately.

#### Definition ####
```cpp
class FileMonitor;
```
```cpp
struct FileMonitor::Node {
    std::string filename;
    uint32_t event_mask;
    std::function <void(const FileEvent&)> callback;
};
```

#### Member functions ####
| Member function | Description |
| ------ | ------ |
| (constructor) | Construct function. |
| Register | Register to FileMonitor. |
| IsRegistered | Registration check. |
| Remove | Remove from FileMonitor. |
| operator bool | Validation check. |

##### iter::FileMonitor::FileMonitor #####
```cpp
FileMonitor(int thread_pool_size = ITER_FILE_MONITOR_POOL_SIZE); // (1)
FileMonitor(const std::shared_ptr <ThreadPool>& thread_pool_ptr); // (2)
```
Constructor (1) will create a thread pool using ```thread_pool_size```.

##### iter::FileMonitor::Register #####
```cpp
int Register(const Node& node);
```
Register ```Node``` to ```FileMonitor```, the return value is the unique owner id of the registered ```Node```.

Return -1 means failed.

##### iter::FileMonitor::IsRegistered #####
```cpp
bool IsRegistered(int owner_id);
```
Check whether the owner id is registered.

##### iter::FileMonitor::Remove #####
```cpp
void Remove(int owner_id);
```
Remove the ```Node``` of the corresponding owner id.

##### iter::FileMonitor::operator bool #####
```
operator bool();
```
Return true if the instance of ```FileMonitor``` is valid.
