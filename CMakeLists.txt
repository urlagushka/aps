cmake_minimum_required(VERSION 3.10)

project(server_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
    main.cpp
    
    src/http/server.cpp
)
