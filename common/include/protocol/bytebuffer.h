#pragma once
#include <vector>
#include <cstdint>
#include <cstring>

/*
 * ByteBuffer:
 * - Gom dữ liệu recv() từ TCP stream
 * - Chỉ đọc khi đủ byte
 */

 class ByteBuffer{
    private:
        std::vector<uint8_t> buffer;
    public:
        void append(const uint8_t* data, size_t len){
            buffer.insert(buffer.end(), data, data + len);
        }

        bool read(void* out, size_t len){
            if(buffer.size() < len) 
                return false;
            std::memcpy(out, buffer.data(), len);
            buffer.erase(buffer.begin(), buffer.begin() + len);
            return true;
        }

        size_t size() const{
            return buffer.size();
        }

 };