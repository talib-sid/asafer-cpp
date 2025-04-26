// test_smartptr_leak.cpp
// ——————————————————————————————
// Shows both a smart‐pointer hint AND a leak report

int* leaky = new int;           // HINT + LEAK
int* fine  = new int(7);
void good() {
    delete fine;
}

int main() {
    // no delete(leaky) ⇒ leak
    good();
    return 0;
}
