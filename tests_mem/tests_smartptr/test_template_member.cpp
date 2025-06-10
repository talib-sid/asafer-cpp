// test_template_member.cpp
// ——————————————————————————————
// Templated struct with pointer member → weak_ptr

#include <memory>

template<typename T>
struct Holder {
    T*   data;     // HINT: std::weak_ptr<T> (if non-owning)
    void set(T* p) {
        data = p;
    }
};

int main() {
    Holder<int> h;
    h.set(new int(42));
    return 0;
}
