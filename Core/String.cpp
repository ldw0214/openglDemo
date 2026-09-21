#include "String.h"
#include <fstream>

using namespace MiniEngine;
bool MiniEngine::String::empty() const {
	return m_size == 0;
}

MiniEngine::String::String(std::ifstream& file)
{
    // 防御：若流未打开，返回空串
    if (!file.is_open()) {
        m_data = nullptr;
        m_size = 0;
        return;
    }

    // 获取文件大小（此时应已处于二进制模式，否则 tellg 可能不准确）
    file.seekg(0, std::ios::end);
    std::streamsize rawSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // 分配缓冲区，读取原始字节
    char* rawData = new char[rawSize + 1];
    file.read(rawData, rawSize);
    rawData[rawSize] = '\0';

    // ---------- 处理 BOM 和换行符 ----------
    const char* p = rawData;
    std::streamsize len = rawSize;

    // 1. 跳过 UTF-8 BOM (EF BB BF)
    if (len >= 3 &&
        static_cast<unsigned char>(p[0]) == 0xEF &&
        static_cast<unsigned char>(p[1]) == 0xBB &&
        static_cast<unsigned char>(p[2]) == 0xBF) {
        p += 3;
        len -= 3;
    }

    // 2. 统一换行符：CRLF (\r\n) 或单独 CR (\r) -> LF (\n)
    std::string processed;
    processed.reserve(len);
    for (std::streamsize i = 0; i < len; ++i) {
        if (p[i] == '\r') {
            if (i + 1 < len && p[i + 1] == '\n') {
                // CRLF: 跳过 \r，\n 会在下一轮添加
                continue;
            }
            else {
                // 单独 CR: 转换为 LF
                processed += '\n';
            }
        }
        else {
            processed += p[i];
        }
    }

    // 3. 将处理好的字符串存入 String 对象
    m_size = processed.size();
    m_data = new char[m_size + 1];
    std::memcpy(m_data, processed.data(), m_size);
    m_data[m_size] = '\0';

    delete[] rawData;
}




