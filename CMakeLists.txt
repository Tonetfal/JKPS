cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)

set(PROJECT_NAME "KPS")
project(${PROJECT_NAME}) 

file(GLOB PROJECT_SOURCE_DIR "Source")
include_directories(${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE SRC_SOURCES "Source/*.cpp")
file(GLOB_RECURSE SRC_HEADERS "Headers/*.hpp")

add_executable(${PROJECT_NAME} ${SRC_SOURCES} ${SRC_HEADERS})

# target_precompile_headers(${PROJECT_NAME} PUBLIC "Headers/DefaultFiles.hpp")

find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED)
target_link_libraries(${PROJECT_NAME} sfml-graphics)
