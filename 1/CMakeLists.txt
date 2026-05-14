cmake_minimum_required(VERSION 3.10)
project(telecom LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(telecom
    src/main.cpp
    src/parser.cpp
    src/io.cpp
    src/bot.cpp
)
