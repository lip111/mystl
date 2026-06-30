#pragma once
#include "mydeque.h"

namespace mystl {


template<typename T, typename Container = LDeque<T>>
class LStack {

public:

    using value_type = typename Container::value_type;
    using size_type = typename Container::size_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;
    using container_type = Container;

    // 6大函数
    LStack() {};
    LStack(const LStack& st) = delete;
    LStack& operator=(const LStack& st) = delete;
    LStack(LStack&& st) = delete;
    LStack& operator=(LStack&& st) = delete;
    ~LStack() {}

    // 查询容量
    bool empty() const noexcept { return c_.empty(); }
    size_type size() const noexcept { return c_.size(); }

    // 访问元素
    reference top() { return c_.back(); }
    const_reference top() const { return c_.back(); }

    // 增加元素
    void push(const value_type& e) { c_.push_back(e); }
    void push(value_type&& e) { c_.push_back(std::move(e)); }
    template<typename... Args>
    void emplace(Args&&... args) { c_.emplace_back(std::forward<Args>(args)...); }

    // 删除元素
    void pop() { c_.pop_back(); }
    

protected:
    Container c_;

};


}