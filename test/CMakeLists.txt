set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.0.0-preview3)

FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

set(TEST_LIBRARIES Catch2::Catch2WithMain lox++::lox++)
set(TEST_INCLUDE_DIRS ../src/)

if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  set(TEST_COMPILER_FLAGS "-Wall -Wextra -Werror -Wpedantic-core-features")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(TEST_COMPILER_FLAGS "-Wall -Wextra -pedantic -Werror")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(TEST_COMPILER_FLAGS "/W4 /WX")
endif()

function(lox_add_tests test_name)
  add_executable(test_${test_name} ${test_name}.cpp)
  target_compile_options(test_${test_name} PRIVATE ${TEST_COMPILER_FLAGS})
  catch_discover_tests(test_${test_name} REPORTER)
  target_link_libraries(test_${test_name} PRIVATE ${TEST_LIBRARIES})
  target_include_directories(test_${test_name} PRIVATE ${TEST_INCLUDE_DIRS})
endfunction()

lox_add_tests(scanner)
lox_add_tests(parser)
