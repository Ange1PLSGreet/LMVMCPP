/****************************************************** 
-     Date:  2025.08.28 15:35
-     File:  console_io.hpp
-     CopyRight Lamina Team
-     This project is followed GPL-3.0 license
********************************************************/
#pragma once
#include "../vm/internal_call_errno.hpp"
#include <iostream>

class ConsoleIO : public InternalCallErrno{
public:
    /**
     * 打印函数
     * @return void
     */
    static void vmCallPrint();

    /**
     * 输入函数
     * @return void
     */
    static void vmCallInput();

    /**
     * Override
     * @param code
     * @param func_name
    */
    void SyscallErrnoOut(long code, std::string func_name) override {
        std::cerr << "VMCall Error: ConsoleIOError, Code -> " << code << "Function: " <<func_name << "\n";
    }
};