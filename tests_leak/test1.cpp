int main() {
    int* a = new int;     //  not deleted
    int* b = new int;
    delete b;
}
