# # #!/usr/bin/env bash
# # # set -euo pipefail

# # TOOL="./build/analyzer.exe"
# # if [[ ! -x "$TOOL" ]]; then
# #   echo "Error: analyzer not found at $TOOL"
# #   exit 1
# # fi

# # # These three cover CRT and libstdc++ headers on MSYS2
# # INCLUDE_FLAGS=(
# #   -I "$(pwd)/include"                   # <-- our stub
# #   -I C:/msys64/mingw64/include
# #   -I C:/msys64/mingw64/include/c++/14.2.0
# #   -I C:/msys64/mingw64/include/c++/14.2.0/x86_64-w64-mingw32
# # )

# # for t in tests_mem/tests_smartptr/test_*.cpp; do
# #   echo "=== $t ==="
# #   "$TOOL" "$t" -- -std=c++17 "${INCLUDE_FLAGS[@]}"
# #   echo
# # done

# #!/usr/bin/env bash
# # set -euo pipefail

# TOOL="./build/analyzer.exe"
# if [[ ! -x "$TOOL" ]]; then
#   echo "Error: analyzer not found at $TOOL"
#   exit 1
# fi

# # System headers for MSYS2’s CRT + libstdc++
# SYS_INCLUDES=(
#   "-I" "C:/msys64/mingw64/include"
#   "-I" "C:/msys64/mingw64/include/c++/14.2.0"
#   "-I" "C:/msys64/mingw64/include/c++/14.2.0/x86_64-w64-mingw32"
#   "-I" "C:/msys64/mingw64/include/c++/14.2.0/x86_64-w64-mingw32"
#   "-I" "C:/msys64/mingw64/x86_64-w64-mingw32/include"
# )

# for t in tests_mem/tests_smartptr/test_*.cpp; do
#   echo "=== $t ==="
#   "$TOOL" "$t" -- -std=c++17 "${SYS_INCLUDES[@]}"
#   echo
# done

# 15.1.0

# #!/usr/bin/env bash
# TOOL="./build/analyzer.exe"
# TEST_DIR="./tests_mem"
# for test in "$TEST_DIR"/tests_smartptr/*.cpp; do
#   echo "=== $test ==="
#   "$TOOL" "$test" -- -std=c++17 \
#       --gcc-toolchain=/mingw64 \
#       -I/mingw64/lib/gcc/x86_64-w64-mingw32/$(ls /mingw64/lib/gcc/x86_64-w64-mingw32 | head -n1)/include
#   echo
# done


# #!/usr/bin/env bash
# TOOL="./build/analyzer.exe"
# TEST_DIR="./tests_mem"
# for test in "$TEST_DIR"/tests_smartptr/*.cpp; do
#   echo "=== $test ==="
#   "$TOOL" "$test" -- -std=c++17 \
#       --gcc-toolchain=/mingw64 \
#       -I/mingw64/lib/gcc/x86_64-w64-mingw32/15.1.0/include
#   echo
# done


#!/usr/bin/env bash
TOOL="./build/analyzer.exe"
TEST_DIR="./tests_mem"
for test in "$TEST_DIR"/tests_smartptr/test_smartptr_vector.cpp; do
  echo "=== $test ==="
  "$TOOL" "$test" -- -std=c++17 \
      --gcc-toolchain=/mingw64 \
      -I/mingw64/lib/gcc/x86_64-w64-mingw32/15.1.0/include
  echo
done
