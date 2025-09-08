/****************************************************** 
-     Date:  2025.08.31 10:58
-     File:  handler_fn.hpp
-     CopyRight Lamina Team
-     This project is followed GPL-3.0 license
********************************************************/
#pragma once
#include "handler.hpp"
#include "../vmcall/console_io.hpp"
template<size_t>
struct HandlerFunction {
    static void (*func)();
};

template<>
struct HandlerFunction<0> {
    static void func() { ConsoleIO::vmCallPrint(); }
};

template<>
struct HandlerFunction<1> {
    static void func() { ConsoleIO::vmCallInput(); }
};

template<>
struct HandlerFunction<2> {
    static void func() { ConsoleIO::vmCallExit(); }
};

class HandlerFn : public Handler {
public:
    static constexpr size_t HANDLER_COUNT = 3;

    template<size_t N>
    static void callHandler();
};
