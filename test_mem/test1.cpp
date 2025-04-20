int main() {
    int* a = new int;
    delete a;
    delete a; // Error: Double delete detected for 'a'

    int* b;
    delete b;
}
