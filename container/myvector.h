#pragma once

#include "myallocator.h"
#include <memory>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <type_traits>

namespace mystl
{

template<typename T, typename Alloc = LAllocator<T>>
class LVector {
public:
    using iterator = T*;

    // 六大函数
    LVector(): first(nullptr), last(nullptr), cap(nullptr) {}
    LVector(const LVector& lv);
    LVector& operator=(const LVector& lv);
    LVector(LVector&& lv) noexcept;
    LVector& operator=(LVector&& lv) noexcept;
    ~LVector();

    // 查询容量
    bool empty() const { return first == last; } // 这里3个函数的const别忘了加！！！
    std::size_t size() const { return last - first; }
    std::size_t capacity() const { return cap - first; } 

    // 获取迭代器
    iterator begin() { return first; }
    iterator end() { return last; }
    const iterator cbegin() const { return first; }
    const iterator cend() const {return last; }

    // 访问元素
    T& operator[](std::size_t n) { return first[n]; }
    const T& operator[](std::size_t n) const { return first[n]; }
    T& at(std::size_t n);
    const T& at(std::size_t n) const;
    T& front() { return *first; }
    const T& front() const { return *first; }
    T& back() { return *(last-1); }
    const T& back() const { return *(last-1); }

    // 添加元素
    void push_back(const T& elem);
    void push_back(T&& elem);
    template<typename... Types>
    void emplace_back(Types&&... args);
    iterator insert(iterator it, const T& elem);
    iterator insert(iterator it, T&& elem);

    // 删除元素
    void pop_back();
    iterator erase(iterator it);
    iterator erase(iterator it1, iterator it2);
    void clear();

    // 扩容
    void reserve(std::size_t n);
    void resize(std::size_t n);
    void resize(std::size_t n, const T& val);

    // swap和列表初始化
    void swap(LVector& other) noexcept {
        std::swap(first, other.first);
        std::swap(last, other.last);
        std::swap(cap, other.cap);
    }

    LVector(std::initializer_list<T> ls): first(nullptr), last(nullptr), cap(nullptr) { // 别忘了初始化first等！！！
        reserve(ls.size());
        for (const auto& e: ls) {
            push_back(e);
        }
    }

private:
    T* first;
    T* last;
    T* cap;
    Alloc alloc;
};

template<typename T, typename Alloc>
LVector<T, Alloc>::LVector(const LVector<T, Alloc>& lv) {
    
    auto newfirst = std::allocator_traits<Alloc>::allocate(alloc, lv.size());
    auto newlast = newfirst;
    try {
        for(T* p = lv.first; p != lv.last; ++p) {
            std::allocator_traits<Alloc>::construct(alloc, newlast++, *p);
        }
    }
    catch(...) {
        for(T* p = newfirst; p != newlast; ++p) {
            std::allocator_traits<Alloc>::destroy(alloc, p);
        }
        std::allocator_traits<Alloc>::deallocate(alloc, newfirst, lv.size());
        throw;
    }

    first = newfirst;
    last = cap = newlast;
}

template<typename T, typename Alloc>
LVector<T, Alloc>::LVector(LVector<T, Alloc>&& lv) noexcept: 
    first(lv.first),last(lv.last),cap(lv.cap) {
        lv.first = lv.last = lv.cap = nullptr;
}

template<typename T, typename Alloc>
LVector<T, Alloc>& LVector<T, Alloc>::operator=(const LVector<T, Alloc>& lv) {
    
    if (this == &lv) return *this;
    auto temp_lv = lv;
    swap(temp_lv);
    return *this;
}

template<typename T, typename Alloc>
LVector<T, Alloc>& LVector<T, Alloc>::operator=(LVector<T, Alloc>&& lv) noexcept {
    if(this == &lv) return *this;
    swap(lv);
    // lv.first = lv.last = lv.cap = nullptr; // 不能置为nullptr,旧资源未释放导致内存泄漏！！！
    return *this;
}

template<typename T, typename Alloc>
LVector<T, Alloc>::~LVector() {

    for(T* p = first; p != last; ++p) {
        std::allocator_traits<Alloc>::destroy(alloc, p);
    }
    std::allocator_traits<Alloc>::deallocate(alloc, first, capacity());
    first = last = cap = nullptr;
}

template<typename T, typename Alloc>
void LVector<T, Alloc>::reserve(std::size_t n) {
    
    if (n <= capacity()) return;
    
    auto newfirst = std::allocator_traits<Alloc>::allocate(alloc, n);
    auto newlast = newfirst;
    try {
        for (T* p = first; p != last; ++p) {
            // 若T类型移动构造不是noexcept,下述代码就不是强异常安全，原对象管理资源被掏空，无法复原！！！
            // std::allocator_traits<Alloc>::construct(alloc, newlast++, std::move(*p)); 
            // 使用if constexpr优化
            if constexpr (std::is_nothrow_move_constructible_v<T>)
                std::allocator_traits<Alloc>::construct(alloc, newlast++, std::move(*p));
            else
                std::allocator_traits<Alloc>::construct(alloc, newlast++, *p);
        }
    } catch(...) {
        for (T* p = newfirst; p != newlast; ++p) {
            std::allocator_traits<Alloc>::destroy(alloc, p);
        }
        std::allocator_traits<Alloc>::deallocate(alloc, newfirst, n);
        throw;
    }

    for (T* p = first; p != last; ++p) {
        std::allocator_traits<Alloc>::destroy(alloc, p);
    }
    std::allocator_traits<Alloc>::deallocate(alloc, first, capacity());
    
    first = newfirst;
    last = newlast;
    cap = newfirst + n;
}

template<typename T, typename Alloc>
void LVector<T, Alloc>::resize(std::size_t n) {
    
    if(n == size()) return;
    if (n > capacity()) reserve(n);

    if(n < size()) {
        for(T* p = first+n; p != last; ++p) {
            std::allocator_traits<Alloc>::destroy(alloc, p);
        }
    }
    else {
        T* p = last;
        try {
            for(; p != first+n; ++p) {
                std::allocator_traits<Alloc>::construct(alloc, p);
            }
        }
        catch(...) {
            for(T* p2 = last; p2 != p; ++p2) {
                std::allocator_traits<Alloc>::destroy(alloc, p2);
            }   
            throw;
        }

    }
    last = first + n;
}

template<typename T, typename Alloc>
void LVector<T, Alloc>::resize(std::size_t n, const T& e) {
    
    if(n == size()) return;
    if (n > capacity()) reserve(n);

    if(n < size()) {
        for(T* p = first+n; p != last; ++p) {
            std::allocator_traits<Alloc>::destroy(alloc, p);
        }
    }
    else {
        T* p = last;
        try {
            for(; p != first+n; ++p) {
                std::allocator_traits<Alloc>::construct(alloc, p, e);
            }
        }
        catch(...) {
            for(T* p2 = last; p2 != p; ++p2) {
                std::allocator_traits<Alloc>::destroy(alloc, p2);
            }   
            throw;
        }

    }
    last = first + n;
}

template<typename T, typename Alloc>
void LVector<T, Alloc>::push_back(const T& elem) {
    
    if (last == cap) reserve(size() == 0 ? 1: 2 * size());
    std::allocator_traits<Alloc>::construct(alloc, last++, elem);
}

template<typename T, typename Alloc>
void LVector<T, Alloc>::push_back(T&& elem) {
    if (last == cap) reserve(size() == 0 ? 1: 2 * size());
    std::allocator_traits<Alloc>::construct(alloc, last++, std::move(elem));
}

template<typename T, typename Alloc>
template<typename... Types>
void LVector<T, Alloc>::emplace_back(Types&&... args) {
    if (last == cap) reserve(size() == 0 ? 1: 2 * size());
    std::allocator_traits<Alloc>::construct(alloc, last++, std::forward<Types>(args)...);
}

template<typename T, typename Alloc>
typename LVector<T, Alloc>::iterator LVector<T, Alloc>::insert(iterator it, const T& e) {
    if (last == cap) {
        std::ptrdiff_t gap = it - first;
        reserve(size() == 0 ? 1: 2*size()); 
        it = first + gap;
    }

    std::allocator_traits<Alloc>::construct(alloc, last);
    for(iterator p = last-1; p >= it; --p) {
        *(p+1) = std::move(*p);
    }

    *it = e;
    ++last;

    return it;
}

template<typename T, typename Alloc>
typename LVector<T, Alloc>::iterator LVector<T, Alloc>::insert(iterator it, T&& e) {
    if (last == cap) {
        std::ptrdiff_t gap = it - first;
        reserve(size() == 0 ? 1: 2*size()); 
        it = first + gap;
    }

    std::allocator_traits<Alloc>::construct(alloc, last);
    for(iterator p = last-1; p >= it; --p) {
        *(p+1) = std::move(*p);
    }

    *it = std::move(e);
    ++last;

    return it;   
}

template<typename T, typename Alloc>
void LVector<T, Alloc>::pop_back() {
    std::allocator_traits<Alloc>::destroy(alloc, --last);
}

template<typename T, typename Alloc>
typename LVector<T, Alloc>::iterator LVector<T, Alloc>::erase(iterator it) {
    for (iterator p = it+1; p != last; ++p) {
        *(p-1) = std::move(*p);
    }
    std::allocator_traits<Alloc>::destroy(alloc, --last);
    return it;
}

template<typename T, typename Alloc>
typename LVector<T, Alloc>::iterator LVector<T, Alloc>::erase(iterator it1, iterator it2) {
    for (iterator p = it2; p != last; ++p) {
        *(p-(it2-it1)) = std::move(*p);
    }
    while(it2 != it1) {
        std::allocator_traits<Alloc>::destroy(alloc, --last);
        --it2;
    }
    return it1;
}

template<typename T, typename Alloc>
void LVector<T, Alloc>::clear() {
    for (T* p = first; p != last; ++p) {
        std::allocator_traits<Alloc>::destroy(alloc, p);
    }
    last = first;
}

template<typename T, typename Alloc>
T& LVector<T, Alloc>::at(std::size_t pos) {
    if (pos >= size()) throw std::out_of_range("index out of range");
    return first[pos];
}

template<typename T, typename Alloc>
const T& LVector<T, Alloc>::at(std::size_t pos) const {
    if (pos >= size()) throw std::out_of_range("index out of range");
    return first[pos];  
}

}