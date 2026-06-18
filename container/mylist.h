#pragma once
#include "myMemoryPool.h"
#include <initializer_list>
#include <memory>
#include <utility>

namespace mystl {

template<typename T, typename Alloc=LMemoryPoolWrapper<T>>
// template<typename T, typename Alloc=std::allocator<T>>
class LList {

public:
    struct ListIterator;
    struct ConstListIterator;

    using value_type = T;
    using allocator_type = Alloc;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using iterator = ListIterator;
    using const_iterator = ConstListIterator;
  
    // 6大函数+列表初始化构造
    LList(): size_(0) { 
        dummy = std::allocator_traits<NodeAlloc>::allocate(node_alloc, 1);
        std::allocator_traits<NodeAlloc>::construct(node_alloc, dummy);
    }
    LList(const LList& ls);
    LList& operator=(const LList& ls);
    LList(LList&& ls) noexcept;
    LList& operator=(LList&& ls) noexcept;
    ~LList();

    // 查询容量
    bool empty() const noexcept { return size_ == 0; }
    size_type size() const noexcept { return size_; }
    size_type max_size() const noexcept { return std::allocator_traits<NodeAlloc>::max_size(node_alloc); }

    // 获取迭代器
    iterator begin() noexcept { return ListIterator(dummy->next); }
    const_iterator begin() const noexcept { return ConstListIterator(dummy->next); }
    iterator end() noexcept { return ListIterator(dummy); }
    const_iterator end() const noexcept { return ConstListIterator(dummy); }
    const_iterator cbegin() const noexcept { return ConstListIterator(dummy->next); }
    const_iterator cend() const noexcept { return ConstListIterator(dummy); }

    // 访问元素
    reference front() { return dummy->next->data; }
    const_reference front() const { return dummy->next->data; }
    reference back() { return dummy->prev->data; }
    const_reference back() const { return dummy->prev->data; }

    // 添加元素
    void push_front(const_reference e);
    void push_front(T&& e);
    template<typename... Args>
    void emplace_front(Args&&... args);

    void push_back(const_reference e);
    void push_back(T&& e);
    template<typename... Args>
    void emplace_back(Args&&... args);

    iterator insert(const_iterator pos, const_reference e);
    iterator insert(const_iterator pos, T&& e);
    template<typename InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last);
    template<typename... Args>
    iterator emplace(const_iterator pos, Args&&... args);
    
    // 删除元素
    void pop_front();
    void pop_back();
    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);
    void remove(const_reference val);
    template<typename Predicate>
    void remove_if(Predicate pred);
    void clear();

    //swap和列表初始化
    void swap(LList& ls) noexcept {
        std::swap(dummy, ls.dummy);
        std::swap(size_, ls.size_);
    }
    LList(std::initializer_list<T> ls): LList() {
        for(const auto& e: ls) {
            push_back(e);
        }
    }

private:
    struct Node {
        Node* prev;
        Node* next;
        T data;

        Node(): prev(this),next(this) {}
        Node(const T& val):data(val),prev(nullptr),next(nullptr) {}
        template<typename... Args>
        Node(Args... args):data(std::forward<Args>(args)...),prev(nullptr),next(nullptr){}
    };

    Node* dummy;
    std::size_t size_;
    Alloc alloc;
    using NodeAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
    NodeAlloc  node_alloc;

    template<typename... Args>
    Node* create_node(Args&&... args);
};

template<typename T, typename Alloc>
struct LList<T, Alloc>::ListIterator {

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    ListIterator(Node *n): node(n) {}

    ListIterator& operator++() {
        node = node->next;
        return *this;
    }

    ListIterator& operator--() {
        node = node->prev;
        return *this;
    }

    ListIterator operator++(int) {
        ListIterator it = *this;
        node = node->next;
        return it;
    }

    ListIterator operator--(int) {
        ListIterator it = *this;
        node = node->prev;
        return it;
    }

    reference operator*() const {
        return node->data;
    }

    pointer operator->() const {
        return &node->data;
    }

    bool operator==(const ListIterator& other) const {
        return node == other.node;
    }

    bool operator!=(const ListIterator& other) const {
        return node != other.node;
    }

private:
    Node *node;
    friend class LList;
};

template<typename T, typename Alloc> 
struct LList<T, Alloc>::ConstListIterator {
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;

    ConstListIterator(const Node* n):node(n) {}
    ConstListIterator(const ListIterator& ls):node(ls.node) {}

    ConstListIterator& operator++() { //前置++
        node = node->next;
        return *this;
    }

    ConstListIterator& operator--() {
        node = node->prev;
        return*this;
    }

    ConstListIterator operator++(int) {
        ConstListIterator it = *this;
        node = node->next;
        return it;
    }

    ConstListIterator operator--(int) {
        ConstListIterator it = *this;
        node = node->prev;
        return it;
    }

    reference operator*() const {
        return node->data;
    }

    pointer operator->() const {
        return &node->data;
    }

    bool operator==(const ConstListIterator& other) const {
        return node == other.node;
    }

    bool operator!=(const ConstListIterator& other) const {
        return node != other.node;
    }

private:
    const Node* node;
    friend class LList;
};

template<typename T, typename Alloc>
LList<T, Alloc>::LList(const LList<T, Alloc>& ls): LList() {  // 需要先把哨兵节点dummy创建出来！！！
    if (ls.empty()) return;
    Node* newHead = nullptr;
    Node* newTail = nullptr;
    try {
        for (Node* node = ls.dummy->next; node != ls.dummy; node = node->next) {
            Node* newNode = static_cast<Node*>(std::allocator_traits<NodeAlloc>::allocate(node_alloc, 1));
            std::allocator_traits<NodeAlloc>::construct(node_alloc, newNode, node->data);
            if (!newTail) {
                newHead = newNode;
            }
            else {
                newTail->next = newNode;
                newNode->prev = newTail;
            }
            newTail = newNode;
        } 
    }
    catch(...) {
        // for循环这段可注释掉测试异常场景内存泄漏
        for(Node* node = newHead; node != nullptr;) {
            Node* temp = node->next; // 注意node后面要销毁，先保存！！！
            std::allocator_traits<NodeAlloc>::destroy(node_alloc, node);
            std::allocator_traits<NodeAlloc>::deallocate(node_alloc, node, 1);
            node = temp;
        }
        throw;
    }

    newTail->next = dummy;
    dummy->prev = newTail;
    dummy->next = newHead;
    newHead->prev = dummy;
    size_ = ls.size_;
}

template<typename T, typename Alloc>
LList<T, Alloc>::LList(LList&& ls) noexcept : 
        dummy(ls.dummy), size_(ls.size_) {
    ls.dummy = nullptr;
    ls.size_ = 0;
}

template<typename T, typename Alloc>
LList<T, Alloc>& LList<T, Alloc>::operator=(const LList& ls) {

    if(this == &ls) return *this;
    LList ls2 = ls;
    swap(ls2);
    return *this;
}

template<typename T, typename Alloc>
LList<T, Alloc>& LList<T, Alloc>::operator=(LList&& ls) noexcept {
    if(this == &ls) return *this;
    swap(ls);
    return *this;
}

template<typename T, typename Alloc>
LList<T, Alloc>::~LList() {

    if(!dummy)  return;

    clear();
    std::allocator_traits<NodeAlloc>::destroy(node_alloc, dummy);
    std::allocator_traits<NodeAlloc>::deallocate(node_alloc, dummy, 1);
}

template<typename T, typename Alloc>
void LList<T, Alloc>::clear() {
    for(Node* node = dummy->next; node != dummy;) {
        Node* temp = node->next;
        std::allocator_traits<NodeAlloc>::destroy(node_alloc, node);
        std::allocator_traits<NodeAlloc>::deallocate(node_alloc, node, 1);
        node = temp;
    }
    dummy->next = dummy;    // 不要忘了哨兵节点重新指向自己！！！
    dummy->prev = dummy;
    size_ = 0;
}

template<typename T, typename Alloc>
template<typename... Args>
typename LList<T, Alloc>::Node* LList<T, Alloc>::create_node(Args&&... args) {
    Node* newNode = std::allocator_traits<NodeAlloc>::allocate(node_alloc, 1);
    try {
        std::allocator_traits<NodeAlloc>::construct(node_alloc, newNode, std::forward<Args>(args)...);
    }
    catch(...) {
        std::allocator_traits<NodeAlloc>::deallocate(node_alloc, newNode, 1);
        throw;
    }
    return newNode;
}


template<typename T, typename Alloc>
void LList<T, Alloc>::push_back(const T& e) {
    Node* newNode = create_node(e);

    dummy->prev->next = newNode;
    newNode->prev = dummy->prev;
    newNode->next = dummy;
    dummy->prev = newNode;
    size_++;
}

template<typename T, typename Alloc>
void LList<T, Alloc>::push_back(T&& e) {
    
    Node* newNode = create_node(std::move(e));

    dummy->prev->next = newNode;
    newNode->prev = dummy->prev;
    newNode->next = dummy;
    dummy->prev = newNode;
    size_++;
}

template<typename T, typename Alloc>
template<typename... Args>
void LList<T, Alloc>::emplace_back(Args&&... args) {
    Node* newNode = create_node(std::forward<Args>(args)...);

    dummy->prev->next = newNode;
    newNode->prev = dummy->prev;
    newNode->next = dummy;
    dummy->prev = newNode;
    size_++;
}

template<typename T, typename Alloc>
void LList<T, Alloc>::push_front(const T& e) {
    Node* newNode = create_node(e);

    newNode->next = dummy->next;
    dummy->next->prev = newNode;
    dummy->next = newNode;
    newNode->prev = dummy;
    size_++;
}

template<typename T, typename Alloc>
void LList<T, Alloc>::push_front(T&& e) {
    Node* newNode = create_node(std::move(e));

    newNode->next = dummy->next;
    dummy->next->prev = newNode;
    dummy->next = newNode;
    newNode->prev = dummy;
    size_++;
}

template<typename T, typename Alloc>
template<typename... Args>
void LList<T, Alloc>::emplace_front(Args&&... args) {

    Node* newNode = create_node(std::forward<Args>(args)...);

    newNode->next = dummy->next;
    dummy->next->prev = newNode;
    dummy->next = newNode;
    newNode->prev = dummy;
    size_++;
}

template<typename T, typename Alloc>
typename LList<T, Alloc>::iterator LList<T, Alloc>::insert(const_iterator pos, const T& e) {
    
    Node* newNode = create_node(e);

    Node* p = const_cast<Node*>(pos.node);
    p->prev->next = newNode;
    newNode->prev = p->prev;
    newNode->next = p;
    p->prev = newNode;
    ++size_;
    return ListIterator(newNode);
}

template<typename T, typename Alloc>
typename LList<T, Alloc>::iterator LList<T, Alloc>::insert(const_iterator pos, T&& e) {
    
    Node* newNode = create_node(std::move(e));

    Node* p = const_cast<Node*>(pos.node);
    p->prev->next = newNode;
    newNode->prev = p->prev;
    newNode->next = p;
    p->prev = newNode;
    ++size_;
    return ListIterator(newNode);
}

template<typename T, typename Alloc>
template<typename InputIt>
typename LList<T, Alloc>::iterator LList<T, Alloc>::insert(const_iterator pos, InputIt first, InputIt last) {
    
    Node* newHead = nullptr;
    Node* newTail = nullptr;
    InputIt node_iter = first;
    size_type new_size_ = 0;
    while(node_iter != last) {
        Node* newNode = nullptr;
        try {
            newNode = std::allocator_traits<NodeAlloc>::allocate(node_alloc, 1);
            std::allocator_traits<NodeAlloc>::construct(node_alloc, newNode, *node_iter);
        }
        catch(...) {
            // 1.本次出现创建Node异常，先把之前创建的Node全部释放掉
            Node* node = newHead;
            while(node != nullptr) {
                Node* temp = node->next;
                std::allocator_traits<NodeAlloc>::destroy(node_alloc, node);
                std::allocator_traits<NodeAlloc>::deallocate(node_alloc, node, 1);
                node = temp;
            }
            // 2.如果本次是construct异常，需要多释放一次，否则不需要
            if(newNode) std::allocator_traits<NodeAlloc>::deallocate(node_alloc, newNode, 1);
            throw;
        }

        if(!newTail)
            newHead = newNode;
        else {
            newTail->next = newNode;
            newNode->prev = newTail;
        }
        newTail = newNode;
        ++new_size_;
        ++node_iter;
    }

    Node* p = const_cast<Node*>(pos.node);
    p->prev->next = newHead;
    newHead->prev = p->prev;
    newTail->next = p;
    p->prev = newTail;

    size_ += new_size_;
    return ListIterator(newHead);
}

template<typename T, typename Alloc>
template<typename... Args>
typename LList<T, Alloc>::iterator LList<T, Alloc>::emplace(const_iterator pos, Args&&... args) {

    Node* newNode = create_node(std::forward<Args>(args)...);

    Node* p = const_cast<Node*>(pos.node);
    p->prev->next = newNode;
    newNode->prev = p->prev;
    newNode->next = p;
    p->prev = newNode;
    ++size_;
    return ListIterator(newNode);
}

template<typename T, typename Alloc>
void LList<T, Alloc>::pop_back() {
    
    Node* delNode = dummy->prev;
    delNode->prev->next = dummy;
    dummy->prev = delNode->prev;
    --size_;

    std::allocator_traits<NodeAlloc>::destroy(node_alloc, delNode);
    std::allocator_traits<NodeAlloc>::deallocate(node_alloc, delNode, 1);
}

template<typename T, typename Alloc>
void LList<T, Alloc>::pop_front() {
    
    Node* delNode = dummy->next;
    dummy->next = delNode->next;
    delNode->next->prev = dummy;
    --size_;

    std::allocator_traits<NodeAlloc>::destroy(node_alloc, delNode);
    std::allocator_traits<NodeAlloc>::deallocate(node_alloc, delNode, 1);
}

template<typename T, typename Alloc>
typename LList<T, Alloc>::iterator LList<T, Alloc>::erase(const_iterator pos) {

    Node* delNode = const_cast<Node*>(pos.node);
    delNode->prev->next = delNode->next;
    delNode->next->prev = delNode->prev;
    --size_;
    iterator it = ListIterator(delNode->next);

    std::allocator_traits<NodeAlloc>::destroy(node_alloc, delNode);
    std::allocator_traits<NodeAlloc>::deallocate(node_alloc, delNode, 1);
    return it;
}

template<typename T, typename Alloc>
typename LList<T, Alloc>::iterator LList<T, Alloc>::erase(const_iterator first, const_iterator last) {
    
    Node* firstNode = const_cast<Node*>(first.node);
    Node* lastNode = const_cast<Node*>(last.node);
    Node* prevNode = firstNode->prev;

    Node* delNode = firstNode;
    size_type new_size_ = size_;
    while(delNode != lastNode) {
        Node* nextNode = delNode->next;
        std::allocator_traits<NodeAlloc>::destroy(node_alloc, delNode);
        std::allocator_traits<NodeAlloc>::deallocate(node_alloc, delNode, 1);       
        delNode = nextNode;
        --new_size_;
    }

    prevNode->next = lastNode;
    lastNode->prev = prevNode;
    size_ = new_size_;

    return ListIterator(lastNode);
}

template<typename T, typename Alloc>
void LList<T, Alloc>::remove(const T& e) {

    const_iterator it = cbegin();
    const_iterator end_iter = cend();
    while(it != end_iter) {
        if(*it == e)
            it = erase(it);
        else
            ++it;
    }
}

template<typename T, typename Alloc>
template<typename Predicate>
void LList<T, Alloc>::remove_if(Predicate pred) {
    const_iterator it = cbegin();
    const_iterator end_iter = cend();
    while(it != end_iter) {
        if(pred(*it))
            it = erase(it);
        else
            ++it;
    }
}



























}