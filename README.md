-Common:
định nghĩa giao thức
hỗ trợ parse TCP stream
-packet_types:
  định nghĩa các loại packet trong system
-packet_header:
  mô tả metadata của mỗi packet
  cho phép parse TCP stream thành packet
-bytebuffer:
  gom dữ liệu từ nhiều lần recv
  giải quyết bài toán TCP là stream, không phải packet
  append: thêm dữ liệu mới vào buffer, dùng sau mỗi  recv
  can_read: kiểm tra buffer có đủ byte để đọc không
  read: copy len byte ra ngoài, xóa khỏi buffer
  size: trả về số byte hiện có
  recv() → append()
        ↓
    ByteBuffer
        ↓
  try_parse_packet()

-packets: 
  khai báo các hàm tạo/parse packet
  là API duy nhất mà client và server dùng để giao tiếp
  make_packet: đóng gói packetHeader|payload
  try_parse_packet: trích packet từ TCP stream, trả về false nếu chưa đủ dữ liệu
