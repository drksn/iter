# ITER #
An c++ library consist of some exquisite components to make things easy.

## Requirements ##
1. G++: --std=c++11 -pthread
2. Linux kernel: >= 2.6 (inotify for FileMonitor)

## Log setting ##
Some components might print logs.

The default log destination is ```stderr```.

You can set the log destination for each log level by using:
```
iter::SetLogDestination(iter::INFO, "notice.log");
iter::SetLogDestination(iter::WARN, "warning.log");
iter::SetLogDestination(iter::ERROR, "error.log");
iter::SetLogDestination(iter::FATAL, "error.log");
```

You can disable all of the logs by using:
```
#define ITER_LOG_DISABLE
```

## Components ##
* [[FileKeeper](https://github.com/qianyl/iter/tree/master/include/iter/filekeeper)] Hot load file and parse to your custom structure automatically.

