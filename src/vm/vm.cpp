/******************************************************
-     Date:  2025.08.24 21:58
-     File:  vm.cpp
-     CopyRight Lamina Team
-     This project is followed GPL-3.0 license
********************************************************/
#include "vm.hpp"
#include <iostream>
#include <string>

std::map<uint8_t, std::function<void(const OpCodeImpl::Instruction*)>> RegisterVM::vm_call_handlers;

void RegisterVM::vm_error(const OpCodeImpl::Instruction& instr) {
    std::cout << "VM Error: ";
    if (instr.rd >= NUM_REGS || instr.rs >= NUM_REGS)
                    throw std::runtime_error("Invalid register number");
    exit(1);
}

size_t RegisterVM::newFunc(const std::vector<OpCodeImpl::Instruction>& program) {
    size_t index = FuncLists.size();
    FuncLists.push_back(program);
    return index;
}

void RegisterVM::run(const std::vector<OpCodeImpl::Instruction>& program){
    const size_t prog_size = program.size();
    if (prog_size == 0) return;

    const OpCodeImpl::Instruction* instr_ptr = program.data();
    const OpCodeImpl::Instruction* end_ptr = instr_ptr + prog_size;

    while (instr_ptr < end_ptr) {
        switch (instr_ptr->op) {
            case OpCodeImpl::OpCode::NEW: {
                newOnHeap(instr_ptr);
                break;
            }
            case OpCodeImpl::OpCode::MOVRM:
            case OpCodeImpl::OpCode::MOVRI: {
                registers[instr_ptr->rd] = instr_ptr->imm;
                break;
            }
            case OpCodeImpl::OpCode::MOVRR: {
                registers[instr_ptr->rd] = registers[instr_ptr->rs];
                break;
            }
            case OpCodeImpl::OpCode::MOVMI: {
                heap[instr_ptr->mem] = static_cast<signed char>(instr_ptr->imm);
                break;
            }
            case OpCodeImpl::OpCode::MOVMM: {
                heap[instr_ptr->mem] = static_cast<signed char>(registers[instr_ptr->imm]);
                break;
            }
            case OpCodeImpl::OpCode::MOVMR: {
                heap[instr_ptr->mem] = static_cast<signed char>(registers[instr_ptr->rs]);
                break;
            }
            case OpCodeImpl::OpCode::ADDR: {
                registers[instr_ptr->rd] += registers[instr_ptr->rs];
                break;
            }
            case OpCodeImpl::OpCode::ADDI: {
                registers[instr_ptr->rd] += instr_ptr->imm;
                break;
            }
            case OpCodeImpl::OpCode::ADDM: {
                registers[instr_ptr->rd] += heap[instr_ptr->mem];
                break;
            }
            case OpCodeImpl::OpCode::SUBR: {
                registers[instr_ptr->rd] -= registers[instr_ptr->rs];
                break;
            }
            case OpCodeImpl::OpCode::SUBI: {
                registers[instr_ptr->rd] -= instr_ptr->imm;
                break;
            }
            case OpCodeImpl::OpCode::SUBM: {
                registers[instr_ptr->rd] -= heap[instr_ptr->mem];
                break;
            }
            case OpCodeImpl::OpCode::MULR: {
                registers[instr_ptr->rd] *= registers[instr_ptr->rs];
                break;
            }
            case OpCodeImpl::OpCode::MULI: {
                registers[instr_ptr->rd] *= instr_ptr->imm;
                break;
            }
            case OpCodeImpl::OpCode::MULM: {
                registers[instr_ptr->rd] *= heap[instr_ptr->mem];
                break;
            }
            case OpCodeImpl::OpCode::DIVR: {
                registers[instr_ptr->rd] /= registers[instr_ptr->rs];
                break;
            }
            case OpCodeImpl::OpCode::DIVI: {
                registers[instr_ptr->rd] /= instr_ptr->imm;
                break;
            }
            case OpCodeImpl::OpCode::DIVM: {
                registers[instr_ptr->rd] /= heap[instr_ptr->mem];
                break;
            }
            case OpCodeImpl::OpCode::IFRR: {
                if(cmp_if_bool<int64_t,int64_t>(instr_ptr->data[0],registers[instr_ptr->rd],registers[instr_ptr->rs])) {
                    run(CallLists[instr_ptr->imm]);
                    if(instr_ptr->size == 19000)return; //临时定义一个用于返回的跳转
                }
                break;
            }
            case OpCodeImpl::OpCode::SYSCALL:
            case OpCodeImpl::OpCode::VMCALL: {
                registerUnionHandler(instr_ptr);
                break;
            }
            case OpCodeImpl::OpCode::CALL: {
                funcCalling(instr_ptr);
                break;
            }
            case OpCodeImpl::OpCode::RET: {
                return;
            }
            case OpCodeImpl::OpCode::HALT: {
                break;
            }
            default:
                throw std::runtime_error("Unknown opcode");
        }

        instr_ptr++;
    }
}

template<typename T1,typename T2>
bool RegisterVM::cmp_if_bool(int8_t bool_cmp, T1 left, T2 right){
        switch (bool_cmp){
            case 0: return left == right;
            case 1: return left != right;
            case 2: return left > right;
            case 3: return left < right;
            case 4: return left >= right;
            case 5: return left <= right;
            default: throw std::runtime_error("Unknown bool_cmp");
        }
}

size_t RegisterVM::newCall(const std::vector<OpCodeImpl::Instruction>& program){
    size_t index = CallLists.size();
    CallLists.push_back(program);
    return index;
}

inline void RegisterVM::funcCalling(const OpCodeImpl::Instruction *instr) {
    try {
        LocalState local_state;
        local_state.saveAllRegisters(registers);
        run(FuncLists[instr->imm]);
        local_state.setReturnValue(registers[0]);
        local_state.restoreAllRegisters(registers);
        registers[0] = local_state.getReturnValue();
    } catch (const std::exception& e) {
        std::cerr << "VM Error: " << e.what() << std::endl;
    }
}

inline void RegisterVM::newOnHeap(const OpCodeImpl::Instruction *instr) {
    if(instr->data.empty()) vm_error(*instr);
    registers[1] = heap.size();
    heap.reserve(heap.size() + instr->data.size());
    heap.insert(heap.end(),instr->data.begin(),instr->data.end());
}

inline void RegisterVM::registerUnionHandler(const OpCodeImpl::Instruction* instr) {
    size_t handler_count = vm_call_handlers.size();

    if (instr->imm >= 0 && instr->imm < static_cast<int64_t>(handler_count)) {
        auto it = vm_call_handlers.find(static_cast<uint8_t>(instr->imm));
        if (it != vm_call_handlers.end()) {
            it->second(instr);
        } else {
            throw std::runtime_error("VMUnionHandler not found for index: " + std::to_string(instr->imm));
        }
    } else {
        throw std::runtime_error("VMUnionHandler index out of range: " + std::to_string(instr->imm) +
                                ", valid range: 0-" + std::to_string(handler_count - 1));
    }
}


