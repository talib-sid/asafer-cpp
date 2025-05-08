// test_shared_globals.cpp
// ——————————————————————————————
// Global and namespace-scope pointer → shared_ptr

// #include <memory>
#include <string>

char*  g1 = new char('A');              // HINT: std::make_shared<char>('A')
// static duration in namespace
static std::string*  g2 = new std::string("hello");  
// HINT: std::make_shared<std::string>("hello")

int main() {
    delete g1;
    delete g2;
    return 0;
}

// 15.1.0
