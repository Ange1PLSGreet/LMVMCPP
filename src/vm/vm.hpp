/******************************************************
-     Date:  2025.08.28 15:35
-     File:  vm.hpp
-     CopyRight Lamina Team
-     This project is followed GPL-3.0 license
********************************************************/
#pragma once
#include "../opcode.hpp"
#include <iostream>
#include <functional>
#include <vector>
#include <cstring>
#include <fstream>
#include <map>
#include <memory>

// =========================
// 添加宏定义
// =========================
#ifdef _MSC_VER
#define __builtin_expect(EXP, C)  (EXP)// 实现gcc/clang自带函数
#endif



// =========================
// 定义寄存器数量
// =========================
constexpr uint8_t NUM_REGS = 15; // r0 ~ r14

// 使用内存对齐属性
struct alignas(uint8_t) Registers {
    uint8_t regs[NUM_REGS];
};

// =========================
// 寄存器状态控制
// =========================
class LocalState {
public:
    // 构造函数
    LocalState() : reg_mask(0), return_value(0) {
        // 使用memset初始化寄存器
        std::memset(saved_registers.regs, 0, sizeof(saved_registers.regs));
    }
    /**
     * 获取返回值
     * @return return_value
     */
    [[nodiscard]] int64_t getReturnValue() const { return return_value; }
    /**
     * 存储寄存器值（单个）
     * @param registers
     * @param reg_index
     */
    void saveRegister(const int64_t* registers, uint8_t reg_index);
    /**
     * 存储所有寄存器值
     * @param registers
     */
    void saveAllRegisters(const int64_t* registers);
    /**
     * 设置返回值
     * @param value
     */
    void setReturnValue(int64_t value) { return_value = value; }
    /**
     * 恢复所有寄存器值
     * @param registers
     */
    void restoreAllRegisters(int64_t* registers);
private:
    // 定义一些私有成员变量
    // 内存对齐
    struct alignas(32) Registers {
        int64_t regs[NUM_REGS];
    } saved_registers{};
    // 使用位掩码来跟踪哪些寄存器被使用（预先分配）
    static constexpr uint64_t FULL_MASK = (1ULL << NUM_REGS) - 1;
    // 寄存器状态位掩码
    uint64_t reg_mask;
    // 函数返回值
    int64_t return_value;
};

// =========================
// 寄存器式虚拟机
// =========================
class RegisterVM {
public:
    /**
     * 定义一个析构函数
     */
    virtual ~RegisterVM() = default;
    int64_t registers[NUM_REGS]{}; // r0 ~ r14
    std::vector<int8_t> heap;    // 堆

    static std::map<uint8_t, std::function<void(const OpCodeImpl::Instruction*)>> vm_call_handlers; // VM调用分发器
    /**
     * 初始化所有寄存器为 0
     */
    RegisterVM() {
        // 初始化寄存器为 0
        std::memset(registers, 0, sizeof(registers));
        heap.push_back(0);// 堆顶为 0
    }
    /**
     * 执行一组指令
     * @param program
     */
    void run(const std::vector<OpCodeImpl::Instruction>& program);
    /**
     * 通过统一分发器注册VMCALL/SYSCALL调用
     * @param instr
     */
    static void registerUnionHandler(const OpCodeImpl::Instruction *instr);
    /**
     * 新建函数
     * @param program
     * @return size_t
     */
    size_t newFunc(const std::vector<OpCodeImpl::Instruction>& program);
    /**
     * 新建控制流跳转
     * @param program
     * @return size_t
     */
    size_t newCall(const std::vector<OpCodeImpl::Instruction>& program);
private:
    int64_t heap_ptr = 1;        // 堆指针
    std::map<int64_t, std::shared_ptr<std::fstream>> file_descriptors; // 文件描述符映射
    int64_t next_file_descriptor = 1; // 下一个文件描述符
protected:
    /**
      * 虚拟机报错
      * @param instr
      */
    static void vm_error(const OpCodeImpl::Instruction& instr);
    /**
    * if判断
    * @tparam T1 模板参数
    * @tparam T2 模板参数
    * @param bool_cmp
    * @param left
    * @param right
    * @return
    */
    template<typename T1,typename T2>
    static bool cmpIfBool(int8_t bool_cmp, T1 left, T2 right);
    std::vector<std::vector<OpCodeImpl::Instruction>> FuncLists; // 函数列表
    std::vector<std::vector<OpCodeImpl::Instruction>> CallLists; // 控制流跳转所用的，避免与FuncLists混淆
    /**
     * 函数调用指令实现封装
     * @param instr
     * @return void
     */
    void funcCalling(const OpCodeImpl::Instruction* instr);
    /**
     * 向堆新分配内存
     * @param instr
     * @return void
     */
    void newOnHeap(const OpCodeImpl::Instruction* instr);


    template<typename T1, typename T2>
    using CmpFunc = bool(*)(T1, T2); // 比较函数指针

    /** 比较函数 **/
    template<typename T1, typename T2>
    /**
     * 比较值相等
     * @param left
     * @param right
     * @return bool
     */
    static bool cmpEq(T1 left, T2 right) { return left == right; }

    template<typename T1, typename T2>
    /**
     * 比较值不相等
     * @param left
     * @param right
     * @return bool
     */
    static bool cmpNe(T1 left, T2 right) { return left != right; }

    template<typename T1, typename T2>
    /**
     * 比较值大于
     * @param left
     * @param right
     * @return bool
     */
    static bool cmpGt(T1 left, T2 right) { return left > right; }

    template<typename T1, typename T2>
    /**
     * 比较值小于
     * @param left
     * @param right
     * @return bool
     */
    static bool cmpLt(T1 left, T2 right) { return left < right; }

    template<typename T1, typename T2>
    /**
     * 比较值大于等于
     * @param left
     * @param right
     * @return bool
     */
    static bool cmpGe(T1 left, T2 right) { return left >= right; }

    template<typename T1, typename T2>
    /**
     * 比较值小于等于
     * @param left
     * @param right
     * @return bool
     */
    static bool cmpLe(T1 left, T2 right) { return left <= right; }

    template<typename T1, typename T2>
    static inline constexpr CmpFunc<T1, T2> cmp_table[] = {
        cmpEq<T1, T2>,   // 0
        cmpNe<T1, T2>,   // 1
        cmpGt<T1, T2>,   // 2
        cmpLt<T1, T2>,   // 3
        cmpGe<T1, T2>,   // 4
        cmpLe<T1, T2>    // 5
    };
};
