# Desription

## Main idea

Implement the program that allow users to make their tasks in 2 and more threads

## Implementation and API

* init(size_t n) - get a number of threads and create them
* push(std::future<void>&& fun) - get a function and push in the task queue (that queue is unique for all threads and threads use it to pop a task)
* wait() - join all threads
  
## Question
