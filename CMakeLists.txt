cmake_minimum_required(VERSION 3.12)
project(compiler)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(./src/lexer)
add_subdirectory(./src/generator)
add_subdirectory(./src/parser)

add_executable(execute ./src/main.c)

target_link_libraries(execute lexer parser generator)

include(CTest)
if(BUILD_TESTING)
  find_package(Python3 COMPONENTS Interpreter REQUIRED)
  add_test(NAME compiler COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/run.py --compiler $<TARGET_FILE:execute>)
endif()
