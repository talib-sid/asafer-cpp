int main() {
    int* p = new int;
    delete p;       // should be OK (normal delete)

    float* f;
    delete f;       // Not allocated (ub)

    delete p;       // again, ub
    delete[] p;   // Not allocated with new[] (undefined behavior)
    delete[] f;   // Not allocated with new[] (undefined behavior)
}
