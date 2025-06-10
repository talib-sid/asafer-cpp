// test_smartptr_vector.cpp
#include <vector>

static std::vector<int>* gv = new std::vector<int>{1,2,3};  // shared_ptr hint

void baz() {
    std::vector<float>* vf = new std::vector<float>(10, 0.5f); // unique_ptr hint
    delete vf;
}

int main() {
    delete gv;
    return 0;
}
