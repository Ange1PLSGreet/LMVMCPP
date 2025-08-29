#我的cmake就写这么简单

cmake_minimum_required(VERSION 3.16.3)

project(lmvmcpp)

# 添加FLTO开关选项，默认开启
option(ENABLE_FLTO "Enable Link Time Optimization" ON)

# 启用测试
enable_testing()

# 使用系统安装的 GTest，默认关闭
option(USE_SYSTEM_GTEST "Use system-installed GTest instead of submodule" OFF)

if (USE_SYSTEM_GTESTA)
    # 查找GTest
    find_package(GTest REQUIRED)
else()
    if (NOT EXISTS "${CMAKE_SOURCE_DIR}/third_party/googletest")
        message(FATAL_ERROR "googletest submodule not found, maybe enable USE_SYSTEM_GTEST to use system-installed GTest, or run \"git submodule update --init --recursive\" to solve this. ")
    endif()
    # 使用submodule里的GTest
    add_subdirectory(third_party/googletest)
endif()

set(CMAKE_CXX_STANDARD 20)
if (ANDROID)
    # Android端不支持avx2指令集
    if (ENABLE_FLTO)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -flto")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
    endif()
else()
    # 暂时去掉优化, 有些编译器debug期不支持优化 -flto
    if (ENABLE_FLTO)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -O3 -march=native -flto")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -O3 -march=native")
    endif()
endif()


add_executable(LMVMCPP
        src/main.cpp
        src/file_loader.cpp
        src/file_loader.hpp
        src/opcode.cpp
        src/opcode.hpp
        src/vm/vm.cpp
        src/vm/vm.hpp
        src/vm/local_state.cpp
        src/syscall/file_operand.cpp
        src/vm/handler.cpp
        src/vm/models.cpp
        src/vm/models.hpp
        src/vm/handler_init.cpp
        src/vmcall/console_io.cpp
        src/vm/handler.cpp
        src/syscall/file_operand.hpp
)

add_executable(ConsoleIOTest
        tests/console_io_test.cpp
        src/vm/vm.cpp
        src/vm/vm.hpp
        src/vm/local_state.cpp
        src/vm/handler.cpp
        src/vm/models.cpp
        src/vm/models.hpp
        src/vm/handler_init.cpp
        src/vmcall/console_io.cpp
        src/opcode.cpp
        src/opcode.hpp
)

# 链接GTest库
if (USE_SYSTEM_GTEST)
    target_link_libraries(ConsoleIOTest GTest::gtest GTest::gtest_main pthread)
else()
    target_link_libraries(ConsoleIOTest gtest gtest_main pthread)
endif()
