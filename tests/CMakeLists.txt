add_executable(test_core
  test_snake.cpp
  test_components.cpp
  test_game_controller.cpp
)
target_link_libraries(test_core PRIVATE snake_core snake_controller gtest_main)
add_test(NAME test_core COMMAND test_core)
