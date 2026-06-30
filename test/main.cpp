// 统一测试入口，测试vector、list等分别放test_vector.cpp、test_list.cpp...
#include <iostream>
using namespace std;

extern void test_allocator();
extern void test_vector();
extern void test_memorypool();
extern void test_list();
extern void test_deque();
extern void test_rbtree();
extern void test_stack();
extern void test_queue();

int main() {

    cout << "wonderful begin!" << endl;
    // test_allocator();
    test_vector();
    test_memorypool();
    test_list();
    test_deque();
    test_rbtree();
    test_stack();
    test_queue();
    return 0;
}