cmake_minimum_required(VERSION 3.10)

project(cv_service LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
    main.cpp
    
    src/http/structs.cpp
    src/http/server.cpp

    src/request/curl_handler.cpp
)

find_package(CURL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)
