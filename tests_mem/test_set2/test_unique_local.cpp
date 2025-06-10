// test_unique_local.cpp
#include <memory>

void h() {
    double* d = new double(3.14);  // NO escape ⇒ unique_ptr
    *d *= 2;
    delete d;
}

int main() { return 0; }
