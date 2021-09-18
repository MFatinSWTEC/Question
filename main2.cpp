#include <iostream>
#include <future>

void fun(char c, std::future<void>&& fun) {
    std::cout << c;
    fun.get();
    std::cout << c;
}



#define DEFERRED  std::launch::deferred
#define FUN(x, y) std::async(std::launch::deferred, fun, x, y)

struct P {
    void print() {
        std::cout << "printer";
    }
};


int main()
{
    P p;
    auto func = [](P p) {p.print();};
    fun('+', FUN('?', FUN('8', std::async(DEFERRED, func, p))));
    std::cout << std::endl;
    fun('+', FUN('8', std::async(DEFERRED, func, p)));
}
