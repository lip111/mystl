#pragma once
#include <iterator>
#include <initializer_list>
#include "myallocator.h"

namespace mystl {

template<typename T, typename Alloc = LAllocator<T>>
class LDeque {

public:
    template<typename Ref, typename Ptr>
    struct LDequeIterator;

    using iterator = LDequeIterator<T&, T*>;
    using const_iterator = LDequeIterator<const T&, const T*>;

    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using allocator_type = Alloc;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;

    // 6大函数
    LDeque();
    LDeque(const LDeque& dq);
    LDeque& operator=(const LDeque& dq);
    LDeque(LDeque&& dq) noexcept;
    LDeque& operator=(LDeque&& dq) noexcept;
    ~LDeque();

    // 获取容量
    bool empty() const noexcept {
        return start_ == finish_;
    }
    size_type size() const noexcept {
        return finish_ - start_;
    }
    size_type max_size() const noexcept {
        return std::allocator_traits<Alloc>::max_size(alloc);
    }

    // 获取迭代器
    iterator begin() noexcept { return start_; }
    //外层类不能访问嵌套类中private成员，反过来可以！！！
    // const_iterator begin() const noexcept { return const_iterator(start_.cur_, start_.node_); }
    const_iterator begin() const noexcept { return start_; }
    iterator end() noexcept { return finish_; }
    const_iterator end() const noexcept { return finish_; }
    const_iterator cbegin() const noexcept { return start_; }
    const_iterator cend() const noexcept { return finish_; }

    // 访问元素
    T& operator[](size_type index) { return *(start_+index); }
    const T& operator[](size_type index) const { return *(start_+index); }
    T& at(size_type index) { if (index >= size()) throw std::out_of_range("deque index of out range"); return *(start_+index); }
    const T& at(size_type index) const { if (index >= size()) throw std::out_of_range("deque index of out range"); return *(start_+index); }
    T& front() { return *start_; }
    const T& front() const { return *start_; }
    T& back() { return *(finish_-1); }
    const T& back() const { return *(finish_-1); }

    // 添加元素
    void push_back(const T& e);
    void push_back(T&& e);
    template<typename... Args>
    void emplace_back(Args&&... args);

    void push_front(const T& e);
    void push_front(T&& e);
    template<typename... Args>
    void emplace_front(Args&&... args);

    iterator insert(const_iterator pos, const T& e);
    iterator insert(const_iterator pos, T&& e);
    template<typename InputIterator>
    iterator insert(const_iterator pos, InputIterator first, InputIterator last);
    template<typename... Args>
    iterator emplace(const_iterator pos, Args&&... args);

    // 删除元素
    void pop_back();
    void pop_front();
    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);
    void clear() noexcept;

    // swap和列表初始化构造
    void swap(LDeque& other) noexcept {
        std::swap(map_, other.map_);
        std::swap(map_size_, other.map_size_);
        std::swap(start_, other.start_);
        std::swap(finish_, other.finish_);
    }

    LDeque(std::initializer_list<T> ls): LDeque() {
        for(const auto& e: ls) {
            push_back(e);
        }
    }

private:
    T** map_;
    size_type map_size_;
    iterator start_;
    iterator finish_;
    Alloc alloc;
    using MapAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<T*>;
    MapAlloc map_alloc;
    // static const size_type BUFF_BYTES = 512; 
    static const size_type BUFF_BYTES = 64; // 方便测试
    static const size_type BUFF_SIZE = sizeof(T) < BUFF_BYTES ? BUFF_BYTES / sizeof(T) : 1;
    // static const size_type INITIAL_MAP_SIZE = 8;
    static const size_type INITIAL_MAP_SIZE = 3; // 方便测试

    // 扩容
    void reserve_map_at_back(size_type nodes_to_add = 1) { reallocate_map(nodes_to_add, false); }
    void reserve_map_at_front(size_type nodes_to_add = 1) { reallocate_map(nodes_to_add, true); }
    void reallocate_map(size_type nodes_to_add, bool add_at_front);


    // insert辅助函数
    iterator insert_base(const_iterator pos, size_type n);

    // erase辅助函数
    iterator erase_base(const_iterator first, const_iterator last);
};


template<typename T, typename Alloc>
template<typename Ref, typename Ptr>
struct LDeque<T, Alloc>::LDequeIterator {
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = Ptr;
    using reference = Ref;

    LDequeIterator():cur_(nullptr),first_(nullptr),last_(nullptr),node_(nullptr) {}
    
    // 注意下面map_slot对于iterator和const_iterator都是T**,这个const只是为了限制cur_指针！！！
    // LDequeIterator(Ptr cur_room, Ptr* map_slot):cur_(cur_room),node_(map_slot),first_(*map_slot),last_(*map_slot+BUFF_SIZE) {}
    LDequeIterator(Ptr cur_room, T** map_slot):cur_(cur_room),node_(map_slot),first_(*map_slot),last_(*map_slot+BUFF_SIZE) {}
    
    template<typename Ref2, typename Ptr2>
    friend struct LDequeIterator;

    template<typename Ref2, typename Ptr2, typename = std::enable_if_t<
            std::is_convertible_v<Ptr2, Ptr> && std::is_convertible_v<Ref2, Ref>>>
    LDequeIterator(const LDequeIterator<Ref2, Ptr2>& other):
        cur_(other.cur_), first_(other.first_), last_(other.last_), node_(other.node_) {}

    LDequeIterator& operator++() {
        ++cur_;
        if (cur_ < last_) return *this;
        ++node_;
        cur_ = *node_;
        first_ = *node_;
        last_ = first_+BUFF_SIZE;

        return *this;
    }

    LDequeIterator& operator--() {
        if(cur_ == first_) {
            --node_;
            first_ = *node_;
            last_ = first_+BUFF_SIZE;
            cur_ = last_;
        }
        --cur_;
        return *this;
    }

    LDequeIterator operator++(int) {
        LDequeIterator dq = *this;
        operator++();
        return dq;
    }

    LDequeIterator operator--(int) {
        LDequeIterator dq = *this;
        operator--();
        return dq;
    }

    Ref operator*() const {
        return *cur_;
    }

    Ptr operator->() const {
        return cur_;
    }

    bool operator==(const LDequeIterator& other) const {
        return cur_ == other.cur_;
    }

    bool operator!=(const LDequeIterator& other) const {
        return cur_ != other.cur_;
    }
    
    bool operator>(const LDequeIterator& other) const {
        if (node_ > other.node_) return true;
        if (node_ < other.node_) return false;
        if (cur_ > other.cur_) return true;
        return false; 
    }

    bool operator<(const LDequeIterator& other) const {
        if (node_ > other.node_) return false;
        if (node_ < other.node_) return true;
        if (cur_ > other.cur_) return false;
        return true; 
    }

    bool operator>=(const LDequeIterator& other) const {
        return operator>(other) || ((node_ == other.node_) && (cur_ == other.cur_));
    }

    bool operator<=(const LDequeIterator& other) const {
        return operator<(other) || ((node_ == other.node_) && (cur_ == other.cur_));
    }

    LDequeIterator operator+(difference_type n) const {
        LDequeIterator it = *this;  // 注意原迭代器内部数据不要有任何修改！！！
        it += n;
        return it;
    }

    LDequeIterator operator-(difference_type n) const {
        LDequeIterator it = *this;
        it -= n;
        return it;
    }

    difference_type operator-(const LDequeIterator& other) const {
        if (node_ > other.node_) 
            return (node_ - other.node_) * BUFF_SIZE + (cur_ - first_) - (other.cur_ - other.first_);
        if (node_ < other.node_)
            return -((node_ - other.node_) * BUFF_SIZE + (cur_ - first_) - (other.cur_ - other.first_));

        return cur_ - other.cur_;
    }

    LDequeIterator& operator+=(difference_type n) {
        
        // 转换为对当前node_.first_的偏移量
        difference_type offset = cur_ - first_ + n;
        difference_type buf_size = static_cast<difference_type>(BUFF_SIZE);
        if (offset >= 0) { 
            node_ += offset / buf_size;
            cur_ = *node_ + (offset % buf_size);
        }
        else {
            // BUFF_SIZE是无符号，offset有符号，除法结果是无符号，需要强转BUFF_SIZE!!!!!!
            node_ += (offset - (buf_size-1)) / buf_size;
            cur_ = *node_ + buf_size-1 + ((offset - (buf_size-1)) % buf_size);  // 需要确认！！！
        }
        first_ = *node_;
        last_ = first_ + BUFF_SIZE;
        return *this;
    }

    LDequeIterator& operator-=(difference_type n) {
        return operator+=(-n);
    }

    Ref operator[](difference_type n) const {
        return *operator+(n);
    }

private:
    Ptr cur_;
    Ptr first_;
    Ptr last_;
    T** node_;
    friend class LDeque; // 使得外部类可以访问嵌套类中的私有成员，可以使用get/set方法优化！！！ 
};

template<typename T, typename Alloc>
LDeque<T, Alloc>::LDeque(): map_size_(INITIAL_MAP_SIZE) {
    map_ = std::allocator_traits<MapAlloc>::allocate(map_alloc, map_size_);
    size_type initial_pos = map_size_ / 2;
    for(size_type i = 0; i < map_size_; ++i)
        map_[i] = nullptr;
    
    map_[initial_pos] = std::allocator_traits<Alloc>::allocate(alloc, BUFF_SIZE);

    start_ = iterator(map_[initial_pos], &map_[initial_pos]);
    finish_ = start_;
}

template<typename T, typename Alloc>
LDeque<T, Alloc>::LDeque(const LDeque& dq): LDeque() {
    const_iterator it = dq.begin();
    while(it != dq.end()) {
        push_back(*it);
        ++it;
    }
}

template<typename T, typename Alloc>
LDeque<T, Alloc>& LDeque<T, Alloc>::operator=(const LDeque& dq) {
    if(this == &dq) return *this;
    LDeque dq2 = dq;
    swap(dq2);
    return *this;
}

template<typename T, typename Alloc>
LDeque<T, Alloc>::LDeque(LDeque&& dq) noexcept:
    map_(dq.map_),map_size_(dq.map_size_),start_(dq.start_),finish_(dq.finish_) {
    dq.map_ = nullptr;
    dq.map_size_ = 0;
    dq.start_ = iterator();
    dq.finish_ = iterator();
}

template<typename T, typename Alloc>
LDeque<T, Alloc>& LDeque<T, Alloc>::operator=(LDeque&& dq) noexcept {
    if(this == &dq) return *this;
    swap(dq);
    return *this;
}

template<typename T, typename Alloc>
LDeque<T, Alloc>::~LDeque() {
    if (map_ == nullptr) return;
    clear();
    for(size_type i = 0; i < map_size_; ++i) {
        if (map_[i] != nullptr) 
            std::allocator_traits<Alloc>::deallocate(alloc, map_[i], BUFF_SIZE);
    }
    std::allocator_traits<MapAlloc>::deallocate(map_alloc, map_, map_size_);
}

template<typename T, typename Alloc>
void LDeque<T, Alloc>::clear() noexcept {

    iterator it = begin();
    while(it != end()) {
        std::allocator_traits<Alloc>::destroy(alloc, it.cur_);
        ++it;
    }
    finish_ = start_;
}

template<typename T, typename Alloc>
void LDeque<T, Alloc>::reallocate_map(size_type nodes_to_add, bool add_at_front) {
    size_type n = finish_.node_ - start_.node_ + 1;
    if(map_size_ >= 8 && map_size_ > 2 * n) {
        size_type newpos = (map_size_ - n) / 2;
        T** p = start_.node_;
        while(p <= finish_.node_) {
            map_[newpos++] = *p;
            *p = nullptr;
            --p;
        }
        finish_.node_ = map_ + newpos - 1;
        start_.node_ = finish_.node_ - n;
        return;
    }

    size_type new_map_size_ = std::max(map_size_ * 2, n + nodes_to_add + 2);
    T** new_map_ = std::allocator_traits<MapAlloc>::allocate(map_alloc, new_map_size_);
    for(size_type i = 0; i < new_map_size_; ++i) 
        new_map_[i] = nullptr;
    
    size_type newpos = 0;
    if (add_at_front)
        newpos = new_map_size_ - n - 1;  // 如果扩容是左边告急，原buff尽量往右边挪，右边只留一个空槽位，方便左边大量数据插入
    else
        newpos = 1;
    difference_type oldpos = start_.node_ - map_;
    for(size_type i = 0; i < n; ++i)
        new_map_[newpos+i] = map_[oldpos+i];

    // 别忘了释放原来的map_！！！
    std::allocator_traits<MapAlloc>::deallocate(map_alloc, map_, map_size_);

    map_ = new_map_;
    map_size_ = new_map_size_;
    start_.node_ = new_map_ + newpos;
    finish_.node_ = start_.node_ + n-1;
}


template<typename T, typename Alloc>
void LDeque<T, Alloc>::push_back(const T& e) {
    
    // finish已经指向本槽位最后一个空位，而且finish所在的槽位已经是最后一个槽位,则需要平移槽位或者扩容map
    if((finish_.cur_ == finish_.last_-1) && (finish_.node_ == map_+map_size_-1))
        reserve_map_at_back();

    // 本槽位只剩一个空位，提前新建一个chunk
    if (finish_.cur_ == finish_.last_-1) {
        map_[finish_.node_-map_+1] = std::allocator_traits<Alloc>::allocate(alloc, BUFF_SIZE);
    }
    std::allocator_traits<Alloc>::construct(alloc, finish_.cur_, e);
    ++finish_;
}

template<typename T, typename Alloc>
void LDeque<T, Alloc>::push_back(T&& e) {

    if((finish_.cur_ == finish_.last_-1) && (finish_.node_ == map_+map_size_-1))
        reserve_map_at_back();

    if (finish_.cur_ == finish_.last_-1) {
        map_[finish_.node_-map_+1] = std::allocator_traits<Alloc>::allocate(alloc, BUFF_SIZE);
    }

    std::allocator_traits<Alloc>::construct(alloc, finish_.cur_, std::move(e));
    ++finish_;
}

template<typename T, typename Alloc>
template<typename... Args>
void LDeque<T, Alloc>::emplace_back(Args&&... args) {
    
    if((finish_.cur_ == finish_.last_-1) && (finish_.node_ == map_+map_size_-1))
        reserve_map_at_back();

    if (finish_.cur_ == finish_.last_-1) {
        map_[finish_.node_-map_+1] = std::allocator_traits<Alloc>::allocate(alloc, BUFF_SIZE);
    }

    std::allocator_traits<Alloc>::construct(alloc, finish_.cur_, std::forward<Args>(args)...);
    ++finish_;
}

template<typename T, typename Alloc>
void LDeque<T, Alloc>::push_front(const T& e) {

    // 如果start已经指向本槽位起点，而且start已经是第0个槽位了，需要平移槽位或者扩容map
    if ((start_.cur_ == start_.first_) && (start_.node_ == map_))
        reserve_map_at_front();
    
    // 本槽位满了，需要新建一个chunk
    if(start_.cur_ == start_.first_) {
        map_[start_.node_-map_-1] = std::allocator_traits<Alloc>::allocate(alloc, BUFF_SIZE);
    }
    --start_;
    std::allocator_traits<Alloc>::construct(alloc, start_.cur_, e);
}

template<typename T, typename Alloc>
void LDeque<T, Alloc>::push_front(T&& e) {
    
    // 如果start已经指向本槽位起点，而且start已经是第0个槽位了，需要平移槽位或者扩容map
    if ((start_.cur_ == start_.first_) && (start_.node_ == map_))
        reserve_map_at_front();
    
    // 本槽位满了，需要新建一个chunk
    if(start_.cur_ == start_.first_) {
        map_[start_.node_-map_-1] = std::allocator_traits<Alloc>::allocate(alloc, BUFF_SIZE);
    }
    --start_;
    std::allocator_traits<Alloc>::construct(alloc, start_.cur_, std::move(e));
}

template<typename T, typename Alloc>
template<typename... Args>
void LDeque<T, Alloc>::emplace_front(Args&&... args) {
    
    // 如果start已经指向本槽位起点，而且start已经是第0个槽位了，需要平移槽位或者扩容map
    if ((start_.cur_ == start_.first_) && (start_.node_ == map_))
        reserve_map_at_front();
    
    // 本槽位满了，需要新建一个chunk
    if(start_.cur_ == start_.first_) {
        map_[start_.node_-map_-1] = std::allocator_traits<Alloc>::allocate(alloc, BUFF_SIZE);
    }
    --start_;
    std::allocator_traits<Alloc>::construct(alloc, start_.cur_, std::forward<Args>(args)...);
}

template<typename T, typename Alloc>
typename LDeque<T, Alloc>::iterator LDeque<T, Alloc>::insert_base(const_iterator pos, size_type n) {
    
    size_type offset = pos - cbegin();
    if ((pos - cbegin()) < (cend() - pos)) {
        // 如果start往前再插入n个元素(中间可能会申请若干个槽位)，会导致start移动到第0号槽位0号房间左边去了，这时就需要平移槽位或者扩容map 
        size_type rooms = (start_.node_ - map_) * BUFF_SIZE + start_.cur_ - start_.first_;
        if (n > rooms) 
            reserve_map_at_front((n-rooms+BUFF_SIZE-1)/BUFF_SIZE);

        // 如果发生map_扩容或者chunk左右移动,需更新pos,这里叫做old_pos,在它的左边插入n个元素
        iterator old_pos = start_ + offset;

        iterator dst_start = start_;
        for (size_type i = 0; i < n; ++i) {
            if (dst_start.cur_ == dst_start.first_) // 此时左侧没空余槽位,需要先申请chunk,再移动
                map_[dst_start.node_-map_-1] = std::allocator_traits<Alloc>::allocate(alloc, BUFF_SIZE);
            --dst_start;
        }

        // [start_, old_pos)的元素需要左移n
        iterator src = start_; // 代表[start_, old_pos)之间的元素,正序挪
        iterator dst = dst_start;
        while(src != old_pos) {
            std::allocator_traits<Alloc>::construct(alloc, dst.cur_, std::move(*src.cur_));
            std::allocator_traits<Alloc>::destroy(alloc, src.cur_);
            ++src;
            ++dst;
        }

        start_ = dst_start;
        // 新元素插入的第一个位置
        return dst_start + offset;
    }
    else {
        // 如果finish往后再插入n个元素(中间可能会申请若干个槽位)，会导致finish超出第(map_size_-1)号槽位last_的位置，这时就需要平移槽位或者扩容map
        size_type rooms = (map_size_ - (finish_.node_ - map_ + 1)) * BUFF_SIZE + finish_.last_ - finish_.cur_;
        if (n > rooms) 
            reserve_map_at_back((n-rooms+BUFF_SIZE-1)/BUFF_SIZE);

        // 如果发生map_扩容或者chunk左右移动,需更新pos,这里叫做old_pos,在它的左边插入n个元素
        iterator old_pos = start_ + offset;

        iterator dst_finish = finish_;
        for (size_type i = 0; i < n; ++i) {
            if (dst_finish.cur_ == dst_finish.last_) { // 此时右侧侧没空余槽位,需要先申请chunk,再移动
                map_[dst_finish.node_-map_+1] = std::allocator_traits<Alloc>::allocate(alloc, BUFF_SIZE);
            }
            ++dst_finish;
        }
        
        // [old_pos, finish_)的元素需要右移n
        iterator src = finish_-1; // 代表[old_pos, finish_)间的元素,逆序挪
        iterator dst = dst_finish-1;
        while(src >= old_pos) {
            std::allocator_traits<Alloc>::construct(alloc, dst.cur_, std::move(*src.cur_));
            std::allocator_traits<Alloc>::destroy(alloc, src.cur_);
            --src;
            --dst;           
        }
        
        finish_ = dst_finish;
        // 新元素插入第一个位置
        return old_pos;
    }   // 未做强异常安全，可优化！！！！
}

template<typename T, typename Alloc>
typename LDeque<T, Alloc>::iterator LDeque<T, Alloc>::insert(const_iterator pos, const T& e) {   
    iterator it = insert_base(pos, 1);
    std::allocator_traits<Alloc>::construct(alloc, it.cur_, e);
    return it;
}

template<typename T, typename Alloc>
typename LDeque<T, Alloc>::iterator LDeque<T, Alloc>::insert(const_iterator pos, T&& e) {
    iterator it = insert_base(pos, 1);
    std::allocator_traits<Alloc>::construct(alloc, it.cur_, std::move(e));
    return it;
}

template<typename T, typename Alloc>
template<typename InputIterator>
typename LDeque<T, Alloc>::iterator LDeque<T, Alloc>::insert(const_iterator pos, InputIterator first, InputIterator last) {
    size_type n = std::distance(first, last);
    iterator it = insert_base(pos, n);
    InputIterator src = first;
    iterator dst = it;
    while(src != last) {
        std::allocator_traits<Alloc>::construct(alloc, dst.cur_, *src);
        ++src;
        ++dst;
    }
    return it;
}


template<typename T, typename Alloc>
template<typename... Args>
typename LDeque<T, Alloc>::iterator LDeque<T, Alloc>::emplace(const_iterator pos, Args&&... args) {
    iterator it = insert_base(pos, 1);
    std::allocator_traits<Alloc>::construct(alloc, it.cur_, std::forward<Args>(args)...);
    return it;
}

template<typename T, typename Alloc>
void LDeque<T, Alloc>::pop_back() {
    
    --finish_;
    std::allocator_traits<Alloc>::destroy(alloc, finish_.cur_);

    if (finish_.cur_ == finish_.last_-1)
        std::allocator_traits<Alloc>::deallocate(alloc, *(finish_.node_+1), BUFF_SIZE);
}

template<typename T, typename Alloc>
void LDeque<T, Alloc>::pop_front() {
    if (start_.cur_ == start_.last_-1) {
        std::allocator_traits<Alloc>::destroy(alloc, start_.cur_);
        ++start_;
        std::allocator_traits<Alloc>::deallocate(alloc, *(start_.node_-1), BUFF_SIZE);
    }
    else {
        std::allocator_traits<Alloc>::destroy(alloc, start_.cur_);
        ++start_;
    }
}

template<typename T, typename Alloc>
typename LDeque<T, Alloc>::iterator LDeque<T, Alloc>::erase_base(const_iterator first, const_iterator last) {

    iterator first1 = iterator(const_cast<T*>(first.cur_), first.node_);
    iterator last1 = iterator(const_cast<T*>(last.cur_), last.node_);
    size_type n = last1 - first1;
    
    iterator it = first1;
    while(it != last1) {
        std::allocator_traits<Alloc>::destroy(alloc, it.cur_);
        ++it;
    }

    // 处理开头开始删，或者删到最后一个元素的情况
    if (first1 == begin()) { start_ += n; return last1; }
    if (last1 == end()) { finish_ -= n; return first1-1;}

    if (first1-start_ < finish_-last1+1) {
        iterator src = first1 - 1;
        iterator dst = last1 - 1;
        while(src >= start_) {  // 如果删除左右两侧元素有bug,函数开头判断下！！！
            std::allocator_traits<Alloc>::construct(alloc, dst.cur_, std::move(*src));
            std::allocator_traits<Alloc>::destroy(alloc, src.cur_);
            --src;
            --dst;
        }

        // T** p = start_.node_;
        // start_ += n;
        // for(;p != start_.node_; ++p)
        //     std::allocator_traits<Alloc>::deallocate(alloc, *p, BUFF_SIZE);

        return last1;
    }
    else {
        iterator src = last1;
        iterator dst = first1;
        while(src < finish_) {
            std::allocator_traits<Alloc>::construct(alloc, dst.cur_, std::move(*src));
            std::allocator_traits<Alloc>::destroy(alloc, src.cur_);
            ++src;
            ++dst;
        }
        
        // T** p = finish_.node_;
        // finish_ -= n;
        // for(;p != finish_.node_; --p)
        //     std::allocator_traits<Alloc>::deallocate(alloc, *p, BUFF_SIZE);

        return first1;
    }

}

template<typename T, typename Alloc>
typename LDeque<T, Alloc>::iterator LDeque<T, Alloc>::erase(const_iterator pos) {
    return erase_base(pos,pos+1);
}

template<typename T, typename Alloc>
typename LDeque<T, Alloc>::iterator LDeque<T, Alloc>::erase(const_iterator first, const_iterator last) {
    return erase_base(first, last);
}
























}