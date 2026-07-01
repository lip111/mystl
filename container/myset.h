#include "myrbtree.h"
#include <initializer_list>

namespace mystl {

template<typename T>
struct Identity {
    const T& operator()(const T& value) const {
        return value;
    }
};

template<typename Key, typename Compare=std::less<Key>>
class LSet {
public:
    using value_type = Key;
    using key_type = Key;
    using size_type = std::size_t;
    using iterator = typename LRBTree<Key, Key, Identity<Key>, Compare>::iterator;
    using const_iterator = typename LRBTree<Key, Key, Identity<Key>, Compare>::const_iterator;

    // 6大函数
    LSet() {}
    LSet(const LSet& s) = delete;
    LSet& operator=(const LSet& s) = delete;
    LSet(LSet&& s) = delete;
    LSet& operator=(LSet&& s) = delete;
    ~LSet() {}
    explicit LSet(const Compare& comp): tree_(comp) {}

    // 访问迭代器
    iterator begin() { return tree_.begin(); }
    const_iterator begin() const { return tree_.begin(); }
    iterator end() { return tree_.end(); }
    const_iterator end() const { return tree_.end(); }

    // 获取容量
    bool empty() const { return tree_.empty(); }
    size_type size() const { return tree_.size(); }

    // 元素访问
    iterator find(const Key& key) { return tree_.find(key); }
    const_iterator find(const Key& key) const { return tree_.find(key); }

    // 增加元素
    std::pair<iterator,bool> insert(const value_type& value) { return tree_.insert_unique(value); }

    // 删除元素
    iterator erase(iterator pos) { return tree_.erase(pos); }
    size_type erase(const key_type& key) { return tree_.erase(key); }
    void clear() { return tree_.clear(); }

    // 列表初始化构造函数
    LSet(std::initializer_list<value_type> ls) {
        for(const auto& e: ls) {
            insert(e);
        }
    }

private:
    LRBTree<Key, Key, Identity<Key>, Compare> tree_;
};

}