find_package(Qt5 REQUIRED COMPONENTS Test)

# Qt Test based test
function(add_core_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_include_directories(${TEST_NAME}
        PRIVATE
            ${CMAKE_SOURCE_DIR}/src/core/include
    )
    target_link_libraries(${TEST_NAME} qtsnake_core Qt5::Test)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

# Plain C++ test (no Qt dependency)
function(add_plain_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_include_directories(${TEST_NAME}
        PRIVATE
            ${CMAKE_SOURCE_DIR}/src/core/include
    )
    target_link_libraries(${TEST_NAME} qtsnake_core)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

# App-layer test (links app includes + Qt5::Core)
function(add_app_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_include_directories(${TEST_NAME}
        PRIVATE
            ${CMAKE_SOURCE_DIR}/src/core/include
            ${CMAKE_SOURCE_DIR}/src/app/include
    )
    target_link_libraries(${TEST_NAME} qtsnake_core qtsnake_app_lib Qt5::Test Qt5::Widgets Qt5::Network)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

add_core_test(test_engine core/test_engine.cpp)
add_core_test(test_state core/test_state.cpp)
add_core_test(test_replay core/test_replay.cpp)
add_core_test(test_ai core/test_ai.cpp)
add_core_test(test_logging core/test_logging.cpp)
add_app_test(test_replay_integration app/test_replay_integration.cpp)
add_app_test(test_network_minimal app/test_network_minimal.cpp)
add_app_test(test_socket_raw app/test_socket_raw.cpp)
target_link_libraries(test_socket_raw ws2_32)
add_plain_test(test_core_plain core/test_core_plain.cpp)
