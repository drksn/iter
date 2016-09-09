# ITER #
An c++ library consist of some exquisite components to make things easy.

## Requirements ##
1. G++: --std=c++11 -pthread
2. Linux kernel: >= 2.6 (inotify for FileMonitor)

## Log settings ##
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
* [[FileMonitor](https://github.com/qianyl/iter/tree/master/include/iter/filemonitor)] Triger the registered callback immediately When the listened file system events occured. 
* [[FileKeeper](https://github.com/qianyl/iter/tree/master/include/iter/filekeeper)] Hot load file into your custom structure automatically.
