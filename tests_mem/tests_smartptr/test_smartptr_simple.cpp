// test_smartptr_simple.cpp
// ——————————————————————————————
// A simple local pointer-initialization with new.
// We expect a hint to use std::make_unique<int>() instead.

// #include <iostream>
// #include <StreamPrinter.hpp>

void foo() {
    int* p = new int(5);  // HINT: consider replacing with std::make_unique<int>()
    // std::cout << *p << "\n";
    // print_int(5)
    delete p;             // still OK, hint should still appear
}

int main() {
    foo();
    return 0;
}
