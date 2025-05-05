// test_unique_return.cpp
// ——————————————————————————————
// Pointer returned from function → unique_ptr

int* makeInt() {
    return new int(7);  // HINT: return std::make_unique<int>(7);
}

int main() {
    int* p = makeInt();
    delete p;
    return 0;
}
