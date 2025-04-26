// test_smartptr_noinit.cpp
// ——————————————————————————————
// A pointer declaration without new should NOT trigger any hint.

void baz() {
    float* f;    // no hint here
    // f = …;
}
