add_library(snake_core STATIC
  Snake.cpp
  Board.cpp
  RngService.h
)
target_include_directories(snake_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
