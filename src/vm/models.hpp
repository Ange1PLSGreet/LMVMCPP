/******************************************************
-     Date:  2025.08.28 01:37
-     File:  models.cpp
-     CopyRight Lamina Team
-     This project is followed GPL-3.0 license
********************************************************/
#pragma once

#include <cstdint>
#include <cstring>
#include <vector>

class LmHeapObject;

// 标记位：低3位区分数据类型（64位TaggedVal）
enum class TaggedType : unsigned char {
    HeapObject = 0b000,
    Smi = 0b001,
    Null = 0b010,
    BTrue = 0b011,
    BFalse = 0b111
};

// 64位TaggedVal：低3位为类型标记，高55位为数据
using TaggedVal = long long;

class TaggedUtil {
public:
    /**
     * 编码小整数
     * @param smi_val
     * @return TaggedVal
     */
    static TaggedVal encode_Smi(int64_t smi_val);
    /**
     * 编码堆对象指针
     * @param obj
     * @return TaggedVal
     */
    static TaggedVal encode_HeapObject(LmHeapObject* obj);

    /**
     * 解码TaggedVal的类型
     * @param val
     * @return TaggedType
     */
    static TaggedType get_tagged_type(TaggedVal val);

    /**
     * 解码小整数
     * @param val
     * @return int64_t
     */
    static int64_t decode_Smi(TaggedVal val);

    /**
     * 解码堆对象指针
     * @param val
     * @return LmHeapObject
     */
    static LmHeapObject* decode_HeapObject(TaggedVal val);

    /**
     * 判断TaggedVal是否为堆对象指针
     * @param val
     * @return bool
     */
    static bool is_HeapObject(TaggedVal val);
};

// 堆对象类型
enum class HeapObjType : unsigned char {
    CodeObject,
    Bigint,
    Array,
    Function,
    String,
    WeakRef
};

class LmHeapObject {
public:
    /**
     * 构造函数
     * @param type
     */
    explicit LmHeapObject(HeapObjType type) : type_(type), ref_count_(1) {}

    /**
     * 析构函数
     */
    virtual ~LmHeapObject() = default;

    /**
     * 增加引用计数
     * @return void
     */
    void make_ref();

    /**
     * 删除引用计数
     * @return void
     */
    void del_ref();

    /**
     * 获取堆对象类型
     * @return HeapObjType
     */
    [[nodiscard]] HeapObjType get_type() const { return type_; }

    /**
     * 获取引用计数
     * @return size_t
     */
    [[nodiscard]] size_t get_ref_count() const { return ref_count_; }
private:
    HeapObjType type_; // 指向堆对象类型
    size_t ref_count_; // 引用计数
};


class LmString : public LmHeapObject {
public:
    char* utf8_data_; // utf8数据

    /**
     * 构造函数
     * 初始化LmString
     * @param utf8_str
     */
    explicit LmString(const char* utf8_str)
        : LmHeapObject(HeapObjType::String),
          utf8_data_(nullptr),
          byte_length_(0),
          char_length_(0)
    {
        if (utf8_str == nullptr) utf8_str = "";

        byte_length_ = std::strlen(utf8_str);

        utf8_data_ = new char[byte_length_ + 1];

        std::strcpy(utf8_data_, utf8_str);

        char_length_ = calc_utf8_char_count(utf8_data_, byte_length_);
    }

    /**
     * 析构函数
     * 清理LmString
     */
    ~LmString() override {
        delete[] utf8_data_;
        utf8_data_ = nullptr;
    }

    /**
     * 获取UTF-8编码字符串
     * @return const char*
     */
    [[nodiscard]] const char* get_utf8_data() const {
        return utf8_data_;
    }

    /**
     * 获取字节长度
     * @return size_t
     */
    [[nodiscard]] size_t byte_len() const {
        return byte_length_;
    }

    /**
     * 获取字符长度
     * @return size_t
     */
    [[nodiscard]] size_t char_len() const {
        return char_length_;
    }

    /**
     * 比较字符串是否相等
     * @param other
     * @return bool
     */
    bool equals(const LmString* other) const;

    /**
     * 拼接字符串
     * @param other
     * @return LmString*
     */
    LmString* concat(const LmString* other) const;

private:
    size_t byte_length_;
    size_t char_length_;    // 缓存 UTF-8 实际字符数（如 "你好" 字节数6，字符数2）

    /**
     * 计算UTF-8字符串
     * @param utf8_str
     * @param byte_len
     * @return size_t
     */
    static size_t calc_utf8_char_count(const char* utf8_str, size_t byte_len);
};

class LmCodeObject : public LmHeapObject {
public:
    enum class CodeType { // 代码类型
        Bytecode,
        MachineCode
    };

private:
    CodeType code_type_; // 代码类型枚举
    std::vector<int> code_; // 字节码或机器码
    std::unordered_map<int,int> jmp_table_; // switch跳转表
    std::vector<LmHeapObject*> consts_      ; // 常量池
    void* machine_code_addr_; // 指向机器码地址
    size_t machine_code_len_; // 机器码长度
    size_t size_; // 代码大小

public:
    /**
     * 构造函数
     * @param code_type
     * @param code
     */
    LmCodeObject(CodeType code_type, const std::vector<int>& code)
        : LmHeapObject(HeapObjType::CodeObject),
          code_type_(code_type),
          code_(code),
          machine_code_addr_(nullptr),
          machine_code_len_(0),
          size_(code.size()) {
        if (code_type_ == CodeType::MachineCode) {
            // 如果是机器码，分配内存并复制代码
            machine_code_len_ = code.size() * sizeof(int);
            machine_code_addr_ = new char[machine_code_len_];
            std::memcpy(machine_code_addr_, code.data(), machine_code_len_);
        }
    }

    /**
     * 析构函数
     */
    ~LmCodeObject() override {
        if (code_type_ == CodeType::MachineCode && machine_code_addr_) {
            delete[] static_cast<char*>(machine_code_addr_);
        }
    }

    /**
     * 获取代码类型
     * @return CodeType
     */
    [[nodiscard]] CodeType get_code_type() const { return code_type_; }

    /**
     * 获取字节码
     * @return std::vector<int>&
     */
    [[nodiscard]] const std::vector<int>& get_bytecode() const;

    /**
     * 获取机器码
     * @return void*
     */
    [[nodiscard]] void* get_machine_code() const;

    /**
     * 获取机器码长度
     * @return size_t
     */
    [[nodiscard]] size_t get_machine_code_len() const;
};

class LmBigint : public LmHeapObject {
public:
    // 每个 uint32_t 存32位数据
    LmBigint(const std::vector<uint64_t>& vals, bool is_negative)
        : LmHeapObject(HeapObjType::Bigint),
          vals_(vals),
          is_negative_(is_negative),
          bit_len_(vals.size() * 32) {}  // 缓存位长

    [[nodiscard]] const std::vector<uint64_t>& get_vals() const { return vals_; }

    [[nodiscard]] bool is_neg() const { return is_negative_; }

    [[nodiscard]] size_t get_len() const { return bit_len_; }
private:
    std::vector<uint64_t> vals_;  // 使用 uint32_t 替代 int
    bool is_negative_;
    size_t bit_len_;
};

class LmArray : public LmHeapObject {
public:
    /**
     * 构造函数初始化
     * @param initial_cap
     */
    explicit LmArray(size_t initial_cap = 4)  // 预分配小容量
        : LmHeapObject(HeapObjType::Array),
          size_(0),
          capacity_(initial_cap) {
        vals_.reserve(capacity_);
    }

    /**
     * 添加元素，扩容X2
     * @param val
     * @return void
     */
    void push(TaggedVal val);

    /**
     * 获取元素
     * @param idx
     * @return TaggedVal
     */
    [[nodiscard]] TaggedVal get(size_t idx) const;

    /**
     * Override
     */
    ~LmArray() override {
        for (TaggedVal val : vals_) {
            if (TaggedUtil::is_HeapObject(val)) {
                TaggedUtil::decode_HeapObject(val)->del_ref();
            }
        }
    }

    /**
     * 获取数组大小
     * @return size_t
     */
    [[nodiscard]] size_t get_size() const { return size_; }
private:
    std::vector<TaggedVal> vals_; // 指向一个TaggedVal的Vector容器
    size_t size_; // 大小
    size_t capacity_; // 容量
};

class LmWeakRef : public LmHeapObject {
public:
    /**
     * 构造函数，初始化WeakRef
     * @param obj
     */
    explicit LmWeakRef(LmHeapObject* obj)
        : LmHeapObject(HeapObjType::WeakRef),
          target_(obj) {}

    /**
     * 析构函数
     * 释放
     */
    ~LmWeakRef() override = default;

    /**
     * 获取目标
     * @return LmHeapObject*
     */
    [[nodiscard]] LmHeapObject* get_target() const { return target_; }

    /**
     * 标记目标失衡！(失效)
     * @return void
     */
    void invalidate() { target_ = nullptr; }
private:
    LmHeapObject* target_; // 指向目标对象
};