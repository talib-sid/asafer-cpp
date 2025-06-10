// test_null_deref.cpp
// #include <iostream>

void foo() {
    int *p = nullptr;           // ptrState[p] == Null
    int  x = *p;                // definite null deref
}

int main() {
    foo();
    return 0;
}
