// test_smartptr_array.cpp
// ——————————————————————————————
// An array allocation; your current hint logic will suggest
// make_unique<T> (for single objects).  Later you could
// extend this to suggest unique_ptr<T[]>.

void bar() {
    char* buf = new char[256];  // HINT: consider replacing with std::make_unique<char>()
    // … use buf …
    delete[] buf;
}
