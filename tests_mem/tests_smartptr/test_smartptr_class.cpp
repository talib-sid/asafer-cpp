// test_smartptr_class.cpp
// ——————————————————————————————
// Pointer to a user‐defined type.

struct Widget {
    Widget(int) {}
    void act() {}
};

int main() {
    Widget* w = new Widget(10);  // HINT: consider replacing with std::make_unique<Widget>()
    w->act();
    delete w;
    return 0;
}
