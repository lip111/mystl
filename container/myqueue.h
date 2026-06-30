#pragma once
#include "mydeque.h"

namespace mystl {


template<typename T, typename Container = LDeque<T>>
class LQueue {
public:

    using value_type = typename Container::value_type;
    using size_type = typename Container::size_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;
    using container_type = Container;

    // 6大函数
    LQueue() {}
    LQueue(const LQueue& q) = delete;
    LQueue& operator=(const LQueue& q) = delete;
    LQueue(LQueue&& q) = delete;
    LQueue& operator=(LQueue&& q) = delete;

    // 获取容量
    bool empty() { return c_.empty(); }
    size_type size() { return c_.size(); }

    // 访问元素
    reference front() { return c_.front(); }
    const_reference front() const { return c_.front(); }
    reference back() { return c_.back(); }
    const_reference back() const { return c_.back(); }

    // 增加元素
    void push(const value_type& e) { c_.push_back(e); }
    void push(value_type&& e) { c_.push_back(std::move(e)); }
    template<typename... Args>
    void emplace(Args&&... args) { c_.emplace_back(std::forward<Args>(args)...); }

    // 删除元素
    void pop() { c_.pop_front(); }

protected:
    Container c_;

};


}