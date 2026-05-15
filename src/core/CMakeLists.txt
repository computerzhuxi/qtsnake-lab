file(GLOB_RECURSE CORE_SOURCES "src/*.cpp")

add_library(qtsnake_core STATIC ${CORE_SOURCES})

target_include_directories(qtsnake_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        src
)

target_compile_features(qtsnake_core PUBLIC cxx_std_17)
