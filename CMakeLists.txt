#我的cmake就写这么简单

cmake_minimum_required(VERSION 3.31)

project(test_project LANGUAGE CXX VERSION 000)

set(CMAKE_CXX_STANDARD 20)

add_executable(LMVMCPP src/main.cpp
    src/opcode.cpp
    src/opcode.h
    src/vm/vm.cpp
    src/vm/vm.h
)

