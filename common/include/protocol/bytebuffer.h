#pragma once
#include <vector>
#include <string>
#include <cstring>
#include <stdexcept>
#include <cstdint>

class ByteBuffer {
private:
    std::vector<uint8_t> buffer;
    size_t readPos = 0;

public:
    ByteBuffer() = default;
    explicit ByteBuffer(size_t size) { buffer.reserve(size); }

    // ===== WRITE =====
    template<typename T>
    void write(const T& value) {
        size_t pos = buffer.size();
        buffer.resize(pos + sizeof(T));
        std::memcpy(buffer.data() + pos, &value, sizeof(T));
    }

    void writeString(const std::string& str) {
        uint32_t len = str.size();
        write(len);
        buffer.insert(buffer.end(), str.begin(), str.end());
    }

    void append(const char* data, size_t len) {
        buffer.insert(buffer.end(), data, data + len);
    }

    // ===== READ =====
    template<typename T>
    T read() {
        if (readPos + sizeof(T) > buffer.size())
            throw std::runtime_error("ByteBuffer read overflow");
        T value;
        std::memcpy(&value, buffer.data() + readPos, sizeof(T));
        readPos += sizeof(T);
        return value;
    }

    std::string readString() {
        uint32_t len = read<uint32_t>();
        if (readPos + len > buffer.size())
            throw std::runtime_error("ByteBuffer string overflow");
        std::string str(reinterpret_cast<char*>(buffer.data() + readPos), len);
        readPos += len;
        return str;
    }

    // ===== UTIL =====
    void clear() {
        buffer.clear();
        readPos = 0;
    }

    size_t size() const { return buffer.size(); }
    size_t remaining() const { return buffer.size() - readPos; }
    const uint8_t* data() const { return buffer.data(); }
    uint8_t* data() { return buffer.data(); }
};
