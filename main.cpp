#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <vector>
#include <future>
#include <queue>

class ThreadPool {
public:

    static void init(size_t n) {
        std::unique_lock<std::mutex> lock(write_mtx);
        for (size_t i = 0; i < n; ++i) {
            threads.emplace_back(std::thread(ThreadPool::work));
        }
        is_init = true;
    }

    static void push(std::future<void>&& fun) {
        std::unique_lock<std::mutex> lock(write_mtx);
        if (!is_init) {
            throw -1;
        }
        q.emplace(std::move(fun));
        cv.notify_one();
    }

    static void wait() noexcept {
        while (!q.empty() || !is_init) {
            cv.notify_all();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        quit = true;
        cv.notify_all();
        for (auto & thread : threads) {
            thread.join();
        }
    }

private:
    static inline std::mutex write_mtx;
    static inline std::mutex cv_mtx;

    static inline std::condition_variable cv;

    static inline std::queue<std::future<void>> q;

    static inline std::vector<std::thread> threads; 

    static inline std::atomic_bool quit{false};

    static inline std::atomic_bool is_init{false};

    static void work() {
        while (!quit) {
            std::unique_lock<std::mutex> lock(cv_mtx);
            cv.wait(lock, []()->bool{return quit || !q.empty();});

            if (!q.empty()) {
                auto func = std::move(q.front());
                q.pop();
                lock.unlock();
                func.get();
            }
        }

    }

};

void my_foo() {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cerr << "ok!" << std::endl;
}

void test_foo() {
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cerr << "ok!" << std::endl;
}

int main() {
    ThreadPool::init(2);
    ThreadPool::push(std::async(std::launch::deferred, my_foo));
    ThreadPool::push(std::async(std::launch::deferred, test_foo));
    ThreadPool::push(std::async(std::launch::deferred, my_foo));
    ThreadPool::push(std::async(std::launch::deferred, my_foo));
    ThreadPool::push(std::async(std::launch::deferred, my_foo));
    ThreadPool::push(std::async(std::launch::deferred, my_foo));
    ThreadPool::wait();
    return 0;
}
