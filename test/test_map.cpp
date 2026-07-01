#include "mymap.h"
#include <cassert>
#include <string>
#include <stdexcept>

void test_map() {
    mystl::LMap<int,int> lmap = {{2,3},{44,22},{6,8},{1,2}};
    assert(4 == lmap.size());

    auto it1 = lmap.begin();
    assert(2 == it1->second);

    auto it2 = lmap.end();
    assert(22 == (--it2)->second);

    auto v1 = lmap[6];
    assert(8 == v1);

    auto& v2 = lmap[7];  // key不存在，插入新节点，值为默认值
    assert(0 == v2);

    lmap[7] = 28;
    assert(28 == v2);

    auto v3 = lmap.at(44);
    assert(22 == v3);

    try {
        auto v4 = lmap.at(45);
    } catch(std::out_of_range& e) {
        assert(e.what() == std::string("map::at value not exist"));
    }

    auto it3 = lmap.find(2);
    assert(3 == it3->second);

    auto it4 = lmap.find(99);
    assert(lmap.end() == it4);

    auto [it5,res] = lmap.insert({56,75});
    assert(75 == it5->second);
    assert(true == res);

    auto [it6,res2] = lmap.insert({56,70});
    assert(75 == it6->second);
    assert(false == res2);    

    auto ret1 = lmap.erase(56);
    assert(1 == ret1);

    auto ret2 = lmap.erase(56);
    assert(0 == ret2);
}