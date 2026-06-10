#include "myallocator.h"
#include <iostream>
#include <cassert>

struct A {
    int a;
    double b;

    A():a(0),b(0.0) { std::cout << "default ctor" << std::endl; }
    A(int _a, double _b):a(_a),b(_b) { std::cout << "ctor" << std::endl; }
    ~A() {std::cout << "dtor" << std::endl; }
};

void test_allocator()
{
    mystl::LAllocator<A> alloc;
    A* newspace = alloc.allocate(3);
    
    A* p1 = newspace;
    A* p2 = newspace + 1;
    alloc.construct(p1, 1, 2.0);
    alloc.construct(p2);

    // std::cout << p1->a << ", " << p1->b << std::endl;
    // std::cout << p2->a << ", " << p2->b << std::endl;
    assert(p1->a == 1);
    assert(2.0 == p1->b);
    assert(0 == p2->a);
    assert(0.0 == p2->b);


    alloc.destroy(p1);
    alloc.destroy(p2);

    alloc.deallocate(newspace, 3);
}

