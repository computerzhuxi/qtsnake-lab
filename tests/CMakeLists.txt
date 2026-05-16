add_executable(test_core
  test_board.cpp
  test_snake.cpp
)
target_link_libraries(test_core PRIVATE snake_core gtest_main)
add_test(NAME test_core COMMAND test_core)
