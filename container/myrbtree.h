#pragma once
#include <functional>
//下面的测试用
#include <iostream>
#include <queue>

namespace mystl {

template<typename Key, typename Value, typename KeyOfValue, typename Compare = std::less<Key>>
class LRBTree {
public:
    template<typename Ptr, typename Ref>
    struct LRBTreeIterator;

    using size_type = std::size_t;
    using iterator = LRBTreeIterator<Value*, Value&>;
    using const_iterator = LRBTreeIterator<const Value*, const Value&>;

    // 6大函数
    LRBTree(): dummy(new Node()), size_(0) { root = dummy; }
    LRBTree(const LRBTree& tree) = delete;
    LRBTree& operator=(const LRBTree& tree) = delete;
    LRBTree(LRBTree&& tree) noexcept = delete;
    LRBTree& operator=(LRBTree&& tree) noexcept = delete;
    ~LRBTree() {
        clear();
        delete dummy;
    }

    // 获取容量
    size_type size() const { return size_; }
    bool empty() const { return size_ == 0; }

    // 获取迭代器
    iterator begin() { return iterator(dummy->left, dummy); }
    const_iterator begin() const { return const_iterator(dummy->left, dummy); }
    iterator end() { return iterator(dummy, dummy); }
    const_iterator end() const { return const_iterator(dummy, dummy); }

    // 访问元素
    iterator find(const Key& key);
    const_iterator find(const Key& key) const { return find(key); }
    iterator lower_bound(const Key& key);
    iterator upper_bound(const Key& key);

    // 增加元素
    std::pair<iterator, bool> insert_unique(const Value& val);

    // 删除元素
    void erase(const_iterator pos);
    size_type erase(const Key& key) {
        if (auto it = find(key); it != end()) {
            erase(it);
            return 1;
        }
        return 0;
    }

    void clear() {
        destroy_tree(root);
        root = dummy;
        size_ = 0;
    }

    // 打印红黑树
    void print() {
        print_tree(root);
    };


private:

    enum Color { RED = 0, BLACK = 1 };

    struct Node {
        Node* parent;
        Node* left;
        Node* right;
        Color color;
        Value value;

        Node(): value(Value{}), color(BLACK), parent(this), left(this), right(this) {}
        Node(const Value& v): value(v), color(RED), parent(nullptr), left(nullptr), right(nullptr) {}
    };  // 这里分号不能少！！！

    Node* dummy;
    Node* root;
    size_type size_;
    Compare comp_;

    // 辅助函数
    Node* maximum(Node* node) const //以node为根的树的最大节点
    {
        while (node->right != dummy)
            node = node->right;
        return node;
    }

    Node* minimum(Node* node) const {
        while (node->left != dummy)
            node = node->left;
        return node;
    }

    // 旋转和修复
    void rotate_left(Node* node);
    void rotate_right(Node* node);
    void insert_fixup(Node* node);
    void erase_fixup(Node* node, Node* parent);
    void transplant(Node* u, Node* v); // v替换u位置

    // 节点管理
    Node* create_node(const Value& val);
    void destroy_node(Node* node) { delete node; }
    void destroy_tree(Node* node) {
        if (node == dummy)
            return;
        destroy_tree(node->left);
        destroy_tree(node->right);
        delete node;
    }
    Node* copy_subtree(Node* src_node, Node* parent);


    // 打印红黑树(方便测试)
    void print_tree(Node* node) {
        if (node == nullptr || node->parent == node) return;
        
        std::queue<Node*> que;
        Node* curr = node;
        que.push(curr);
        while(!que.empty()) {
            int n = que.size();
            while(n--) {
                curr = que.front();
                que.pop();

                // 打印格式：当前节点值(父节点值,是父节点左孩子L/右孩子R,当前节点颜色)
                // 6(10,L,RED)
                if (curr->parent == dummy)
                        std::cout << curr->value << "(" << "NULL" << ","
                        << "NULL" << ","
                        << (curr->color == RED ? "RED":"BLACK") << ")" << "     ";
                else
                    std::cout << curr->value << "(" << curr->parent->value << ","
                        << (curr->parent->left == curr ? "L":"R") << ","
                        << (curr->color == RED ? "RED":"BLACK") << ")" << "     ";

                if (curr->left != dummy)
                    que.push(curr->left);
                if (curr->right != dummy)
                    que.push(curr->right);   
            }
            std::cout << "" << std::endl;
        }
        std::cout << "" << std::endl;
    }
};

template<typename Key, typename Value, typename KeyOfValue, typename Compare>
template<typename Ptr, typename Ref>
struct LRBTree<Key,Value,KeyOfValue,Compare>::LRBTreeIterator {

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = Value;
    using diffence_type = std::ptrdiff_t;
    using pointer = Ptr;
    using reference = Ref;

    LRBTreeIterator(Node* node, Node* dummy1): node_(node),dummy_(dummy1) {}

    template<typename Ptr2, typename Ref2>
    LRBTreeIterator(const LRBTreeIterator<Ptr2, Ref2>& it): node_(it.node_),dummy_(it.dummy_) {}

    
    bool operator!=(const LRBTreeIterator other) const {
        return node_ != other.node_;
    }

    bool operator==(const LRBTreeIterator other) const {
        return node_ == other.node_;
    }

    pointer operator->() const {
        return node_;
    }

    reference operator*() const {
        return node_->value;
    }

    // 中序后继
    LRBTreeIterator& operator++() {
        this->node_ = increment(node_);
        return *this;
    }

    LRBTreeIterator operator++(int) {
        LRBTreeIterator it = *this;
        ++(*this);
        return it;
    }

    // 中序前驱
    LRBTreeIterator& operator--() { 
        this->node_ = decrement(node_);
        return *this;
    }

    LRBTreeIterator operator--(int) {
        LRBTreeIterator it = *this;
        --(*this);
        return it;
    }


private:
    Node* node_;
    Node* dummy_;
    friend class LRBTree;
    
    Node* increment(Node* node) {
        if (node->right != dummy_) {
            Node* p = node->right;
            while(p->left != dummy_) {
                p = p->left;
            }
            return p;
        }
        else {
            Node* p = node;
            while(p->parent != dummy_ && p->parent->left != p) {
                p = p->parent;
            }
            return p->parent;
        }
    }
    Node* decrement(Node* node) {
        if (node == dummy_) // 迭代器为end()的时候特殊处理！！！
            return dummy_->right;

        if (node->left != dummy_) {
            Node* p = node->left;
            while(p->right != dummy_) {
                p = p->right;
            }
            return p;
        }
        else {
            Node* p = node;
            while(p->parent != dummy_ && p->parent->right != p) {
                p = p->parent;
            }
            return p->parent;
        }
    }
};

template<typename Key, typename Value, typename KeyOfValue, typename Compare>
typename LRBTree<Key,Value,KeyOfValue,Compare>::iterator 
    LRBTree<Key,Value,KeyOfValue,Compare>::find(const Key& key) {
    KeyOfValue kov;
    Node* curr = root;
    while(curr != dummy) {
        Key k = kov(curr->value);
        if(!comp_(k,key) && !comp_(key,k))
            break;
        else if(comp_(key,k))
            curr = curr->left;
        else
            curr = curr->right;
    }

    return iterator(curr,dummy);
}


template<typename Key, typename Value, typename KeyOfValue, typename Compare>
void LRBTree<Key,Value,KeyOfValue,Compare>::rotate_left(typename LRBTree::Node* node) {

    Node* parent = node;
    Node* kid = parent->right;
    
    parent->right = kid->left;
    if(kid->left != dummy)
        kid->left->parent = parent;
    
    kid->left = parent; // 左旋
    if (parent->parent == dummy) {
        kid->parent = dummy;
        root = kid; // 爷爷不存在，当前孩子节点需要更新为root！！！
    }
    else {
        if (parent->parent->left == parent)
            parent->parent->left = kid;
        else
            parent->parent->right = kid;
        kid->parent = parent->parent;
    }
    parent->parent = kid;
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare>
void LRBTree<Key,Value,KeyOfValue,Compare>::rotate_right(typename LRBTree::Node* node) {

    Node* parent = node;
    Node* kid = parent->left;

    parent->left = kid->right;
    if (kid->right != dummy)
        kid->right->parent = parent;

    kid->right = parent;    // 右旋
    if (parent->parent == dummy) {
        kid->parent = dummy;
        root = kid; // 爷爷不存在，当前孩子节点需要更新为root！！！
    }
    else {
        if(parent->parent->left == parent)
            parent->parent->left = kid;
        else
            parent->parent->right = kid;
        kid->parent = parent->parent;
    }
    parent->parent = kid;
}

// 注意类外函数定义的写法！！！
template<typename Key, typename Value, typename KeyOfValue, typename Compare>
typename LRBTree<Key,Value,KeyOfValue,Compare>::Node* 
    LRBTree<Key,Value,KeyOfValue,Compare>::create_node(const Value& val) {
    
    Node* newNode = new Node(val);
    newNode->parent = dummy;
    newNode->left = dummy;
    newNode->right = dummy;
    return newNode;
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare>
void LRBTree<Key,Value,KeyOfValue,Compare>::insert_fixup(Node* node) {

    Node* parent = node->parent;
    Node* uncle = dummy;
    while(node != root && parent->color == RED) {
        
        if (parent->parent->left == parent)
            uncle = parent->parent->right;
        else
            uncle = parent->parent->left;

        // 3.父红，叔黑(LL/LR/RL/RR)
        if (parent->color == RED && uncle->color == BLACK) {
            if (parent->parent->left == parent && parent->left == node) {
                rotate_right(parent->parent);
                parent->color = BLACK;
                parent->right->color = RED;
            }
            else if(parent->parent->left == parent && parent->right == node) {
                rotate_left(parent);
                rotate_right(node->parent);
                node->color = BLACK;
                node->right->color = RED;
            }
            else if(parent->parent->right == parent && parent->left == node) {
                rotate_right(parent);
                rotate_left(node->parent);
                node->color = BLACK;
                node->left->color = RED;
            }
            else if(parent->parent->right == parent && parent->right == node) {
                rotate_left(parent->parent);
                parent->color = BLACK;
                parent->left->color = RED;
            }
            return;
        }

        // 4.父红，叔红
        if (parent->color == RED && uncle->color == RED) {
            parent->color = BLACK;
            uncle->color = BLACK;
            parent->parent->color = RED;
            node = parent->parent;
            parent = node->parent;

        }
    }

    if (node == root)
        node->color = BLACK;
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare>
std::pair<typename LRBTree<Key,Value,KeyOfValue,Compare>::iterator, bool> 
    LRBTree<Key,Value,KeyOfValue,Compare>::insert_unique(const Value& val) {
    
    KeyOfValue kov;
    Node* newNode = create_node(val);

    // 找插入位置
    Node* parent = dummy;
    Node* curr = root;
    Key target_key = kov(newNode->value);  
    while(curr != dummy) {
        Key curr_key = kov(curr->value);
        if (!comp_(target_key, curr_key) && !comp_(curr_key, target_key))
            return {iterator(curr,dummy), false};

        parent = curr;
        if (comp_(target_key, curr_key))
            curr = curr->left;
        else if (comp_(curr_key, target_key))
            curr = curr->right;
    }

    // 插入
    if (parent == dummy) {
        root = newNode;
    }
    else {
        Key k = kov(parent->value);
        if (comp_(target_key, k)) {
            parent->left = newNode;
            newNode->parent = parent;
        }
        else {
            parent->right = newNode;
            newNode->parent = parent;
        }
    }

    // 1.插入根节点
    if (root == newNode) {
        root->color = BLACK;
    }

    // 2.插入节点父节点是黑色(无需调整，直接返回)
    else if (newNode->parent->color == BLACK) {
    }

    // 调整
    else {
        insert_fixup(newNode);
    }
    
    if (newNode == root) {
        dummy->left = newNode;
        dummy->right = newNode;
    }
    else if (comp_(newNode->value, dummy->left->value))
        dummy->left = newNode;
    else if (comp_(dummy->right->value, newNode->value))
        dummy->right = newNode;

    ++size_;
    return {iterator(newNode,dummy), true};
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare>
void LRBTree<Key,Value,KeyOfValue,Compare>::transplant(Node* u, Node* v) {

    if (u->parent == dummy) {
        v->parent = dummy;  // 不能漏了,被替换节点是根节点，也要更新v的parent,否则parent还是指向u,而u即将被释放！！！
        root = v;
    }
        
    else {
        if (v != dummy) // 当v是哨兵，不能把parent指向u的parent！！！
            v->parent = u->parent;

        if (u->parent->left == u)
            u->parent->left = v;
        else
            u->parent->right = v;
    }
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare>
void LRBTree<Key,Value,KeyOfValue,Compare>::erase_fixup(Node* node, Node* parent) {

    while(node->color == BLACK && node != root) {

        Node* brother = dummy;
        // 不能写成parent->left != dummy
        // 虽然刚进入修复函数肯定是双黑哨兵，但是修复过程可能向上调整，这时双黑就不是哨兵，而是实际节点！！！
        if (parent->left != node) { 
            brother = parent->left;
        }
        else {
            brother = parent->right;
        }

        if (brother->color == BLACK) {
            
            Node* l = brother->left;
            Node* r = brother->right; 
            // 3.兄弟黑色，且至少一个红孩
            // if (l != dummy || r != dummy)
            // l != dummy并不能表示就是红孩,需要使用l->color == RED,虽然第一次进入调整确实如此,但是有可能双黑上移调整，
            // 这时候会产生兄弟节点的孩子是真实黑色节点的情况！！！
            if (l->color == RED || r->color == RED) {

                // if (brother->parent->left == brother && l != dummy) { // LL
                if (brother->parent->left == brother && l->color == RED) { 
                    l->color = brother->color;
                    brother->color = brother->parent->color;
                    brother->parent->color = BLACK;
                    rotate_right(brother->parent);
                }

                // else if (brother->parent->left == brother && l == dummy) { // LR
                else if (brother->parent->left == brother && l->color != RED) {
                    r->color = brother->parent->color;
                    brother->parent->color = BLACK; // LR也需要父节点变黑！！！
                    rotate_left(brother);
                    rotate_right(r->parent);
                }

                // else if (brother->parent->right == brother && r == dummy) { // RL
                else if (brother->parent->right == brother && r->color != RED) {
                    l->color = brother->parent->color;
                    brother->parent->color = BLACK; // RL也需要父节点变黑！！！
                    rotate_right(brother);
                    rotate_left(l->parent);
                }

                // else if (brother->parent->right == brother && r != dummy) { // RR
                else if (brother->parent->right == brother && r->color == RED) {
                    r->color = brother->color;
                    brother->color = brother->parent->color;
                    brother->parent->color = BLACK;
                    rotate_left(brother->parent);          
                }
                break;
            }

            // 4.兄弟黑色，只有黑色孩子
            else {
                brother->color = RED;
                node = parent;
                parent = parent->parent;
            }
        }

        // 5.兄弟红色
        else {
            brother->color = BLACK;
            parent->color = RED;
            if (parent->left == brother)
                rotate_right(parent);
            else 
                rotate_left(parent);
        }
    }
    // 双黑节点上移到红色节点或者根节点
    node->color = BLACK;
};

template<typename Key, typename Value, typename KeyOfValue, typename Compare>
void LRBTree<Key,Value,KeyOfValue,Compare>::erase(const_iterator pos) {
    
    // 如果删除节点的左右孩子都有，找后继，交换值后删除后继
    Node* node = pos.node_;
    if (node->left != dummy && node->right != dummy) {
        Node* successor = minimum(node->right);
        node->value = successor->value;
        node = successor;
    }

    // 替换
    Node* x = dummy;
    if (node->left != dummy) {
        x = node->left;
        transplant(node, x);
    }
    else if (node->right != dummy) {
        x = node->right;
        transplant(node, x);
    }
    else {
        transplant(node, x);    //当前节点无孩子，交换节点是哨兵，也要进行处理！！！
    }
    Node* parent = node->parent;
    
    // 1.删除节点只有左/右孩子
    if (node->left != dummy || node->right != dummy) {
        x->color = BLACK;
        destroy_node(node);
    }
    // 2.删除节点无孩，红色
    else if (node->color == RED) {
        destroy_node(node);
    }
    else {
        destroy_node(node);
            
        // 进入修复函数后删除节点确定是无孩，黑色节点
        erase_fixup(x, parent);
    }

    --size_;
    if (root != dummy) {
        dummy->left = minimum(root);
        dummy->right = maximum(root);
    }
    else {
        dummy->left = dummy;
        dummy->right = dummy;
    }
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare>
typename LRBTree<Key,Value,KeyOfValue,Compare>::iterator 
LRBTree<Key,Value,KeyOfValue,Compare>::lower_bound(const Key& key) {

    KeyOfValue kov;
    Node* curr = root;
    Node* res = dummy;

    while(curr != dummy) {
        Key k = kov(curr->value);
        if (comp_(k,key)) {
            curr = curr->right;
        }
        else {
            res = curr;
            curr = curr->left;
        }
    }

    return iterator(res, dummy);
}

template<typename Key, typename Value, typename KeyOfValue, typename Compare>
typename LRBTree<Key,Value,KeyOfValue,Compare>::iterator 
LRBTree<Key,Value,KeyOfValue,Compare>::upper_bound(const Key& key) {
    
    KeyOfValue kov;
    Node* curr = root;
    Node* res = dummy;

    while(curr != dummy) {
        Key k = kov(curr->value);
        if (!comp_(key,k)) {
            curr = curr->right;
        }
        else {
            res = curr;
            curr = curr->left;
        }
    }

    return iterator(curr,dummy);
}


}