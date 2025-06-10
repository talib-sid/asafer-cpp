// test_shared_static_local.cpp
// ——————————————————————————————
// static local pointer → shared_ptr

#include <vector>

void beta() {
    std::vector<int> a(5);
    static int* arr = new int[5];  // HINT: static std::make_shared<int[]>(5)
    // ... use arr ...
    delete[] arr;
}
