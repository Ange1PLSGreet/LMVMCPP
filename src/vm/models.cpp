/****************************************************** 
-     Date:  2025.08.28 01:37
-     File:  models.cpp
-     CopyRight Lamina Team
-     This project is followed GPL-3.0 license
********************************************************/
#include "models.hpp"
#include <cassert>

TaggedVal TaggedUtil::encode_Smi(int64_t smi_val) {
    return (static_cast<TaggedVal>(smi_val) << 3) | static_cast<TaggedVal>(TaggedType::Smi);
}

TaggedVal TaggedUtil::encode_HeapObject(LmHeapObject *obj) {
    assert(reinterpret_cast<TaggedVal>(obj) % 8 == 0);
    return reinterpret_cast<TaggedVal>(obj) | static_cast<TaggedVal>(TaggedType::HeapObject);
}

TaggedType TaggedUtil::get_tagged_type(TaggedVal val){
    return static_cast<TaggedType>(val & 0b111);  // 取低3位
}

int64_t TaggedUtil::decode_Smi(TaggedVal val) {
    assert(get_tagged_type(val) == TaggedType::Smi);
    return static_cast<int64_t>(val) >> 3;
}

LmHeapObject* TaggedUtil::decode_HeapObject(TaggedVal val) {
    assert(get_tagged_type(val) == TaggedType::HeapObject);
    return reinterpret_cast<LmHeapObject*>(val & ~0b111ULL);
}

bool TaggedUtil::is_HeapObject(TaggedVal val) {
    return get_tagged_type(val) == TaggedType::HeapObject;
}

void LmHeapObject::make_ref() {
    ref_count_++;
}

void LmHeapObject::del_ref() {
    ref_count_ --;
    if (ref_count_ == 0) delete this;
}

bool LmString::equals(const LmString *other) const {
    if (other == nullptr) return false;
    return (byte_length_ == other->byte_len())
           && (std::strcmp(utf8_data_, other->utf8_data_) == 0);
}

LmString *LmString::concat(const LmString *other) const {
    if (other == nullptr) return new LmString(utf8_data_);

    size_t new_byte_len = byte_length_ + other->byte_len();
    char* new_utf8 = new char[new_byte_len + 1];

    std::strcpy(new_utf8, utf8_data_);
    std::strcat(new_utf8, other->utf8_data_);

    auto* result = new LmString(new_utf8);
    delete[] new_utf8;

    return result;
}

size_t LmString::calc_utf8_char_count(const char *utf8_str, size_t byte_len) {
    if (byte_len == 0) return 0;
    size_t char_count = 0;
    const auto* p = reinterpret_cast<const unsigned char*>(utf8_str);
    const unsigned char* end = p + byte_len;

    while (p < end) {
        int step = 1;
        // UTF-8 编码规则：
        // '.' 代表 'x'
        // - 单字节：0x......（最高位0）
        // - 双字节：110..... 10xxxxxx
        // - 三字节：1110xxxx 10xxxxxx 10xxxxxx
        // - 四字节：11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        if ((*p & 0x80) == 0) step = 1; // 单字节字符
        else if ((*p & 0xE0) == 0xC0) step = 2; // 双字节字符
        else if ((*p & 0xF0) == 0xE0) step = 3; // 三字节字符（中文、日文等）
        else if ((*p & 0xF8) == 0xF0) step = 4; // 四字节字符（ emoji 等）
        else p += 1; // 容错
        p += step;
        char_count++;
    }
    return char_count;
}

const std::vector<int> &LmCodeObject::get_bytecode() const {
    assert(code_type_ == CodeType::Bytecode);
    return code_;
}

void *LmCodeObject::get_machine_code() const {
    assert(code_type_ == CodeType::MachineCode);
    return machine_code_addr_;
}

size_t LmCodeObject::get_machine_code_len() const {
    assert(code_type_ == CodeType::MachineCode);
    return machine_code_len_;
}

void LmArray::push(TaggedVal val) {
    if (size_ >= capacity_) {
        capacity_ *= 2;
        vals_.reserve(capacity_);
    }
    if (TaggedUtil::is_HeapObject(val)) {
        TaggedUtil::decode_HeapObject(val)->make_ref();
    }
    vals_.push_back(val);
    size_++;
}

TaggedVal LmArray::get(size_t idx) const {
    assert(idx < size_);
    return vals_[idx];
}