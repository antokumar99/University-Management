cmake_minimum_required(VERSION 3.16)
project(UniversityManagementSystem VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql PrintSupport)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.cpp
    src/*.h
)

add_executable(UniversityManagementSystem ${SOURCES})

target_include_directories(UniversityManagementSystem PRIVATE
    src
    src/core
    src/dao
    src/models
    src/services
    src/ui
)

target_link_libraries(UniversityManagementSystem PRIVATE
    Qt6::Widgets
    Qt6::Sql
    Qt6::PrintSupport
)
