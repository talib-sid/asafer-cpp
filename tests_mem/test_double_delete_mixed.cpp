int main() {
    int* a = new int[4];
    delete a;    // Mismatch (new[] + delete)
    delete a;    // Double delete
}
