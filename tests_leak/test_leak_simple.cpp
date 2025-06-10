// test_leak_simple.cpp
// ——————————————————————————————
// This example shows a function‑local allocation that never gets freed.
// We expect your tool to report a leak for `p` in leak1() and no
// leak for `q` in noleak1().

// #include <iostream>

void leak1() {
    int* p = new int;    // LEAK: `p` is allocated but never deleted
}

void noleak1() {
    int* q = new int;    // OK: `q` is allocated...
    delete q;            // ...and correctly deleted
}

int main() {
    leak1();
    noleak1();
    return 0;
}
