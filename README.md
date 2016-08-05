# ITER #
An c++ library consist of some exquisite components to make things easy.
## Requirements ##
1. G++: --std=c++11 -pthread -luuid
2. Linux kernel: >= 2.6 (inotify)

## Log setting ##
Some components might print logs, if you add:
```
ITER_LOG_INIT("./iter.log");
```
In your code.

All of the log will be redirectied to the file ```"./iter.log"```. If not, the log will be printed to ```stderr```. 

## Components ##
* [[FileKeeper](https://github.com/qianyl/iter/tree/master/include/iter/filekeeper)] Hot load file and parse to your custom structure automatically.
* [[Link](https://github.com/qianyl/iter/tree/master/include/iter/link)] Combine the binary functors (which is formed as 'bool func(TypeA, TypeB)').




