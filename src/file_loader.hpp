/******************************************************
-     Date:  2025.08.28 15:35
-     File:  file_loader.hpp
-     CopyRight Lamina Team
-     This project is followed GPL-3.0 license
********************************************************/
#pragma once
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

class FileLoader{
public:

    // 文件头结构
    struct FileHeader {
        uint32_t magic = MAGIC_NUMBER;      // 魔数 "QTLM"
        uint32_t version = CURRENT_VERSION; // 版本号
        uint64_t codeSize = 0;              // 代码段长度
        uint64_t dataSize = 0;              // 数据段长度
        uint64_t symbolTableSize = 0;       // 符号表长度
        uint64_t codeNum = 0;               // 代码段指令数量
    };

    // 完整文件结构
    struct FileData {
        FileHeader header;
        std::vector<uint8_t> codeSegment;
        std::vector<uint8_t> dataSegment;
        std::vector<uint8_t> symbolTableSegment;
    };

    /**
    * 加载完整的文件数据（包括所有段）
    * @param filename
    * @return FileData
    */
    static FileData loadFullFileData(const std::string &filename);

    /**
    * 读取文件二进制头
    * @param file
    * @return FileHeader
    */
    static FileHeader readFileHeader(std::ifstream &file);

    /**
     * 验证文件头
     * @param header
     * @return Boolean
     */
    static bool validateHeader(const FileHeader &header);

    /**
     * 写入文件二进制头
     * @param file
     * @param codeSize
     * @param codeNum
     * @param dataSize
     * @param symbolTableSize
     * @return void
     */
    static void writeFileHeader(std::ofstream &file, uint64_t codeSize = 0, uint64_t codeNum = 0, uint64_t dataSize = 0, uint64_t symbolTableSize = 0);
private:
    // 魔数定义
    static constexpr uint32_t MAGIC_NUMBER = 0x4D4C5451; // "QTLM"这个字符串的小端序

    // 当前版本号
    static constexpr uint32_t CURRENT_VERSION = 1;
};