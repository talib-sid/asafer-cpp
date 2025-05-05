// test_mixed.cpp
// ——————————————————————————————
// Mixed scenarios in one file

#include <memory>

int*  a = new int;            // shared_ptr hint
static double* b = new double; // shared_ptr hint

void f() {
    char*  c = new char;      // unique_ptr hint
    delete c;
}

struct S {
    std::shared_ptr<S> next;
    S*                 prev;   // weak_ptr hint
};

int main() {
    f();
    delete a;
    delete b;
    return 0;
}
