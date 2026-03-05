cmake_minimum_required(VERSION 3.16)

project(qt-tcp-server VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

qt_standard_project_setup()

qt_add_executable(qt-tcp-server
    main.cpp
    mytcpserver.cpp
    mytcpserver.h
    authmanager.h
    authmanager.cpp
    dbmanager.h
    dbmanager.cpp
    serverfunc.h
    serverfunctions.h
    serverfunctions.cpp
)

target_link_libraries(qt-tcp-server PRIVATE Qt6::Core)
target_link_libraries(qt-tcp-server PRIVATE Qt6::Network)