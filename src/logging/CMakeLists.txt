add_library(snake_logging STATIC
  Logger.cpp
)
target_include_directories(snake_logging PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
