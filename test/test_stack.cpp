#include "mystack.h"
#include <cassert>

void test_stack() {
    mystl::LStack<int> st;
    assert(st.empty());

    st.push(1);
    assert(1 == st.size());
    assert(1 == st.top());

    st.push(2);
    assert(2 == st.size());
    assert(2 == st.top());

    st.pop();
    assert(1 == st.size());
    assert(1 == st.top());

    mystl::LStack<std::string> st2;
    st2.emplace("watashi wa takagi, anata no namae wa nan desu ka?");
}