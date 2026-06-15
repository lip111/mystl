#include "myMemoryPool.h"
#include <iostream>
#include <cassert>

class TestMemoryPoolObj {
public:
    // string 字符串长度小于等于15,不包括结束符，SSO优化，不向堆申请内存
    TestMemoryPoolObj() { s = std::string(16, 'a'); std::cout << "TestMemoryPoolObj ctor" << std::endl; }
    ~TestMemoryPoolObj() { std::cout << "TestMemoryPoolObj dtor" << std::endl; }
private:
    std::string s;
};

void test_memorypool()
{
    mystl::FixedSizeMemoryPool mempool(30, 3);
    void* p = mempool.allocate(1); 
    // std::cout << p << std::endl;

    void* p2 = mempool.allocate(1);
    // std::cout << p2 << std::endl;

    void* p3 = mempool.allocate(1);
    // std::cout << p3 << std::endl;

    void* p4 = mempool.allocate(1); // 另起一页

    mempool.deallocate(p); // 还到当前页还是所属页？

    mempool.construct(static_cast<TestMemoryPoolObj*>(p2));
    mempool.destroy(static_cast<TestMemoryPoolObj*>(p2));

    // std::cout << mempool.get_free_list() << std::endl;
    // std::cout << mempool.get_page_list() << std::endl;
}