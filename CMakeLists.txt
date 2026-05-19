cmake_minimum_required(VERSION 3.16)
project(SnakeArena VERSION 1.0.0 LANGUAGES CXX)

if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(MSVC)
  add_compile_options(/utf-8)
endif()

find_package(Qt5 REQUIRED COMPONENTS Widgets Network LinguistTools)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)
if(MSVC)
  target_compile_options(gtest PRIVATE /W0)
  target_compile_options(gtest_main PRIVATE /W0)
else()
  target_compile_options(gtest PRIVATE -w)
  target_compile_options(gtest_main PRIVATE -w)
endif()

enable_testing()
add_subdirectory(src/core)
add_subdirectory(src/logging)
add_subdirectory(src/ui)
add_subdirectory(src/controller)
add_subdirectory(src/audio)
add_subdirectory(src/app)
add_subdirectory(tests)

set(RESOURCE_FILE src/resources/resources.qrc)

add_executable(SnakeArena
    src/main.cpp
    ${RESOURCE_FILE}
)
target_link_libraries(SnakeArena PRIVATE snake_app snake_controller snake_ui snake_audio snake_logging Qt5::Widgets)

add_executable(SnakeServer src/main_server.cpp)
target_link_libraries(SnakeServer PRIVATE snake_core snake_logging Qt5::Core)
