add_library(snake_audio STATIC
  AudioManager.cpp
)
target_include_directories(snake_audio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
