add_library(snake_controller STATIC
  GameController.cpp
  components/InputComponent.cpp
  components/MoveComponent.cpp
  components/CollisionComponent.cpp
  components/FoodComponent.cpp
  components/RenderComponent.cpp
)
target_include_directories(snake_controller PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(snake_controller PUBLIC snake_core snake_ui snake_logging Qt5::Widgets)
