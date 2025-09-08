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
        RegisterVM* vm = Handler::current_vm;
        if (!vm) return;

        const size_t addr = vm->registers[9];
        if (addr < vm->heap.size() && vm->heap[addr] != nullptr) {
            auto* arr = dynamic_cast<LmArray*>(vm->heap[addr]);
            if (arr) {
                std::string str;
                for (size_t i = 0; i < arr->get_size(); ++i) {
                    TaggedVal val = arr->get(i);
                    if (TaggedUtil::get_tagged_type(val) == TaggedType::Smi) {
                        char c = static_cast<char>(TaggedUtil::decode_Smi(val));
                        if (c == 0) break;
                        str += c;
                    }
                }
                fputs(str.c_str(), stdout);
            }
        }
    };
}

void ConsoleIO::vmCallInput() {
    RegisterVM::vm_call_handlers[1] = [](const OpCodeImpl::Instruction*) {
        RegisterVM* vm = Handler::current_vm;
        if (!vm) return;

        const size_t addr = vm->registers[9];
        std::string prompt;
        if (addr < vm->heap.size() && vm->heap[addr] != nullptr) {
            auto* arr = dynamic_cast<LmArray*>(vm->heap[addr]);
            if (arr) {
                for (size_t i = 0; i < arr->get_size(); ++i) {
                    TaggedVal val = arr->get(i);
                    if (TaggedUtil::get_tagged_type(val) == TaggedType::Smi) {
                        char c = static_cast<char>(TaggedUtil::decode_Smi(val));
                        if (c == 0) break;
                        prompt += c;
                    }
                }
            }
        }
        fputs(prompt.c_str(), stdout);

        std::string input;
        std::getline(std::cin, input);

        auto* arr = new LmArray(input.length() + 1);
        for (size_t i = 0; i < input.length(); ++i) {
            arr->push(TaggedUtil::encode_Smi(input[i]));
        }
        arr->push(TaggedUtil::encode_Smi(0));

        size_t output_addr = vm->heap.size();
        vm->heap.push_back(arr);

        vm->registers[0] = static_cast<int64_t>(output_addr);
    };
}

void ConsoleIO::vmCallExit() {
    RegisterVM::vm_call_handlers[2] = [](const OpCodeImpl::Instruction*) {
        RegisterVM* vm = Handler::current_vm;
        if (!vm) return;

        const size_t addr = vm->registers[9];
        int exit_code = 0;
        if (addr < vm->heap.size() && vm->heap[addr] != nullptr) {
            auto* arr = dynamic_cast<LmArray*>(vm->heap[addr]);
            if (arr && arr->get_size() > 0) {
                TaggedVal val = arr->get(0);
                if (TaggedUtil::get_tagged_type(val) == TaggedType::Smi) {
                    exit_code = static_cast<int>(TaggedUtil::decode_Smi(val));
                }
            }
        }
        std::exit(exit_code);
    };
}
