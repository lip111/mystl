#include <cassert>
#include <iostream>
#include "mydeque.h"


void  test_deque_6functions() {
    // 默认构造
    mystl::LDeque<std::string> dq1;

    // 拷贝构造
    mystl::LDeque<std::string> dq2 = {"kirei", "kirai", "karai"};
    mystl::LDeque<std::string> dq3 = dq2;

    // 拷贝赋值
    dq3 = dq3;
    dq3 = dq2;

    // 移动构造
    mystl::LDeque<std::string> dq4 = {"nishikata", "desu"};
    mystl::LDeque<std::string> dq5 = std::move(dq4);

    // 移动赋值
    dq5 = std::move(dq5);
    dq5 = std::move(dq4);
}

void  test_deque_capacity() {
    mystl::LDeque<std::string> dq1 = {"watashi", "wa", "watermelon"};
    assert(!dq1.empty());
    assert(3 == dq1.size());
}

void  test_deque_iterator() {
    // begin,end
    mystl::LDeque<std::string> dq1 = {"my", "kettle", "boil"};
    dq1.begin();
    dq1.end();
    const mystl::LDeque<std::string> dq2 = {"my", "kettle", "boil"};
    dq2.begin();
    dq2.end();

    // cbegin, cend
    mystl::LDeque<std::string> dq3 = {"my", "kettle", "boil"};
    dq3.cbegin();
    dq3.cend();
}

void  test_deque_get_element() {
    // [],at
    mystl::LDeque<int> dq1 = {0,1,2,3,4,5,6,7,8,9};
    assert(4 == dq1[4]);
    assert(5 == dq1.at(5));
    try { dq1.at(11); } catch(std::out_of_range& e) { assert(std::string(e.what()) == "deque index of out range"); }

    const mystl::LDeque<int> dq2 = {10,11,12,13,14,15,16,17,18,19};
    assert(14 == dq2[4]);
    assert(15 == dq2.at(5));
    try { dq2.at(11); } catch(std::out_of_range& e) { assert(std::string(e.what()) == "deque index of out range"); }

    // front,back
    mystl::LDeque<int> dq3 = {20,21,22,23,24,25,26,27,28,29};
    assert(20 == dq3.front());
    assert(29 == dq3.back());

    const mystl::LDeque<int> dq4 = {30,31,32,33,34,35,36,37,38,39};
    assert(30 == dq4.front());
    assert(39 == dq4.back());
}

void  test_deque_add_element() {

    // push_back,emplace_back
    mystl::LDeque<int> dq1 = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30};
    dq1.push_back(31);  // 触发扩容 3—>6
    assert(32 == dq1.size());
    assert(31 == dq1.back());
    int a = 32;
    dq1.push_back(a);
    assert(33 == dq1.size());
    assert(32 == dq1.back());
    dq1.emplace_back(33);
    assert(34 == dq1.size());
    assert(33 == dq1.back());

    // push_front,emplace_front
    mystl::LDeque<int> dq2 = {0};
    dq2.push_front(-1);
    assert(2 == dq2.size());
    assert(-1 == dq2.front());
    int b = -2;
    dq2.push_front(b);
    assert(3 == dq2.size());
    assert(-2 == dq2.front());
    dq2.emplace_front(-3);
    assert(4 == dq2.size());
    assert(-3 == dq2.front());
    for(int i = -4; i >= -16; --i) dq2.emplace_front(i);
    dq2.emplace_front(-17); // 触发扩容 3->6
    assert(18 == dq2.size());
    assert(-17 == dq2.front());

    // insert,emplace
    mystl::LDeque<int> dq3 = {0,1,2,3,4,5,6,7,8,9};
    assert(10 == dq3.size());
    dq3.insert(dq3.begin()+3, 55);
    assert(11 == dq3.size());
    assert(55 == dq3[3]);
    int c = 26;
    dq3.insert(dq3.begin()+3, c);
    assert(12 == dq3.size());
    assert(26 == dq3[3]);
    dq3.emplace(dq3.begin()+8, 90);
    assert(13 == dq3.size());
    assert(90 == dq3[8]);

    dq3.insert(dq3.begin(), 245);
    assert(14 == dq3.size());
    assert(245 == dq3[0]);
    dq3.insert(dq3.end(), 547);
    assert(15 == dq3.size());
    assert(547 == dq3.back());

    // 区间insert
    mystl::LDeque<int> dq4 = {0,1,2};
    int arr[17] = {11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27};
    dq4.insert(dq4.begin()+1, arr, arr+17);
    assert(20 == dq4.size());
    assert(11 == dq4[1]);
    assert(27 == dq4[17]);

}

void  test_deque_delete_element() {
    // pop_back,pop_front
    mystl::LDeque<int> dq1 = {0,1,2,3,4,5,6,7,8,9};
    assert(10 == dq1.size());
    dq1.pop_back();
    assert(9 == dq1.size());
    assert(8 == dq1.back());
    dq1.pop_front();
    assert(8 == dq1.size());
    assert(1 == dq1.front()); 

    // erase
    mystl::LDeque<int> dq2 = {0,1,2,3,4,5,6,7,8,9};
    dq2.erase(dq2.begin());
    assert(9 == dq2.size());
    assert(1 == dq2.front());

    dq2.erase(dq2.begin(),dq2.begin()+3);
    assert(6 == dq2.size());
    assert(4 == dq2.front());

    //clear
    mystl::LDeque<int> dq3 = {0,1,2,3,4,5,6,7,8,9};
    assert(10 == dq3.size());
    dq3.clear();
    assert(0 == dq3.size());

}


void test_deque() {
    test_deque_6functions();
    test_deque_capacity();
    test_deque_iterator();
    test_deque_get_element();
    test_deque_add_element();
    test_deque_delete_element();
}