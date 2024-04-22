cmake_minimum_required(VERSION 3.0)
project(ChatServer)

# Установка стандарта языка
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавление исходных файлов
set(SOURCES
    "main.cpp"
    "source/FileSender.cpp"
    "source/Server.cpp"
    "source/MessagerHandler.cpp"
    "source/RequestHandler.cpp"
    "source/DataBase.cpp"
)

# Добавление заголовочных файлов
set(HEADERS
    "header/CommonConst.h"
    "header/Server.h"
    "header/FileSender.h"
    "header/MessagerHandler.h"
    "header/RequestHandler.h"
    "header/DataBase.h"
)

# Найдем uwebsockets и установим пути к его заголовочным файлам
find_path(UWEBSOCKETS_INCLUDE_DIRS "uwebsockets/App.h")
if (UWEBSOCKETS_INCLUDE_DIRS)
    message("Found uwebsockets: ${UWEBSOCKETS_INCLUDE_DIRS}")
    include_directories(${UWEBSOCKETS_INCLUDE_DIRS})
endif()

# Найдем nlohmann-json и подключим его
find_package(nlohmann_json CONFIG REQUIRED)
if (nlohmann_json_FOUND)
    message("Found nlohmann_json: ${nlohmann_json_INCLUDE_DIRS}")
    include_directories(${nlohmann_json_INCLUDE_DIRS})
    target_link_libraries(main PRIVATE nlohmann_json::nlohmann_json)
endif()

# Найдем libpq (PostgreSQL) и подключим его
find_package(PostgreSQL REQUIRED)
if (PostgreSQL_FOUND)
    message("Found PostgreSQL")
    include_directories(${PostgreSQL_INCLUDE_DIRS})
    target_link_libraries(main PRIVATE ${PostgreSQL_LIBRARIES})
endif()

# Добавление исполняемого файла
add_executable(ChatServer ${SOURCES} ${HEADERS})
