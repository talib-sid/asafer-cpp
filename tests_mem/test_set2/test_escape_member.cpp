// test_escape_member.cpp
#include <memory>

struct Holder {
    int* data;
};

void g() {
    Holder h;
    h.data = new int(10);   // ESCAPES into struct ⇒ shared_ptr
}

int main() { return 0; }
