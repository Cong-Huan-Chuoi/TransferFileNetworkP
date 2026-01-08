#pragma once
#include <vector>
#include <cstdint>
#include <cstring>

class ByteBuffer {
public:
    void append(const uint8_t* data, size_t len) {
        buf.insert(buf.end(), data, data + len);
    }

    bool can_read(size_t len) const {
        return buf.size() >= len;
    }

    bool read(void* out, size_t len) {
        if (!can_read(len)) return false;
        std::memcpy(out, buf.data(), len);
        buf.erase(buf.begin(), buf.begin() + len);
        return true;
    }

    size_t size() const { return buf.size(); }

    bool peek(void* out, size_t len) const {
    if (buf.size() < len) return false;
    std::memcpy(out, buf.data(), len);
    return true;
    }

private:
    std::vector<uint8_t> buf;
};
