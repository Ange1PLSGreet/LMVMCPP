/****************************************************** 
-     Date:  2025.08.28 15:31
-     File:  console_io.cpp
-     CopyRight Lamina Team
-     This project is followed GPL-3.0 license
********************************************************/
#include "console_io.hpp"
#include "../vm/handler.hpp"

void ConsoleIO::vmCallPrint() {
    RegisterVM::vm_call_handlers[0] = [](const OpCodeImpl::Instruction*) {
        const size_t addr = Handler::current_vm->registers[9];
        fputs(reinterpret_cast<const char*>(&Handler::current_vm->heap[addr]), stdout);
    };
}

void ConsoleIO::vmCallInput() {
    RegisterVM::vm_call_handlers[1] = [](const OpCodeImpl::Instruction*) {
        const size_t addr = Handler::current_vm->registers[9];
        std::string input;
        std::getline(std::cin, input);
        if (addr + input.length() >= Handler::current_vm->heap.size()) {
            Handler::current_vm->heap.resize(addr + input.length() + 1);
        }
        for (size_t i = 0; i < input.length(); ++i) {
            Handler::current_vm->heap[addr + i] = static_cast<int8_t>(input[i]);
        }
        Handler::current_vm->heap[addr + input.length()] = 0;
    };
}
