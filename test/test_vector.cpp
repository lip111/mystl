#include <cassert>
#include <iostream>

#include "myallocator.h"
#include "myvector.h"

// 测试扩容函数
void test_expand_capacity() {
    mystl::LVector<std::string> lv{"nishikata", "takagi", "sora"};
    assert(lv.size() == 3);
    assert(lv.capacity() == 3);

    // reserve扩容
    lv.reserve(6);
    assert(lv.size() == 3);
    assert(lv.capacity() == 6);

    // reserve缩小
    lv.reserve(2);
    assert(lv.size() == 3);
    assert(lv.capacity() == 6);

    // resize扩充size非扩容
    lv.resize(4);
    assert(lv.size() == 4);
    assert(lv.capacity() >= 6);
    assert(lv[3] == "");

    // resize扩充size,自动扩容
    lv.resize(8, "nishikata");
    assert(lv.size() == 8);
    assert(lv.capacity() >= 8);
    assert(lv[7] == "nishikata");

    // resize缩小size
    lv.resize(2);
    assert(lv.size() == 2);
    assert(lv.capacity() >= 7);

    lv.resize(0);
    assert(lv.size() == 0);
    assert(lv.capacity() >= 7);
}

// 测试6大函数：默认\拷贝\移动构造，拷贝\移动赋值、析构
void test_6functions() {
    // 默认构造
    mystl::LVector<std::string> lv1;
    assert(lv1.empty());

    // 拷贝构造
    mystl::LVector<int> lv2 = {3,4};
    mystl::LVector<int> lv3 = lv2;
    assert(lv2[0] == 3);
    assert(lv2[1] == 4);
    assert(lv3[0] == 3);
    assert(lv3[1] == 4);
    
    // 拷贝赋值
    mystl::LVector<int> lv4;
    lv4 = lv3;
    assert(lv4[0] == 3);
    assert(lv4[1] == 4);
    
    // 移动构造
    mystl::LVector<int> lv5{6,8};
    mystl::LVector<int> lv6 = std::move(lv5);
    assert(lv5.empty());
    assert(lv6.capacity() == 2);
    assert(lv6[0] == 6);
    assert(lv6[1] == 8);

    // 移动赋值
    mystl::LVector<int> lv7;
    lv7 = std::move(lv6);
    assert(lv6.empty());
    assert(lv7.capacity() == 2);
    assert(lv7[0] == 6);
    assert(lv7[1] == 8);
}

// 测试添加元素
void test_add_element() {
    // push_back
    mystl::LVector<int> lv;
    lv.push_back(1);
    lv[0] = 1;
    assert(lv.size()==1);
    assert(lv.capacity()==1);
    
    lv.push_back(3);
    assert(lv.size()==2);
    assert(lv.capacity() == 2);

    mystl::LVector<std::string> lv2;
    std::string s = "nishikata";
    lv2.push_back(std::move(s));
    assert(s.empty());

    // emplace_back
    mystl::LVector<std::string> lv3;
    lv3.emplace_back("takagi");
    assert(lv3.size() == 1);
    assert(lv3.capacity() == 1);

    // insert
    mystl::LVector<std::string> lv4;
    auto it = lv4.insert(lv4.begin(), "watashi");
    assert(*it == "watashi");
    assert(lv4[0] == "watashi");

    std::string s2 = "onigiri";
    it = lv4.insert(lv4.end(), std::move(s2));
    assert(*it == "onigiri");
    assert(lv4[1] == "onigiri");

}

// 测试删除元素
void test_delete_element() {
    
    mystl::LVector<int> lv = {1,9,9,7,1,1,1,3};
    //pop_back
    lv.pop_back();
    assert(lv.size() == 7);
    assert(lv[lv.size()-1] == 1);
    
    // erase
    lv.erase(lv.begin());
    assert(lv[0] == 9);

    lv.erase(lv.begin()+3,lv.begin()+6);
    assert(lv.size() == 3);
    assert(lv[0] == 9);
    assert(lv[2] == 7);

    // clear
    lv.clear();
    assert(lv.empty());
}

// 测试访问元素
void test_get_element() {
    
    mystl::LVector<int> lv = {5,2,0,1,3,1,4};
    // front
    auto& e1 = lv.front();
    assert(5 == lv[0]);
    e1 = 6;
    assert(6 == lv[0]);

    // back
    auto& e2 = lv.back();
    assert(4 == lv[6]);
    e2 = 6;
    assert(6 == lv[6]);

    // at
    try {
        lv.at(-1);
    } 
    catch(std::out_of_range& e) {
        assert(std::string(e.what()) == "index out of range");
    }
    auto a = lv.at(1);
    assert(2 == a);
}


void test_vector() {
    test_expand_capacity();
    test_6functions();
    test_add_element();
    test_delete_element();
    test_get_element();
}