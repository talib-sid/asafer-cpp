// script to print stuff without the heavy iostream overhead

// #include <stdio.h>  // Using C-style I/O instead of iostream
#include <llvm/Support/raw_ostream.h>

extern "C" {
    void print_int(int value) {
        // printf("%d\n", value);
        llvm::outs() << value << "-int\n";
    }
    void print_string(const char* str) {
        // printf("%d\n", value);
        llvm::outs() << value << "-string\n";
    }
}