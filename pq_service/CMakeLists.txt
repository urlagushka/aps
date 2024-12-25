cmake_minimum_required(VERSION 3.10)

project(pg_service LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
    main.cpp
    
    src/http/structs.cpp
    src/http/server.cpp

    src/request/curl_handler.cpp

    src/data/client.cpp
    src/data/file.cpp
    src/data/request.cpp
)
