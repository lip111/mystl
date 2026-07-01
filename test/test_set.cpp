#include "myset.h"
#include <cassert>

void test_set() {
    mystl::LSet<int> lset = {5,23,32,124,31,7};
    assert(6 == lset.size());

    auto it = lset.begin();
    assert(5 == *it);
    auto it2 = lset.end();
    assert(124 == *--it2);

    lset.erase(5);
    lset.erase(124);
    assert(4 == lset.size());
    
    auto it3 = lset.begin();
    assert(7 == *it3);
    auto it4 = lset.end();
    assert(32 == *--it4);

    auto [it5, res] = lset.insert(31);
    assert(false == res);
}
