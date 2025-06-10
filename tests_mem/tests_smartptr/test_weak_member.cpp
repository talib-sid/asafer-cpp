// test_weak_member.cpp
// ——————————————————————————————
// Class-field raw pointer → weak_ptr

#include <memory>

class Widget {
public:
    std::shared_ptr<Widget> child;
    Widget*                 parent;   // HINT: std::weak_ptr<Widget>
};

int main() {
    return 0;
}
