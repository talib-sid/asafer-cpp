// test_uninit_var.cpp
// #include <iostream>

void bar() {
    int a;                      // varInit[a] == false
    int b = 5;                  // varInit[b] == true
    int c = a + b;              // use of uninitialized 'a'
    // std::cout << b << "\n";     // OK
}

int main() {
    bar();
    return 0;
}
