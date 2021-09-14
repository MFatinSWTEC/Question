# Desription

## Main idea

Implement the program that allow users to make their tasks in 2 and more threads

## Implementation and API

* ```init(size_t n)``` - get a number of threads and create them
* ```push(std::future<void>&& fun)``` - get a function and push in the task queue (that queue is unique for all threads and threads use it to pop a task)
* ```wait()``` - join all threads
* If you want to push function, you needs to use this syntaxis ```std::async(std::launch::deferred,YOUR_FUNCTIONS)```. Also you able to add some argumets 
  
  
## Question

1) I am not shure in ```wait()``` function implementation. It seams bad guidelines to use ```std::this_thread::sleep_for(std::chrono::milliseconds(100));```
2) I have this implementation now ```std::vector<std::thread> threads```, but also it may be done by ```std::packaged_task``` or ```async``` array. What is the best choise?
3) Function ```push``` able to throw exception. Need I create my own exception?
4) ThreadPool in my implementation is a singleton. What can be done to use ```whait()``` if user don't done it by hisself
