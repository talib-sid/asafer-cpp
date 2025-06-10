// test_mixed.cpp

int* make_null() {
    return nullptr;
}

void mixed() {
    int *p1 = make_null();      // Unknown→NonNull? (we conservatively mark NonNull)
    int *p2;                    // Unknown
    p2 = nullptr;               // ptrState[p2] = Null

    if (p1) {
        int x = *p1;            // path‐insensitive: ptrState[p1] == NonNull ⇒ no warning
    }

    int y = *p2;                // definite null deref
    int z;                      
    if (p2 != nullptr) {
        z = 42;                 
    }                           // z may still be uninitialized here
}

int main() {
    mixed();
}
