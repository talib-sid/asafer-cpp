
TOOL="./build/analyzer.exe"
for t in tests_mem/test_rec/test_*.cpp; do
  echo "=== $t ==="
  "$TOOL" "$t" -- -std=c++17
  echo
done