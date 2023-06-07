cmake_minimum_required(VERSION 3.20)
project(compiler)

set(CMAKE_CXX_STANDARD 20)

aux_source_directory(. compiler_src)
add_executable(compiler main.cpp ${compiler_src} Error.h Symbol.h Virtual.h)