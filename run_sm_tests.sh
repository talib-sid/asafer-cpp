for t in tests_mem/tests_smartptr/test_smartptr_*.cpp; do
  echo "=== $t ==="
  ./build/analyzer.exe "$t" -- -std=c++17
  echo
done
