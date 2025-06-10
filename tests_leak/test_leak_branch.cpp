// test_leak_branch.cpp
// ——————————————————————————————
// This example shows a conditional allocation that’s only freed
// on one branch of a boolean test.  When the branch does NOT delete,
// your tool should flag it as a leak.

// #include <iostream>

void maybeFree(bool cond) {
    char* buf = new char[16];  // ALLOC: always new[]
    if (cond) {
        delete[] buf;          // FREED on one path
        return;
    }
    // NO delete[] here when cond==false ⇒ LEAK
}

int main() {
    maybeFree(false);  // will leak
    maybeFree(true);   // no leak
    return 0;
}