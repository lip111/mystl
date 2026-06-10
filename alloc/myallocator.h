#pragma once
#include <cstddef>
#include <utility>

namespace mystl 
{

template<typename T>
class LAllocator {
public:
    using value_type = T; // 必须要加上！分配器萃取器要用！

    LAllocator() = default;

    T* allocate(std::size_t n) {
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    template<typename... Types>
    void construct(T* p, Types&&... args) {
        new (p) T(std::forward<Types>(args)...);
    }

    void deallocate(T* p, std::size_t n) {
        ::operator delete(p);
    }

    void destroy(T* p) {
        p->~T();
    }

};

}