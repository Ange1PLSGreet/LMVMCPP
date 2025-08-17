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

// 文件打开方法
int64_t RegisterVM::file_open(const std::string& filename, std::ios::openmode mode) {
    std::fstream file(filename, mode);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    
    int64_t fd = next_file_descriptor++;
    file_descriptors[fd] = std::move(file);
    return fd;
}

// 文件读取方法
std::vector<int8_t> RegisterVM::file_read(int64_t fd, size_t count) {
    auto it = file_descriptors.find(fd);
    if (it == file_descriptors.end()) {
        throw std::runtime_error("Invalid file descriptor");
    }
    
    std::vector<int8_t> buffer(count);
    it->second.read(reinterpret_cast<char*>(buffer.data()), count);
    
    // 调整缓冲区大小以匹配实际读取的字节数
    buffer.resize(it->second.gcount());
    return buffer;
}

// 文件写入方法
void RegisterVM::file_write(int64_t fd, const std::vector<int8_t>& data) {
    auto it = file_descriptors.find(fd);
    if (it == file_descriptors.end()) {
        throw std::runtime_error("Invalid file descriptor");
    }
    
    it->second.write(reinterpret_cast<const char*>(data.data()), data.size());
    it->second.flush();
}

// 文件关闭方法
void RegisterVM::file_close(int64_t fd) {
    auto it = file_descriptors.find(fd);
    if (it == file_descriptors.end()) {
        throw std::runtime_error("Invalid file descriptor");
    }
    
    it->second.close();
    file_descriptors.erase(it);
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
            case OpCode::HALT: {
                return;
            }
            default:
                throw std::runtime_error("Unknown opcode");
        }
    }