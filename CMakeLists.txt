cmake_minimum_required(VERSION 3.25)
project(ChatServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    "main.cpp"
    "source/Server.cpp"
    "source/MessagerHandler.cpp"
    "source/RequestHandler.cpp"
    "source/DataBase.cpp"
    "bcrypt-cpp/src/bcrypt.cpp"
    "bcrypt-cpp/src/blowfish.cpp"
	"source/UserModel.cpp"
    "source/TimeUtils.cpp"
)

set(HEADERS
    "header/CommonConst.h"
    "header/Server.h"
    "header/MessagerHandler.h"
    "header/RequestHandler.h"
    "header/DataBase.h"
    "bcrypt-cpp/include/bcrypt.h"
    "bcrypt-cpp/src/node_blf.h"
    "bcrypt-cpp/src/openbsd.h"
    "header/UserModel.h"
    "header/WebSocketUser.h"
    "header/TimeUtils.h"
)

# Пути к заголовочным файлам uwebsockets и nlohmann-json
set(UWEBSOCKETS_INCLUDE_DIRS "C:/vcpkg/vcpkg-master/installed/x64-windows/include")
set(NLOHMANN_JSON_INCLUDE_DIRS "C:/vcpkg/vcpkg-master/installed/x64-windows/include")

if (UWEBSOCKETS_INCLUDE_DIRS)
    message("Found uwebsockets: ${UWEBSOCKETS_INCLUDE_DIRS}")
    include_directories(${UWEBSOCKETS_INCLUDE_DIRS})
endif()

if (NLOHMANN_JSON_INCLUDE_DIRS)
    message("Found nlohmann_json: ${NLOHMANN_JSON_INCLUDE_DIRS}")
    include_directories(${NLOHMANN_JSON_INCLUDE_DIRS})
endif()

find_package(PostgreSQL REQUIRED)
if (PostgreSQL_FOUND)
    message("Found PostgreSQL")
    include_directories(${PostgreSQL_INCLUDE_DIRS})
endif()

add_executable(ChatServer ${SOURCES} ${HEADERS})

if (PostgreSQL_FOUND)
    target_link_libraries(ChatServer PRIVATE ${PostgreSQL_LIBRARIES})
endif()