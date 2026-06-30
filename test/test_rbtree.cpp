#include "myrbtree.h"
#include <queue>
#include <iostream>
#include <cassert>

namespace {
    template<typename T>
    struct Identity {
        const T& operator()(const T& value) const {
            return value;
        }
    };

}



void test_rbtree_insert() {

    mystl::LRBTree<int,int,Identity<int>> tree;
    // 17,18,23,34,27,15,9,6,8,5,25
    tree.insert_unique(17);
    tree.insert_unique(18);
    tree.insert_unique(23);
    tree.insert_unique(34);
    tree.insert_unique(27);
    tree.insert_unique(15);
    tree.insert_unique(9);
    tree.insert_unique(6);
    tree.insert_unique(8);
    tree.insert_unique(5);
    tree.insert_unique(25);
    // tree.print();
}

void test_rbtree_erase() {
    mystl::LRBTree<int,int,Identity<int>> tree;
    // 15,9,18,6,13,17,27,10,23,34,25,37
    tree.insert_unique(15);
    tree.insert_unique(9);
    tree.insert_unique(18);
    tree.insert_unique(6);
    tree.insert_unique(13);
    tree.insert_unique(17);
    tree.insert_unique(27);
    tree.insert_unique(10);
    tree.insert_unique(23);
    tree.insert_unique(34);
    tree.insert_unique(25);
    tree.insert_unique(37);
    // std::cout << "tree to delete" << std::endl;
    // tree.print();

    // 删除顺序：18 25 15 6 13 37 27 17 34 9 10 23
    tree.erase(18);
    // std::cout << "delete 18" << std::endl;
    // tree.print();

    tree.erase(25);
    // std::cout << "delete 25" << std::endl;
    // tree.print();

    tree.erase(15);
    // std::cout << "delete 15" << std::endl;
    // tree.print();

    tree.erase(6);
    // std::cout << "delete 6" << std::endl;
    // tree.print();

    tree.erase(13);
    // std::cout << "delete 13" << std::endl;
    // tree.print();

    tree.erase(37);
    // std::cout << "delete 37" << std::endl;
    // tree.print();

    tree.erase(27);
    // std::cout << "delete 27" << std::endl;
    // tree.print();

    tree.erase(17);
    // std::cout << "delete 17" << std::endl;
    // tree.print();

    tree.erase(34);
    // std::cout << "delete 34" << std::endl;
    // tree.print();

    tree.erase(9);
    // std::cout << "delete 9" << std::endl;
    // tree.print();

    tree.erase(10);
    // std::cout << "delete 10" << std::endl;
    // tree.print();

    tree.erase(23);
    // std::cout << "delete 23" << std::endl;
    // tree.print();
}

void test_rbtree_iterator() {
    mystl::LRBTree<int,int,Identity<int>> tree;
    // 17,18,23,34,27,15,9,6,8,5,25
    tree.insert_unique(17);
    tree.insert_unique(18);
    tree.insert_unique(23);
    tree.insert_unique(34);
    tree.insert_unique(27);
    tree.insert_unique(15);
    tree.insert_unique(9);
    tree.insert_unique(6);
    tree.insert_unique(8);
    tree.insert_unique(5);
    tree.insert_unique(25);
    // tree.print();
    //                                    15(NULL,NULL,BLACK)     
    //                     8(15,L,BLACK)                       18(15,R,BLACK)     
    //          6(8,L,BLACK)     9(8,R,BLACK)     17(18,L,BLACK)            27(18,R,RED)     
    // 5(6,L,RED)                                                23(27,L,BLACK)       34(27,R,BLACK)     
    //                                                                  25(23,R,RED) 

    auto it = tree.begin();
    assert(5 == *it);
    assert(5 == *it++);
    assert(8 == *++it);

    auto it2 = tree.end();
    assert(34 == *--it2);
    assert(34 == *it2--);
    assert(25 == *--it2);

    auto it3 = tree.find(18);
    assert(23 == *++it3);

    auto it4 = tree.find(15);
    assert(9 == *--it4);
}


void test_rbtree() {
    test_rbtree_insert();
    test_rbtree_erase();
    test_rbtree_iterator();
}