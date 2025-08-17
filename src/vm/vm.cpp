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
            // 文件操作指令处理
            case OpCode::FOPEN: {
                // 从堆中获取文件名
                std::string filename;
                for (size_t i = instr.imm; i < heap.size() && heap[i] != 0; ++i) {
                    filename += static_cast<char>(heap[i]);
                }
                
                // 根据rs寄存器的值确定打开模式
                std::ios::openmode mode = std::ios::in | std::ios::out;
                if (registers[instr.rs] & 1) mode = std::ios::in;
                if (registers[instr.rs] & 2) mode = std::ios::out;
                if (registers[instr.rs] & 4) mode |= std::ios::app;
                if (registers[instr.rs] & 8) mode |= std::ios::binary;
                
                registers[instr.rd] = file_open(filename, mode);
                break;
            }
            case OpCode::FREAD: {
                // 从文件中读取数据到堆中
                auto data = file_read(registers[instr.rs], static_cast<size_t>(instr.imm));
                
                // 将数据写入堆中
                heap.reserve(heap_ptr + data.size() + 1);
                if (heap.size() < heap_ptr + data.size() + 1) {
                    heap.resize(heap_ptr + data.size() + 1);
                }
                
                for (size_t i = 0; i < data.size(); ++i) {
                    heap[heap_ptr + i] = data[i];
                }
                heap[heap_ptr + data.size()] = 0; // 空终止符
                
                // 返回读取的字节数
                registers[instr.rd] = static_cast<int64_t>(data.size());
                break;
            }
            case OpCode::FWRITE: {
                // 从堆中获取要写入的数据
                std::vector<int8_t> data;
                for (size_t i = instr.imm; i < heap.size() && heap[i] != 0; ++i) {
                    data.push_back(heap[i]);
                }
                
                file_write(registers[instr.rs], data);
                registers[instr.rd] = static_cast<int64_t>(data.size());
                break;
            }
            case OpCode::FCLOSE: {
                file_close(registers[instr.rd]);
                break;
            }
            case OpCode::HALT: {
                return;
            }
            default:
                throw std::runtime_error("Unknown opcode");
        }
    }