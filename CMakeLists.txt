cmake_minimum_required(VERSION 3.16)
project(QtSnakeLab VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(BUILD_NETWORK "Build network multiplayer module" ON)
option(BUILD_TESTS "Build test suite" OFF)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

if(BUILD_TESTS)
    find_package(Qt5 REQUIRED COMPONENTS Test)
endif()

if(BUILD_NETWORK)
    find_package(Qt5 REQUIRED COMPONENTS Network)
endif()

add_subdirectory(src/core)
add_subdirectory(src/app)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ---- Install ----

install(TARGETS qtsnake_app RUNTIME DESTINATION bin)

# windeployqt for release builds
if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    install(CODE "
        set(QT_DEPLOY \"${CMAKE_INSTALL_PREFIX}/bin\")
        execute_process(
            COMMAND windeployqt --qmldir ${CMAKE_SOURCE_DIR}/src
                    ${CMAKE_INSTALL_PREFIX}/bin/qtsnake_app.exe
            WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin
        )
    ")
endif()

# ---- Packaging (NSIS) ----

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_NAME "QtSnakeLab")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "QtSnake Lab")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Snake game built with Qt5/C++")
set(CPACK_GENERATOR "NSIS")
set(CPACK_NSIS_DISPLAY_NAME "QtSnake Lab")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
set(CPACK_NSIS_CREATE_ICONS_EXTRA
    "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\QtSnake Lab.lnk' '$INSTDIR\\\\bin\\\\qtsnake_app.exe'")
set(CPACK_NSIS_DELETE_ICONS_EXTRA
    "Delete '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\QtSnake Lab.lnk'")
include(CPack)
