cmake_minimum_required(VERSION 3.5)
project(ChatServer LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 17)

set(sources  
    "main.cpp"
    "source/ServiceFunc.cpp"
    "source/Server.cpp"
)
set(headers
    "header/CommonConst.h"
    "header/Server.h"
    "header/ServiceFunc.h"
)

set(VCPKG_INSTALLED_DIR "C:/vcpkg/vcpkg-master/installed/x64-windows")

find_path(UWEBSOCKETS_INCLUDE_DIRS NAMES "uwebsockets/App.h" PATHS "${VCPKG_INSTALLED_DIR}/include" NO_DEFAULT_PATH)
find_path(NLOHMANN_JSON_INCLUDE_DIRS NAMES "nlohmann/json.hpp" PATHS "${VCPKG_INSTALLED_DIR}/include" NO_DEFAULT_PATH)
find_package(PostgreSQL REQUIRED)

add_executable(ChatServer ${sources} ${headers})

target_include_directories(ChatServer PRIVATE ${UWEBSOCKETS_INCLUDE_DIRS} ${NLOHMANN_JSON_INCLUDE_DIRS} ${PostgreSQL_INCLUDE_DIRS})
