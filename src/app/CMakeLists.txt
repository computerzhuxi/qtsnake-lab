file(GLOB_RECURSE APP_SOURCES "src/*.cpp")
file(GLOB_RECURSE APP_HEADERS "include/*.h")

# Static library for linking into tests and executable
add_library(qtsnake_app_lib STATIC
    ${APP_SOURCES}
    ${APP_HEADERS}
)

target_include_directories(qtsnake_app_lib
    PUBLIC
        include
        ${CMAKE_SOURCE_DIR}/src/core/include
)

target_link_libraries(qtsnake_app_lib
    PUBLIC
        qtsnake_core
        Qt5::Widgets
)

if(BUILD_NETWORK)
    target_link_libraries(qtsnake_app_lib PUBLIC Qt5::Network)
    target_compile_definitions(qtsnake_app_lib PUBLIC BUILD_NETWORK)
endif()

# Thin executable
add_executable(qtsnake_app
    src/main.cpp
)

target_link_libraries(qtsnake_app
    PRIVATE
        qtsnake_app_lib
)
