int main() {
    int* a = new int; 
    delete a;
    delete a; // (double delte)

    int* b;
    delete b; // (delete is fine, but should give warning that, you're del smthn that was never allocated on the heap)
}
