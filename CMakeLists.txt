cmake_minimum_required(VERSION 3.10)
project(TransferFileNetworkP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =========================
# COMMON (protocol)
# =========================
include_directories(
    ${PROJECT_SOURCE_DIR}/common/include
    ${PROJECT_SOURCE_DIR}/server/include
    ${PROJECT_SOURCE_DIR}/client/include
)

file(GLOB COMMON_SRC
    common/src/*.cpp
)

add_library(common STATIC ${COMMON_SRC})

# =========================
# SERVER
# =========================
file(GLOB SERVER_SRC
    server/src/*.cpp
)

add_executable(server ${SERVER_SRC})

target_link_libraries(server
    common
    pthread
)

# =========================
# CLIENT
# =========================
file(GLOB CLIENT_SRC
    client/src/*.cpp
)

add_executable(client ${CLIENT_SRC})

target_link_libraries(client
    common
)

# =========================
# BUILD MESSAGE
# =========================
message(STATUS "Build server and client successfully")
