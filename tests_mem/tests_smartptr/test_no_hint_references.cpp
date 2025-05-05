// test_no_hint_references.cpp
// ——————————————————————————————
// References and non-new initializers should NOT trigger

void gamma() {
    int x = 10;
    int& r = x;      // no hint
    int* p = nullptr; // no hint (no new)
}
