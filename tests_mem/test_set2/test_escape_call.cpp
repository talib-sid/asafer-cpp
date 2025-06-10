// test_escape_call.cpp
#include <memory>

void registerPtr(int*);  

void f() {
    int* p = new int(7);   // ESCAPES via call ⇒ shared_ptr
    registerPtr(p);
}

int main() { return 0; }
