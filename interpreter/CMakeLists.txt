cmake_minimum_required(VERSION 3.20)
project(interpreter)

set(CMAKE_CXX_STANDARD 20)

aux_source_directory(. interpreter)
add_executable(interpreter ${interpreter})