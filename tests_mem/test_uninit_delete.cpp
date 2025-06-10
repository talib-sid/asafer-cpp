int main() {
    int* x;  // -> uninitialized state.
    // x = new int;
    delete x;  //  Not allocated with new (garbage or nullptr)
}
