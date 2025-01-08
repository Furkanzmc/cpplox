cmake_minimum_required(VERSION 3.19)

project(
  lox++
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_OSX_ARCHITECTURES
    "arm64;x86_64"
    CACHE STRING "" FORCE)
option(LOX_BUILD_TESTS OFF "Set to on to build tests.")

set(LOX_PP_SOURCES src/scanner.cpp src/token.cpp src/ast_printer.cpp
                   src/parser.cpp src/interpreter.cpp src/environment.cpp)
set(PROJECT_SOURCES src/main.cpp)

if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  set(LOX_BUILD_FALGS -Wall -Wextra -Wpedantic-core-features -Werror)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(LOX_BUILD_FALGS -Wall -Wextra -pedantic -Werror)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(LOX_BUILD_FALGS /W4 /WX)
endif()

add_library(lox++ STATIC ${LOX_PP_SOURCES})
add_library(lox++::lox++ ALIAS lox++)

add_executable(lox_cli ${PROJECT_SOURCES})

add_subdirectory(./zmcx)

target_compile_options(lox++ PRIVATE ${LOX_BUILD_FALGS})
target_compile_options(lox_cli PRIVATE ${LOX_BUILD_FALGS})

target_link_libraries(lox++ PRIVATE zmcx::zmcx)
target_link_libraries(lox_cli PRIVATE lox++::lox++ zmcx::zmcx)

find_package(Python3 COMPONENTS Interpreter)
if(Python3_FOUND)
  execute_process(
    COMMAND ${Python3_EXECUTABLE}
            ${PROJECT_SOURCE_DIR}/scripts/generate_expressions.py
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_QUIET COMMAND_ECHO NONE)
else()
  message(FATAL_ERROR "Python executable is not found.")
endif(Python3_FOUND)

add_custom_target(
  run
  COMMAND "./lox++"
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Run the executable.")

if(LOX_BUILD_TESTS)
  add_subdirectory(test/)
endif()
