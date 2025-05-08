// test_smartptr_shared.cpp
// global and static pointers → shared_ptr suggestions

// #include <iostream>
#include <memory>
using namespace std;

int* gbl = new int(42);     // suggest std::make_shared<int>(42)

void foo() {
    static double* s = new double(3.14);  // suggest static make_shared<double>
    // std::cout << *s << "\n";
    delete s;
}

int main() {
    delete gbl;
    return 0;
}
