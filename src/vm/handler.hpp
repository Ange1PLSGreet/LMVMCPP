/****************************************************** 
-     Date:  2025.08.27 00:02
-     File:  handler.hpp
-     CopyRight Lamina Team
-     This project is followed GPL-3.0 license
********************************************************/
#pragma once
#include "vm.hpp"

/**
 * 统一分发器
 */
class Handler{
public:
    /**
     * 调用表函数
     * @return void
     */
     static void vmCallTable(RegisterVM& vm);

     static RegisterVM* current_vm; // 当前VM的实例
};
