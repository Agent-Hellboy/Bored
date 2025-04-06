cmake_minimum_required(VERSION 3.10)
project(keyword_extractor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find LLVM and Clang from provided paths
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Using ClangConfig.cmake in: ${Clang_DIR}")

# Setup LLVM and Clang include paths
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
include_directories(SYSTEM ${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Main tool executable
add_executable(keyword_extractor main.cpp)

# Link against required Clang/LLVM libs
target_link_libraries(keyword_extractor
  PRIVATE
    clangTooling
    clangFrontend
    clangAST
    clangLex
    clangBasic
    clangASTMatchers
    ${LLVM_LIBS}
)

# Suppress Clang-specific warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(keyword_extractor PRIVATE -Wno-unused-command-line-argument)
endif()

