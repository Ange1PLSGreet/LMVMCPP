#我的cmake就写这么简单

cmake_minimum_required(VERSION 3.16.3)

project(lmvmcpp)

# 添加FLTO开关选项，默认关闭
option(ENABLE_FLTO "Enable Link Time Optimization" ON)

# 启用测试
enable_testing()

# 查找GTest，如果找不到则使用FetchContent自动下载
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    set(GTEST_LIBRARIES GTest::gtest GTest::gtest_main)
endif()

set(CMAKE_CXX_STANDARD 20)
if (ANDROID)
    if (ENABLE_FLTO)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -flto")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
    endif()
else()
    # 暂时去掉优化, 有些编译器debug期不支持优化 -flto
    if (ENABLE_FLTO)
        if (MSVC)
            # MSVC的优化标志
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GL")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG")
        else()
            # GCC/Clang的优化标志
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -O3 -march=native -flto")
        endif()
    else()
        if (NOT MSVC)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -O3 -march=native")
        endif()
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

if(GTest_FOUND)
    target_link_libraries(ConsoleIOTest GTest::gtest GTest::gtest_main pthread)
else()
    target_link_libraries(ConsoleIOTest ${GTEST_LIBRARIES})
    if(NOT MSVC)
        target_link_libraries(ConsoleIOTest pthread)
    endif()
endif()
