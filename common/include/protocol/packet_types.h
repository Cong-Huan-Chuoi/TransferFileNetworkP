#pragma once
#include <cstdint>

enum class PacketType : uint16_t {

  /* ==================== AUTH ==================== */
  AUTH_REGISTER_REQ = 1,
  AUTH_REGISTER_RES = 2,

  AUTH_LOGIN_REQ    = 3,
  AUTH_LOGIN_RES    = 4,

  AUTH_LOGOUT_REQ   = 5,
  AUTH_LOGOUT_RES   = 6,


  /* ==================== GROUP ==================== */
  GROUP_CREATE_REQ        = 10,
  GROUP_CREATE_RES        = 11,

  GROUP_JOIN_REQ          = 12,
  GROUP_JOIN_RES          = 13,

  GROUP_APPROVE_REQ       = 14,
  GROUP_APPROVE_RES       = 15,

  GROUP_INVITE_REQ        = 16,
  GROUP_INVITE_RES        = 17,

  GROUP_ACCEPT_INVITE_REQ = 18,
  GROUP_ACCEPT_INVITE_RES = 19,

  GROUP_LEAVE_REQ         = 20,
  GROUP_LEAVE_RES         = 21,

  GROUP_KICK_REQ          = 22,
  GROUP_KICK_RES          = 23,

  GROUP_LIST_MEMBERS_REQ  = 24,
  GROUP_LIST_MEMBERS_RES  = 25,

  GROUP_LIST_REQ          = 26,
  GROUP_LIST_RES          = 27,


  /* ================= FILE SYSTEM (CONTROL) ================= */
  FILE_LIST_REQ   = 40,
  FILE_LIST_RES   = 41,

  FILE_MKDIR_REQ  = 42,
  FILE_MKDIR_RES  = 43,

  FILE_DELETE_REQ = 44,
  FILE_DELETE_RES = 45,

  FILE_RENAME_REQ = 46,
  FILE_RENAME_RES = 47,

  FILE_COPY_REQ   = 48,
  FILE_COPY_RES   = 49,

  FILE_MOVE_REQ   = 50,
  FILE_MOVE_RES   = 51,


  /* ================= FILE TRANSFER (STREAMING) ================= */
  FILE_UPLOAD_BEGIN = 60,
  FILE_UPLOAD_CHUNK = 61,
  FILE_UPLOAD_END   = 62,
  FILE_UPLOAD_RES   = 63,

  FILE_DOWNLOAD_BEGIN = 64,
  FILE_DOWNLOAD_CHUNK = 65,
  FILE_DOWNLOAD_END   = 66,
  FILE_DOWNLOAD_RES   = 67,


  /* ================= SYSTEM / ERROR ================= */
  ERROR_RES = 100
};
