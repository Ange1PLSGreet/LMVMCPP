/****************************************************** 
-     Date:  2025.08.28 15:27
-     File:  handler_init.cpp
-     CopyRight Lamina Team
-     This project is followed GPL-3.0 license
********************************************************/
#include "../vm/handler.hpp"
#include "../vmcall/console_io.hpp"


Handler::Handler() {
    handlers = {
        [] { ConsoleIO::vmCallPrint(); },
        [] { ConsoleIO::vmCallInput(); },
        [] { ConsoleIO::vmExit();      }
    };
}