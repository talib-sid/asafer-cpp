# asafer cpp

## Overview
asafer_C++ is a static analyzer for C++ code that detects potential memory safety issues. It is built on top of the Clang Static Analyzer and uses the Clang tooling infrastructure to provide a user-friendly interface for analyzing C++ code.


## what is what

- ```analyzer``` -> the main executable that runs the analysis
- ```analyzer.cpp``` -> the main file that contains the entry point for the analyzer
- ```analyzer.h``` -> the header file that contains the declarations for the analyzer
```main.cpp``` -> CLI + setup only


## How to Build
```bash
cd ~/clang-tool/build
cmake .. -G Ninja
ninja
```

## How to run

``` bash
./analyzer ../test.cpp -- -std=c++17
```