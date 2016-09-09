# ITER #
An c++ library consist of some exquisite components to make things easy.

## Requirements ##
1. G++: --std=c++11 -pthread
2. Linux kernel: >= 2.6 (inotify for FileMonitor)

## Log setting ##
Some components might print logs to ```stderr```.

You can redirect the log destination of each level by using:
```
iter::SetLogDestination(iter::INFO, "notice.log");
iter::SetLogDestination(iter::WARN, "warning.log");
iter::SetLogDestination(iter::ERROR, "error.log");
iter::SetLogDestination(iter::FATAL, "error.log");
```

Compile options: 
* ```-DITER_LOG_DISABLE``` : Disable all of the logs.

## Components ##
* [[FileKeeper](https://github.com/qianyl/iter/tree/master/include/iter/filekeeper)] Hot load file and parse to your custom structure automatically.

