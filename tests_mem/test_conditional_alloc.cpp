int main() {
    int* data;

    data = new int[2];
    delete data; // should be delete[] for deallocate of the entire thing
}
