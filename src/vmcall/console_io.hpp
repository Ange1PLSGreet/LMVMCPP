/****************************************************** 
-     Date:  2025.08.28 15:35
-     File:  console_io.hpp
-     CopyRight Lamina Team
-     This project is followed GPL-3.0 license
********************************************************/
#pragma once
#include <iostream>

class ConsoleIO{
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
     * 虚拟机退出
     * @return void
     */
    static void vmCallExit();
};