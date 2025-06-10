// test_escape_return.cpp
#include <memory>

struct Foo { Foo(int) {} };

Foo* makeFoo() {
    Foo* p = new Foo(42);   // ESCAPES via return ⇒ shared_ptr
    return p;
}

int main() {
    Foo* f = makeFoo();     // local receive but ownership transferred
    return 0;
}
