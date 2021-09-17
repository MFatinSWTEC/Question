#include <iostream>
#include <future>

void fun(char c, std::future<void>&& fun) {
    std::cout << c;
    fun.get();
    std::cout << c;
}

void print() {
    std::cout << "printer";
}

#define DEFERRED  std::launch::deferred
#define FUN(x, y) std::async(std::launch::deferred, fun, x, y)

int main()
{
    fun('+', FUN('?', FUN('8', std::async(DEFERRED, print))));
    std::cout << std::endl;
    fun('+', FUN('8', std::async(DEFERRED, print)));
}
