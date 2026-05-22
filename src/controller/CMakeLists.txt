add_library(snake_controller STATIC
  GameController.cpp
  InputComponent.cpp
  MoveComponent.cpp
  CollisionComponent.cpp
  RenderComponent.cpp
  FoodSpawner.cpp
  RngService.h
)
target_include_directories(snake_controller PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(snake_controller PUBLIC snake_core snake_ui snake_logging Qt5::Widgets)
