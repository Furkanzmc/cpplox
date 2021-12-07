cmake_minimum_required(VERSION 3.19)

project(
  lox++
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT_SOURCES src/main.cpp src/scanner.cpp src/log.cpp)

add_executable(lox++ ${PROJECT_SOURCES})

if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  target_compile_options(lox++ PRIVATE -Wall -Wextra -pedantic -Werror)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(lox++ PRIVATE -Wall -Wextra -pedantic -Werror)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(lox++ PRIVATE /W4 /WX)
  target_compile_definitions(lox++ PUBLIC -D_WIN32 -D_WIN32_WINNT=0x0A00)
endif()

add_custom_target(
  run
  COMMAND "./lox++"
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Run the executable.")
