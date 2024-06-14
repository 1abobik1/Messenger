cmake_minimum_required(VERSION 3.20)
project(MessengerServer VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

include(FetchContent)

# --- nlohmann/json --------------------------------------------------------
find_package(nlohmann_json 3.11 QUIET)
if(NOT nlohmann_json_FOUND)
    FetchContent_Declare(nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
        DOWNLOAD_EXTRACT_TIMESTAMP ON)
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# --- uWebSockets + uSockets (no SSL, no compression) ----------------------
FetchContent_Declare(uwebsockets
    GIT_REPOSITORY https://github.com/uNetworking/uWebSockets.git
    GIT_TAG v20.80.0
    GIT_SHALLOW ON
    GIT_SUBMODULES uSockets)
FetchContent_MakeAvailable(uwebsockets)

file(GLOB USOCKETS_SOURCES
    ${uwebsockets_SOURCE_DIR}/uSockets/src/*.c
    ${uwebsockets_SOURCE_DIR}/uSockets/src/eventing/*.c
    ${uwebsockets_SOURCE_DIR}/uSockets/src/crypto/*.c)
add_library(usockets STATIC ${USOCKETS_SOURCES})
target_include_directories(usockets PUBLIC ${uwebsockets_SOURCE_DIR}/uSockets/src)
target_compile_definitions(usockets PUBLIC LIBUS_NO_SSL)
set_target_properties(usockets PROPERTIES C_STANDARD 11)
if(WIN32)
    target_link_libraries(usockets PUBLIC ws2_32)
endif()

add_library(uwebsockets INTERFACE)
# The code includes <uwebsockets/App.h>, as vcpkg lays it out.
set(UWS_INCLUDE_ROOT ${CMAKE_BINARY_DIR}/uws_include)
file(MAKE_DIRECTORY ${UWS_INCLUDE_ROOT})
file(COPY ${uwebsockets_SOURCE_DIR}/src/ DESTINATION ${UWS_INCLUDE_ROOT}/uwebsockets)
target_include_directories(uwebsockets SYSTEM INTERFACE ${UWS_INCLUDE_ROOT} ${UWS_INCLUDE_ROOT}/uwebsockets)
target_compile_definitions(uwebsockets INTERFACE UWS_NO_ZLIB)
target_link_libraries(uwebsockets INTERFACE usockets)

# --- PostgreSQL client ------------------------------------------------------
find_package(PostgreSQL REQUIRED)

# --- bcrypt (vendored) -------------------------------------------------------
add_library(bcrypt STATIC third_party/bcrypt/src/bcrypt.cpp third_party/bcrypt/src/blowfish.cpp)
target_include_directories(bcrypt PUBLIC third_party/bcrypt/include PRIVATE third_party/bcrypt/src)

# --- server --------------------------------------------------------------------
add_executable(messenger-server
    src/main.cpp
    src/api.cpp
    src/auth_service.cpp
    src/chat.cpp
    src/config.cpp
    src/db/connection.cpp
    src/db/friend_repository.cpp
    src/db/message_repository.cpp
    src/db/session_repository.cpp
    src/db/user_repository.cpp
    src/http.cpp
    src/log.cpp
    src/rate_limiter.cpp
    src/server.cpp
    src/sha256.cpp
    src/validation.cpp
)
target_include_directories(messenger-server PRIVATE include)
target_link_libraries(messenger-server PRIVATE uwebsockets nlohmann_json::nlohmann_json PostgreSQL::PostgreSQL bcrypt)

if(MSVC)
    target_compile_options(messenger-server PRIVATE /W4 /utf-8 /permissive-)
    target_compile_definitions(messenger-server PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX)
else()
    target_compile_options(messenger-server PRIVATE -Wall -Wextra -Wpedantic)
endif()
