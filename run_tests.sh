#!/bin/bash

# --- Configuration ---
# Path to your analyzer binary (adjust if you use a different build dir)
TOOL="./build/analyzer.exe"
if [ ! -x "$TOOL" ]; then
    echo "Error: Analyzer not found or not executable at $TOOL"
    exit 1
fi

TEST_DIR="./tests_mem"
OUTPUT_DIR="./test_results"
mkdir -p "$OUTPUT_DIR"

# List of test files (relative to TEST_DIR)
TEST_FILES=(
  test_ok_single.cpp
  test_ok_array.cpp
  test_warn_array_deleted_as_single.cpp
  test_warn_single_deleted_as_array.cpp
  test_double_delete_mixed.cpp
  test_uninit_delete.cpp
  test_conditional_alloc.cpp
)

# --- Run Tests ---
for test_file in "${TEST_FILES[@]}"; do
    echo "Running test: $test_file"
    OUTPUT_FILE="$OUTPUT_DIR/${test_file%.cpp}_output.txt"
    # Invoke the tool with the proper path
    "$TOOL" "$TEST_DIR/$test_file" -- -std=c++17 > "$OUTPUT_FILE" 2>&1
    echo "  ↳ Results → $OUTPUT_FILE"
done

echo "All tests completed! Results are in $OUTPUT_DIR"
