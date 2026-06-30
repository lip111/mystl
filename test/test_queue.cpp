#include "myqueue.h"
#include <cassert>

void test_queue() {
    mystl::LQueue<int> que;
    assert(que.empty());

    que.push(1);
    que.push(2);
    que.push(3);
    que.push(4);

    assert(4 == que.size());
    assert(1 == que.front());
    assert(4 == que.back());

    que.pop();
    assert(3 == que.size());
    assert(2 == que.front());

    mystl::LQueue<std::string> que2;
    que2.emplace("hajimemashite, nishikata desu.");
}