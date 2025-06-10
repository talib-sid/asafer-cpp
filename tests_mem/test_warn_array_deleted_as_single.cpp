int main() {
    int* arr = new int[5];
    delete arr;  // Should warn: allocated with new[] but deleted with delete
}
