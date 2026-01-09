cmake_minimum_required(VERSION 3.10)
project(TransferFileNetworkP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =========================
# INCLUDE DIRECTORIES
# =========================
include_directories(
    ${PROJECT_SOURCE_DIR}/common/include
    ${PROJECT_SOURCE_DIR}/server/include
    ${PROJECT_SOURCE_DIR}/client/include
)

# =========================
# COMMON (protocol)
# =========================
file(GLOB COMMON_SRC
    common/src/*.cpp
)

add_library(common STATIC ${COMMON_SRC})

# =========================
# SERVER
# =========================
file(GLOB SERVER_CORE_SRC
    server/src/*.cpp
)

file(GLOB SERVER_SERVICE_SRC
    server/src/services/*.cpp
)

add_executable(server
    ${SERVER_CORE_SRC}
    ${SERVER_SERVICE_SRC}
)

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
