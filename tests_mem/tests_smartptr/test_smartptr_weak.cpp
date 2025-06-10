// test_smartptr_weak.cpp
// class‐member raw pointer → weak_ptr suggestion

#include <memory>

struct Node {
    std::shared_ptr<Node> next;  // OK
    Node*                prev;   // suggest std::weak_ptr<Node>
};

int main() {
    return 0;
}
