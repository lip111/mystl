// 统一测试入口，测试vector、list等分别放test_vector.cpp、test_list.cpp...
#include <iostream>
using namespace std;

extern void test_allocator();
extern void test_vector();
extern void test_memorypool();

int main() {

    cout << "wonderful begin!" << endl;
    // test_allocator();
    test_vector();
    test_memorypool();
    return 0;
}