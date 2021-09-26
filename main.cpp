#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <typeinfo>
#include <array>
#include <type_traits>


class DefaultPointerDeleter {
public:
    template<class T>
    void operator()(T* ptr) {
        delete ptr;
    }
};

class DefaultArrayDeleter {
public:
    
    template<class T>
    static void del(T* ptr) {
        delete[] ptr;
    }
};

class FileDeleter {
public:
    template<class T>
    void operator()(T* file) {
        std::fclose(file);
    }
};


class xRay {
public:
    xRay() {
        std::cout << __FUNCTION__ << std::endl;
    }


    xRay(const xRay& other) {
        std::cout << __FUNCTION__ << std::endl;
    }

    xRay(const xRay&& other) {
        std::cout << __FUNCTION__ << std::endl;
    }

    ~xRay() {
        std::cout << __FUNCTION__ << std::endl;
    }
};



template<class T, class Deleter = DefaultPointerDeleter>
class shared_ptr1 : Deleter {
public:
    template<class U>
    shared_ptr1(U* data) : instanceCount{ new int{1} } {
        std::cout << __FUNCTION__ << std::endl;
        
        //this->data = data;
        std::cout << sizeof(data) << std::endl;
        std::cout << sizeof(U) << std::endl;
         
        std::cout << "id - " << typeid(U*).name() << std::endl;
        std::cout << "id - " << typeid(T*).name() << std::endl;
    }
    ~shared_ptr1() {
        std::cout << __FUNCTION__ << std::endl;
        if (instanceCount && (--(*instanceCount) == 0)) {
            if constexpr (std::is_array_v<T> && std::is_base_of_v<Deleter, DefaultPointerDeleter>) {
                //DefaultArrayDeleter::del(data);
                delete[] data;
            }
            else {
                Deleter::del(data);
            }
            //d1.del(data);
            // std::cout << "id - " << typeid(d1).name() << std::endl;
            delete instanceCount;
        }
    }

private:
    T* data;
    Deleter d1;
    // std::decay_t<T> data;
    int* instanceCount;
};


template <class Deleter, class T>
void foo(T* t) {
    
    //Deleter d;
    //d(t);
   // Deleter
}

// https://habr.com/ru/post/263751/
int main() {
    //xRay* arr = new xRay[5];
   // DefaultArrayDeleter::del(arr);
    //std::shared_ptr<xRay[]> p(new xRay[5]);

    //shared_ptr1<xRay[]> pt(new xRay[5]);
    shared_ptr1<int[]> pt(new int[5]);
}
