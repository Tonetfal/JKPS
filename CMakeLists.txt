cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)

set(COMPILE_FLAGS(-DCMAKE_CXX_FLAGS="-Wall -Wextra -Werror"))

project(JKPS CXX) 

file(GLOB PROJECT_SOURCE_DIR "Source")
include_directories(${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE SRC_SOURCES "Source/*.cpp")
file(GLOB_RECURSE SRC_HEADERS "Headers/*.hpp")

add_executable(${PROJECT_NAME} ${SRC_SOURCES} ${SRC_HEADERS})

target_compile_definitions(${PROJECT_NAME} PRIVATE COMPILE_FLAGS)

find_package(Threads)
find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED)

target_link_libraries(${PROJECT_NAME} sfml-system sfml-window sfml-graphics ${CMAKE_THREAD_LIBS_INIT})