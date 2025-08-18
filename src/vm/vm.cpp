#include "vm.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace opcode;

void RegisterVM::vm_error(Instruction instr) {
    std::cout << "VM Error: ";
    if (instr.rd >= NUM_REGS || instr.rs >= NUM_REGS)
                    throw std::runtime_error("Invalid register number in MOV");
    exit(1);
}

void RegisterVM::execute(opcode::Instruction instr) {
        switch (instr.op) {
            case OpCode::NEW: {
                if(instr.data.empty()) vm_error(instr);
                heap.reserve(heap.size() + instr.data.size());
                heap.insert(heap.end(),instr.data.begin(),instr.data.end());
                break;
            }
            case OpCode::MOVRI: {
                registers[instr.rd] = instr.imm;
                break;
            }
            case OpCode::MOVRR: {
                registers[instr.rd] = registers[instr.rs];
                break;
            }
            case OpCode::MOVRM: {
                registers[instr.rd] = registers[instr.rs] == 0 ? instr.imm : registers[instr.rs];
                break;
            }
            case OpCode::ADDR: {
                registers[instr.rd] += registers[instr.rs];
                break;
            }
            case OpCode::ADDI: {
                registers[instr.rd] += instr.imm;
                break;
            }
            case OpCode::ADDM: {
                break;
            }
            case OpCode::SUBR: {
                registers[instr.rd] -= registers[instr.rs];
                break;
            }
            case OpCode::VMCALL: {
                switch (static_cast<VmCallList>(instr.imm)) {
                case VmCallList::CONSOLE_READ: {
                    //registers[instr.rd] = getchar();
                    break;
                }
                case VmCallList::CONSOLE_WRITE: {
                    for(size_t i=registers[1];heap[i]!=0;i++)putchar(heap[i]);
                    break;
                }
                case VmCallList::EXIT: {
                    exit(registers[instr.rd]);
                }
                }
                break;
            }
            case OpCode::CALL: {
                switch (static_cast<VmCallList>(instr.imm)) {
                case VmCallList::CONSOLE_READ: {
                    // registers[instr.rd] = getchar();
                    break;
                }
                case VmCallList::CONSOLE_WRITE: {
                    for (size_t i = registers[1]; heap[i] != 0; i++)
                        putchar(heap[i]);
                    break;
                }
                case VmCallList::EXIT: {
                    exit(registers[instr.rd]);
                }
                }
                break;
            }
            case OpCode::HALT: {
                return;
            }
            default:
                throw std::runtime_error("Unknown opcode");
        }
    }