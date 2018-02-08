# utils
Some C++ utils classes in C++11 to be used on platforms where no C++14/17 is available.

This repository has the following utils:
* Optional. It's a class that may or may not store a value.
* ActiveWorker. It's a class that runs a thread that executes tasks from its task queue.
* ThreadPool. It's a thread pool consisting on a vector of ActiveWorkers .
