add_library(snake_ui STATIC
  GameScene.cpp
  GameView.cpp
  SnakeItem.cpp
  FoodItem.cpp
)
target_include_directories(snake_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(snake_ui PUBLIC snake_core Qt5::Widgets)
