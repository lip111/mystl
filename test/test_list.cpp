#include "mylist.h"
#include <cassert>
#include <string>
#include <iostream>

// 测试6大函数
void test_list_6functions() {
    // 默认构造
    mystl::LList<int> ls;
    assert(ls.size() == 0);
    
    // 拷贝构造(以及列表初始化构造)
    mystl::LList<std::string> temp{"nishikata", "takagi"};
    mystl::LList<std::string> ls2 = temp;
    assert(ls2.size() == 2);

    // 移动构造
    mystl::LList<std::string> ls3 = std::move(temp);
    assert(ls3.size() == 2);

    // 拷贝赋值
    ls2 = ls2;
    assert(ls2.size() == 2);

    ls2 = ls3;
    assert(ls2.size() == 2);

    // 移动赋值
    ls2 = std::move(ls2);
    assert(ls2.size() == 2);

    ls2 = std::move(ls3);
    assert(ls2.size() == 2);
}

// 测试查询容量
void test_list_capacity() {
    mystl::LList<int> ls = {7,23,4,8};
    assert(4 == ls.size());
    assert(false == ls.empty());
    // std::cout << ls.max_size() << std::endl;
}

// 测试获取迭代器
void test_list_iterator() {
    mystl::LList<int> ls;
    const mystl::LList<int> cls;
    mystl::LList<int> ls2 = {7,12,64,15};

    auto start_iter = ls.begin();
    auto end_iter = ls.end();
    assert(start_iter == end_iter);

    auto start2_iter = cls.begin();
    auto end2_iter = cls.end();
    assert(start2_iter == end2_iter);

    auto cstart_iter = ls.cbegin();
    auto cend_iter = ls.cend();
    assert(cstart_iter == cend_iter);

    start_iter = ls2.begin();
    end_iter = ls2.end();
    assert(start_iter != end_iter);

    start_iter++;
    assert(12 == *start_iter);
    start_iter--;
    assert(7 == *start_iter);

    (++start_iter)--;
    assert(7 == *start_iter);
    (--end_iter)++;
    assert(ls2.end() == end_iter);
    
    struct A { int x_; A() = default; A(int x):x_(x) {} };
    mystl::LList<A> ls3 = {A(6)};
    auto start3_iter = ls3.begin();
    assert(6 == start3_iter->x_);
}

// 测试获取元素
void test_list_get_element() {
    mystl::LList<int> ls = {7,12,64,15};
    const mystl::LList<int> cls = {17,22,74,25};
    auto first = ls.front();
    auto first2 = cls.front();
    assert(7 == first);
    assert(17 == first2);

    auto last = ls.back();
    auto last2 = cls.back();
    assert(15 == last);
    assert(25 == last2);
}

// 测试添加元素
void test_list_add_element() {
    // push_back,emplace_back
    mystl::LList<std::string> ls;
    ls.push_back("nishikata");
    assert(1 == ls.size());
    std::string s = "takagi";
    ls.push_back(s);
    assert(2 == ls.size());
    ls.emplace_back("sora");
    assert(3 == ls.size());

    auto it = ls.cbegin();
    assert(*it == "nishikata");
    ++it;
    assert(*it == "takagi");
    ++it;
    assert(*it == "sora");

    // push_front, emplace_front
    ls.push_front("kirei");
    assert(4 == ls.size());
    std::string s2 = "kirai";
    ls.push_front(s2);
    assert(5 == ls.size());
    ls.emplace_front("karai");
    assert(6 == ls.size());

    it = ls.cbegin();
    assert(*it == "karai");
    ++it;
    assert(*it == "kirai");
    ++it;
    assert(*it == "kirei");

    // insert,emplace
    auto it2 = it;
    auto it3 = ls.emplace(it2, "watashi");
    assert(7 == ls.size());
    assert(*it3 == "watashi");

    auto it4 = ls.insert(it3, "ashita");
    assert(8 == ls.size());
    assert(*it4 == "ashita");

    std::string s3 = "kyou";
    auto it5 = ls.insert(it4, s3);
    assert(9 == ls.size());
    assert(*it5 == "kyou");

    const char* arr[2] = {"suki", "desu"};
    auto it6 = ls.insert(it5, arr, arr+2);
    assert(11 == ls.size());
    assert(*it6 == "suki");
    ++it6;
    assert(*it6 == "desu");
    ++it6;
    assert(*it6 == "kyou");
    ++it6;
    assert(*it6 == "ashita");
    ++it6;
    assert(*it6 == "watashi");
    ++it6;
    assert(*it6 == "kirei"); 
}

// 测试删除元素
void test_list_delete_element() {
    // pop_back,pop_front
    mystl::LList<std::string> ls = {"nishikata", "wa", "takagi", "daisuki", "desu"};
    
    ls.pop_back();
    assert("daisuki" == ls.back());
    assert(4 == ls.size());

    ls.pop_front();
    assert("wa" == ls.front());
    assert(3 == ls.size());

    // erase
    mystl::LList<std::string> ls2 = {"takagi", "wa", "nishikata", "daisuki", "desu"};
    auto it1 = ls2.erase(ls2.cbegin());
    assert(4 == ls2.size());
    assert("wa" == *it1);

    auto first = ++ls2.cbegin();
    auto last = --ls2.cend();
    auto it2 = ls2.erase(first, last);
    assert(2 == ls2.size());
    assert("desu" == *it2);
    assert("wa" == *--it2);

    // remove, remove_if
    mystl::LList<std::string> ls3 = {"watashi", "no", "takagi", "wa", "doko", "desu", "ka"};
    ls3.remove("doko");
    assert(6 == ls3.size());
    ls3.remove_if([](const std::string& s) { if (s == "ka") return true; return false; });
    assert(5 == ls3.size());
}


void test_list() {
    test_list_6functions();
    test_list_capacity();
    test_list_iterator();
    test_list_get_element();
    test_list_add_element();
    test_list_delete_element();
}