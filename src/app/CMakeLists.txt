add_library(snake_app STATIC
  AppShell.cpp
  GamePage.cpp
  InfoBar.cpp
  LeaderboardWidget.cpp
  MainMenuWidget.cpp
  PauseWidget.cpp
  GameOverWidget.cpp
  SettingsWidget.cpp
)
target_include_directories(snake_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(snake_app PUBLIC snake_controller snake_ui snake_audio snake_logging Qt5::Widgets)
