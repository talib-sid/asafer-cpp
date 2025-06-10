// test_unique_simple.cpp
// ——————————————————————————————
// Local pointer → unique_ptr

void alpha() {
    float* f = new float(1.23f);
    // HINT: std::make_unique<float>(1.23f)
    delete f;
}
