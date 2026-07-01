#include "myrbtree.h"
#include <initializer_list>
#include <stdexcept>

namespace mystl {

template<typename Key, typename T>
struct Select1st {
    const Key& operator()(const std::pair<const Key, T>& value) const {
        return value.first;
    }
};

template<typename Key, typename T, typename Compare = std::less<Key>>
class LMap {
public:
    // 类型别名
    using value_type = std::pair<const Key, T>;
    using key_type = Key;
    using size_type = std::size_t;
    using iterator = typename LRBTree<Key,std::pair<const Key,T>,Select1st<Key,T>,Compare>::iterator;
    using const_iterator = typename LRBTree<Key,std::pair<const Key,T>,Select1st<Key,T>,Compare>::const_iterator;


    // 6大函数
    LMap() {}
    LMap(const LMap& m) = delete;
    LMap& operator=(const LMap& m) = delete;
    LMap(LMap&& m) = delete;
    LMap& operator=(LMap&& m) = delete;
    ~LMap() {}
    explicit LMap(Compare comp): tree_(comp) {}

    // 获取容量
    bool empty() { return tree_.empty(); }
    size_type size() { return tree_.size(); }

    // 访问迭代器
    iterator begin() { return tree_.begin(); }
    const_iterator begin() const { return tree_.begin(); }
    iterator end() { return tree_.end(); }
    const_iterator end() const { return tree_.end(); }

    // 元素访问
    T& operator[](const key_type& key) {
        auto [it, _] = tree_.insert_unique(value_type(key, T()));
        return it->second;
    }

    T& at(const key_type& key) {
        auto it = tree_.find(key);
        if (it == end())
            throw std::out_of_range("map::at value not exist");
        else
            return it->second;
    }
    const T& at(const key_type& key) const {
        auto it = tree_.find(key);
        if (it == end())
            throw std::out_of_range("map::at value not exist");
        else
            return it->second;        
    }
    iterator find(const key_type& key) { return tree_.find(key); }
    const_iterator find(const key_type& key) const { tree_.find(key); }

    // 增加元素
    std::pair<iterator,bool> insert(const value_type& value) { return tree_.insert_unique(value); }

    // 删除元素
    void erase(iterator pos) { return tree_.erase(pos); }
    size_type erase(const key_type& key) { return tree_.erase(key); }
    void clear() { tree_.clear(); }

    // 列表初始化构造函数
    LMap(std::initializer_list<value_type> ls) {
        for(const auto& e: ls) {
            insert(e);
        }
    }

private:
    LRBTree<Key,std::pair<const Key,T>,Select1st<Key,T>,Compare> tree_; // 注意这里pair内部Key为const！！！
};
}