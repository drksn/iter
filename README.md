# ITER #
An c++ library consist of some exquisite components to make things easy.

## Requirements ##
1. G++: --std=c++11 -pthread
2. Linux kernel: >= 2.6 (inotify for FileMonitor)

## Log setting ##
Some components might print logs to ```stderr```.

You can redirect the logs to your target file by adding  ```ITER_LOG_INIT("path/filename")``` at the beginning of your code.

```#define ITER_LOG_DISABLE``` can disable all of the iter-logs.

## Components ##
* [[FileKeeper](https://github.com/qianyl/iter/tree/master/include/iter/filekeeper)] Hot load file and parse to your custom structure automatically.




