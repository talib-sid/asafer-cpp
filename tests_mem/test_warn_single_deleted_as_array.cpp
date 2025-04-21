int main() {
    int* x = new int;
    delete[] x;  // Should warn: allocated with new but deleted with delete[]
}
