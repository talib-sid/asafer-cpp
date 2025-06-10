// test_shared_globals.cpp
// ——————————————————————————————
// Global and namespace-scope pointer → shared_ptr

// #include <memory>
// #include <string>

char*  g1 = new char('A');              // HINT: std::make_shared<char>('A')
int main() {
    delete g1;
    return 0;
}

// 15.1.0
