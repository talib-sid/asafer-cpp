
TOOL="./build/analyzer.exe"
for t in tests_mem/test_null/test_*.cpp; do
  echo "=== $t ==="
  "$TOOL" "$t" -- -std=c++17
  echo
done