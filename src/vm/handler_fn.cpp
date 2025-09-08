#include "handler_fn.hpp"
/******************************************************
-     Date:  2025.08.31 11:02
-     File:  handler_fn.cpp
-     CopyRight Lamina Team
-     This project is followed GPL-3.0 license
********************************************************/
template<size_t N>
void HandlerFn::callHandler() {
    HandlerFunction<N>::func();
}
