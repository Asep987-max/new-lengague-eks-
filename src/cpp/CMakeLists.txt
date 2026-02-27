cmake_minimum_required(VERSION 3.12)
project(executor_core CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# Source files
set(SOURCES
    src/executor_core.cpp
    src/lexer.cpp
    src/parser.cpp
)

# Executable
add_executable(executor_core ${SOURCES})

# Testing (Catch2)
enable_testing()
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.5.2
)
FetchContent_MakeAvailable(Catch2)

add_executable(executor_tests tests/test_lexer.cpp tests/test_parser.cpp tests/test_quoting.cpp src/lexer.cpp src/parser.cpp)
target_link_libraries(executor_tests PRIVATE Catch2::Catch2WithMain)

include(GoogleTest) # Optional, if we switch frameworks
