int main() {
    int* a = new int[5];
    delete a; // ⚠️ new[] with delete

    float* b = new float;
    delete[] b; // ⚠️ new with delete[]
}
