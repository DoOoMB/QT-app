cmake_minimum_required(VERSION 3.16)

project(qt-tcp-server VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Network Sql Test)

qt_standard_project_setup()

qt_add_executable(qt-tcp-server
    main.cpp
    mytcpserver.cpp
    mytcpserver.h
    dbmanager.h
    dbmanager.cpp
    serverfunc.h
    serverfunctionsmanager.h
    serverfunctionsmanager.cpp

)

target_link_libraries(qt-tcp-server PRIVATE Qt6::Core)
target_link_libraries(qt-tcp-server PRIVATE Qt6::Network)
target_link_libraries(qt-tcp-server PRIVATE Qt6::Sql)

enable_testing()

qt_add_executable(server_tests
    tests/tst_server.cpp
    serverfunctionsmanager.cpp
    dbmanager.cpp
)

target_include_directories(server_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(server_tests
    PRIVATE
        Qt6::Core
        Qt6::Network
        Qt6::Sql
        Qt6::Test
)

add_test(NAME server_tests COMMAND server_tests)