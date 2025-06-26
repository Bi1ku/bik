cmake_minimum_required(VERSION 3.10)
project(compiler)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(./src/lexer)
add_subdirectory(./src/generator)
add_subdirectory(./src/parser)

add_executable(execute ./src/main.c)

target_link_libraries(execute lexer parser generator)
