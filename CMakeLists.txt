cmake_minimum_required(VERSION 3.19)

project(
  lox++
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LOX_PP_SOURCES src/scanner.cpp src/token.cpp src/ast_printer.cpp)
set(PROJECT_SOURCES src/main.cpp)

add_library(lox++ STATIC ${LOX_PP_SOURCES})
add_library(lox++::lox++ ALIAS lox++)

add_executable(lox_cli ${PROJECT_SOURCES})

if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  target_compile_options(lox++ PRIVATE -Wall -Wextra -Wpedantic-core-features
                                       -Werror)
  target_compile_options(lox_cli PRIVATE -Wall -Wextra -Wpedantic-core-features
                                         -Werror)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(lox++ PRIVATE -Wall -Wextra -pedantic -Werror)
  target_compile_options(lox_cli PRIVATE -Wall -Wextra -pedantic -Werror)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(lox++ PRIVATE /W4 /WX)
  target_compile_options(lox_cli PRIVATE /W4 /WX)
endif()

target_link_libraries(lox_cli PRIVATE lox++::lox++)

find_package(Python3 COMPONENTS Interpreter)
if(Python3_FOUND)
  execute_process(
    COMMAND ${Python3_EXECUTABLE}
            ${PROJECT_SOURCE_DIR}/scripts/generate_expressions.py
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_QUIET COMMAND_ECHO NONE)
else()
  message(FATAL "Python executable is not found.")
endif(Python3_FOUND)

add_custom_target(
  run
  COMMAND "./lox++"
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Run the executable.")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_subdirectory(test/)
endif()
