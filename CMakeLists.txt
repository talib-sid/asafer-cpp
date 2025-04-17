cmake_minimum_required(VERSION 3.13)
project(ClangMemoryAnalyzer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable generation of compile_commands.json (useful for clangd, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Let the user know we're starting
message(STATUS "Configuring ClangMemoryAnalyzer")

# Find LLVM and Clang via CONFIG mode
set(LLVM_DIR "C:/msys64/mingw64/lib/cmake/llvm" CACHE PATH "LLVM config path")
set(Clang_DIR "C:/msys64/mingw64/lib/cmake/clang" CACHE PATH "Clang config path")

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Using ClangConfig.cmake in: ${Clang_DIR}")

# Include directories
include_directories(
  ${LLVM_INCLUDE_DIRS}
  ${CLANG_INCLUDE_DIRS}  
  
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MatchHandlers
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MemoryTracker

  ${CMAKE_CURRENT_SOURCE_DIR}/src/AST
)


add_definitions(${LLVM_DEFINITIONS})

# Your executable
add_executable(analyzer src/main.cpp)


# Link Clang and LLVM libraries (make sure to match versions)
target_link_libraries(analyzer
  # PRIVATE
  # clangTooling
  # clangFrontend
  # clangAST
  # clangBasic
  # clangLex
  PRIVATE
  clangFrontend
  clangTooling
  clangAST
  clangLex
  clangBasic

)


# Optional: For debugging build options
message(STATUS "Clang Tool will be built in ${CMAKE_BINARY_DIR}")
