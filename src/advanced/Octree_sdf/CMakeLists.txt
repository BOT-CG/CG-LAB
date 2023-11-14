cmake_minimum_required(VERSION 3.16)

project(sdf VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mbmi2")
endif()

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(LINUX OR MINGW)
    find_package(TBB REQUIRED)
endif()

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_subdirectory(src)
