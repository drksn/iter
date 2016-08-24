# ITER #
An c++ library consist of some exquisite components to make things easy.
## Requirements ##
1. G++: --std=c++11 -pthread
2. Linux kernel: >= 2.6 (inotify for FileMonitor)

## Log setting ##
Some components might print logs, if you add
```
ITER_LOG_INIT("./iter.log");
```
in the beginning of your code.

All of the logs will be redirectied to the file ```"./iter.log"```.

If not, the logs will be printed to ```stderr```.

## Components ##
* [[FileKeeper](https://github.com/qianyl/iter/tree/master/include/iter/filekeeper)] Hot load file and parse to your custom structure automatically.




