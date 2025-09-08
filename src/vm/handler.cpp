/****************************************************** 
-     Date:  2025.08.27 00:02
-     File:  handler.cpp
-     CopyRight Lamina Team
-     This project is followed GPL-3.0 license
********************************************************/
#include "handler.hpp"
#include "handler_fn.hpp"
#include "vm.hpp"

RegisterVM* Handler::current_vm = nullptr;

void Handler::vmCallTable(RegisterVM& vm) {
    static bool initialized = false;
    if (initialized) return;
    initialized = true;
    current_vm = &vm;
    HandlerFunction<0>::func();
    HandlerFunction<1>::func();
    HandlerFunction<2>::func();

}