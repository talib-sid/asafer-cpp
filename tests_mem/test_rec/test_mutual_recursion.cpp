// test_mutual_recursion.cpp
bool isEven(int);

bool isOdd(int n) {
    if (n == 0) return false;
    return isEven(n - 1);  // calls isEven
}

bool isEven(int n) {
    if (n == 0) return true;
    return isOdd(n - 1);   // calls isOdd → cycle
}
